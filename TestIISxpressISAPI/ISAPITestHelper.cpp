#include "StdAfx.h"
#include "ISAPITestHelper.h"

#include "AutoMemberCriticalSection.h"

CComAutoCriticalSection CISAPITestHelper::m_csState;
std::map<HTTP_FILTER_CONTEXT*, CISAPITestHelper*> CISAPITestHelper::m_State;

CComAutoCriticalSection CISAPITestHelper::m_csFileDataCache;
std::map<std::string, std::vector<BYTE> > CISAPITestHelper::m_FileDataCache;

CISAPITestHelper::CISAPITestHelper(HTTP_FILTER_CONTEXT& context) : m_pContext(NULL), m_dwDisableNotifications(0)
{
	context.AddResponseHeaders = AddResponseHeaders;
	context.AllocMem = AllocMem;
	context.GetServerVariable = GetServerVariable;
	context.ServerSupportFunction = ServerSupportFunction;
	context.WriteClient = WriteClient;

	AutoMemberCriticalSection cs(&m_csState);
	m_pContext = &context;
	m_State[m_pContext] = this;
}

CISAPITestHelper::~CISAPITestHelper()
{
	AutoMemberCriticalSection cs(&m_csState);
	m_State.erase(m_State.find(m_pContext));
	m_pContext = NULL;
}

void CISAPITestHelper::Attach(HTTP_FILTER_SEND_RESPONSE& response)
{	
	response.AddHeader = AddHeader;
	response.GetHeader = GetHeader;
	response.SetHeader = SetHeader;
}

BOOL WINAPI CISAPITestHelper::GetServerVariable(struct _HTTP_FILTER_CONTEXT * pfc, LPSTR lpszVariableName, LPVOID lpvBuffer, LPDWORD lpdwSize)
{
	AutoMemberCriticalSection cs(&m_csState);

	CISAPITestHelper* pThis = m_State[pfc];

	std::map<std::string, std::string>::const_iterator iter = pThis->m_ServerVariables.find(lpszVariableName);
	if (iter == pThis->m_ServerVariables.end())
	{
		return FALSE;
	}

	strcpy_s((char*) lpvBuffer, *lpdwSize, iter->second.c_str());

	if (iter->second.size() < *lpdwSize)
	{
		*lpdwSize = (DWORD) iter->second.size() + 1;
	}

	return TRUE;
}

BOOL WINAPI CISAPITestHelper::AddResponseHeaders(struct _HTTP_FILTER_CONTEXT * pfc, LPSTR lpszHeaders, DWORD dwReserved)
{
	return TRUE;
}

BOOL WINAPI CISAPITestHelper::WriteClient(struct _HTTP_FILTER_CONTEXT * pfc, LPVOID Buffer, LPDWORD lpdwBytes, DWORD dwReserved)
{
	AutoMemberCriticalSection cs(&m_csState);

	BYTE* BufferStart = (BYTE*) Buffer;
	m_State[pfc]->m_WriteClientResponse.insert(m_State[pfc]->m_WriteClientResponse.end(), BufferStart, BufferStart + *lpdwBytes);

	return TRUE;
}

void CISAPITestHelper::GetWriteClientResponse(HTTP_FILTER_CONTEXT* pCtxt, std::vector<BYTE>& data)
{
	AutoMemberCriticalSection cs(&m_csState);
	CISAPITestHelper* pThis = m_State[pCtxt];

	data.assign(pThis->m_WriteClientResponse.begin(), pThis->m_WriteClientResponse.end());
}

void* WINAPI CISAPITestHelper::AllocMem(struct _HTTP_FILTER_CONTEXT* pfc, ULONG_PTR cbSize, ULONG_PTR dwReserved)
{
	AutoMemberCriticalSection cs(&m_csState);
	CISAPITestHelper* pThis = m_State[pfc];
	return pThis->AllocateBlock(cbSize);	
}

BOOL WINAPI CISAPITestHelper::ServerSupportFunction(struct _HTTP_FILTER_CONTEXT* pfc, enum SF_REQ_TYPE sfReq, PVOID pData, ULONG_PTR ul1, ULONG_PTR ul2)
{
	if (sfReq == SF_REQ_DISABLE_NOTIFICATIONS)
	{
		SetDisableNotificationsFlags(pfc, ul1);
	}

	return TRUE;
}

BOOL WINAPI CISAPITestHelper::GetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPCSTR lpszName, LPVOID lpvBuffer, LPDWORD lpdwSize)
{
	std::string name(lpszName);
	LowerString(name);

	AutoMemberCriticalSection cs(&m_csState);
	CISAPITestHelper* pThis = m_State[pfc];

	std::map<std::string, std::string>::const_iterator iter = pThis->m_Headers.find(name);
	if (iter == pThis->m_Headers.end())
	{
		return FALSE;
	}

	strcpy_s((char*) lpvBuffer, *lpdwSize, iter->second.c_str());

	if (iter->second.size() < *lpdwSize)
	{
		*lpdwSize = (DWORD) iter->second.size() + 1;
	}

	return TRUE;
}

BOOL WINAPI CISAPITestHelper::SetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPCSTR lpszName, LPCSTR lpszValue)
{
	std::string name(lpszName);
	LowerString(name);

	AutoMemberCriticalSection cs(&m_csState);
	CISAPITestHelper* pThis = m_State[pfc];

	std::map<std::string, std::string>::iterator iter = pThis->m_Headers.find(name);
	if (iter == pThis->m_Headers.end())
	{
		return FALSE;
	}

	if (*lpszValue != '\0')
	{
		iter->second = lpszValue;
	}
	else
	{
		pThis->m_Headers.erase(iter);
		iter = pThis->m_Headers.end();
	}

	return TRUE;
}

BOOL WINAPI CISAPITestHelper::AddHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPCSTR lpszName, LPCSTR lpszValue)
{
	std::string name(lpszName);
	LowerString(name);

	AutoMemberCriticalSection cs(&m_csState);
	CISAPITestHelper* pThis = m_State[pfc];

	std::map<std::string, std::string>::const_iterator iter = pThis->m_Headers.find(name);
	if (iter != pThis->m_Headers.end())
	{
		return FALSE;
	}

	pThis->m_Headers[name] = lpszValue;

	return TRUE;
}

BOOL WINAPI CISAPITestHelper::GetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPSTR lpszName, LPVOID lpvBuffer, LPDWORD lpdwSize)
{
	return GetHeader(pfc, (LPCSTR) lpszName, lpvBuffer, lpdwSize);
}

BOOL WINAPI CISAPITestHelper::SetHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPSTR lpszName, LPSTR lpszValue)
{
	return SetHeader(pfc, (LPCSTR) lpszName, (LPCSTR) lpszValue);
}

BOOL WINAPI CISAPITestHelper::AddHeader(struct _HTTP_FILTER_CONTEXT* pfc, LPSTR lpszName, LPSTR lpszValue)
{
	return AddHeader(pfc, (LPCSTR) lpszName, (LPCSTR) lpszValue);
}

void CISAPITestHelper::SetDisableNotificationsFlags(HTTP_FILTER_CONTEXT* pCtxt) 
{ 
	AutoMemberCriticalSection cs(&m_csState);
	m_State[pCtxt]->m_dwDisableNotifications = 0; 
}

void CISAPITestHelper::SetDisableNotificationsFlags(HTTP_FILTER_CONTEXT* pCtxt, DWORD dwDisableNotifications)
{ 
	AutoMemberCriticalSection cs(&m_csState);
	m_State[pCtxt]->m_dwDisableNotifications = dwDisableNotifications;	
}

DWORD CISAPITestHelper::GetDisableNotificationsFlags(HTTP_FILTER_CONTEXT* pCtxt) 
{ 
	AutoMemberCriticalSection cs(&m_csState);
	DWORD dwDisableNotifications = m_State[pCtxt]->m_dwDisableNotifications;
	return dwDisableNotifications; 
}

BYTE* CISAPITestHelper::AllocateBlock(int nSize)
{
	if (nSize <= 0)
	{
		return NULL;
	}

	m_AllocatedBlocks.push_back(std::vector<BYTE>(nSize));

	std::vector<std::vector<BYTE> >::iterator iter = --m_AllocatedBlocks.end();

	return &(*iter)[0];
}

void CISAPITestHelper::SetServerVariable(LPSTR lpszVariableName, const char* pszValue)
{
	m_ServerVariables[lpszVariableName] = pszValue;
}

void CISAPITestHelper::SetServerVariable(LPSTR lpszVariableName, LPVOID lpvBuffer)
{
	m_ServerVariables[lpszVariableName] = (char*) lpvBuffer;
}

void CISAPITestHelper::LowerString(std::string& text)
{
	for (size_t i = 0; i < text.size(); i++)
	{
		if (text[i] >= 'A' && text[i] <= 'Z')
		{
			text[i] += 0x20;
		}
	}
}

bool CISAPITestHelper::LoadFileData(LPCSTR pszFileName, AutoArray<BYTE>& fileData, DWORD& dwFileLength)
{
	if (pszFileName == NULL)
	{
		return false;
	}

	if (m_FileDataCache.find(pszFileName) != m_FileDataCache.end())
	{
		AutoMemberCriticalSection cs(&m_csFileDataCache);

		FileCacheType::const_iterator iter = m_FileDataCache.find(pszFileName);
		if (iter != m_FileDataCache.end())
		{	
			dwFileLength = (DWORD) iter->second.size();			
			AutoArray<BYTE> pbyData(dwFileLength);
			std::copy(iter->second.begin(), iter->second.end(), (BYTE*) pbyData);
			
			fileData = pbyData;
			return true;
		}
	}

	CFile file;	
	if (file.Open(CString(pszFileName), CFile::modeRead | CFile::shareDenyWrite) == FALSE)
	{
		return false;
	}

	dwFileLength = (DWORD) file.GetLength();
	AutoArray<BYTE> pbyData(dwFileLength);
	file.Read(pbyData, dwFileLength);

	fileData = pbyData;

	if (m_FileDataCache.find(pszFileName) == m_FileDataCache.end())
	{
		AutoMemberCriticalSection cs(&m_csFileDataCache);

		FileCacheType::const_iterator iter = m_FileDataCache.find(pszFileName);
		if (iter == m_FileDataCache.end())
		{	
			std::vector<BYTE> data(dwFileLength, 0);
			std::copy((BYTE*) fileData, (BYTE*) fileData + dwFileLength, data.begin());
			m_FileDataCache[pszFileName] = data;
		}
	}

	return true;
}