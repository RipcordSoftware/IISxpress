#include "StdAfx.h"
#include "myhttpresponsedata.h"

CMyHTTPResponseData::CMyHTTPResponseData(void)
{	
	m_bGotHeader = false;

	// reserve at least 1k of space for the header
	m_sHeader.reserve(1024);

	// initialise the buffer to hold the response body
	m_pbyData = NULL;
	m_dwDataSize = 0;
	m_dwAllocated = 0;

	m_dwInitialAllocSize = HTTPREQUEST_BLOCKSIZE;
	m_dwAllocBlockSize = HTTPREQUEST_BLOCKSIZE;
}

CMyHTTPResponseData::~CMyHTTPResponseData(void)
{
	if (m_pbyData != NULL)
	{
		free(m_pbyData);
	}
}

bool CMyHTTPResponseData::GotHeader(void)
{
	return m_bGotHeader;
}

bool CMyHTTPResponseData::Init(bool bResetBuffers, DWORD dwInitialAllocSize, DWORD dwAllocBlockSize)
{
	m_bGotHeader = false;
	m_sHeader.clear();

	m_Params.clear();

	m_sResponseCode.clear();
	m_sResponseString.clear();
	m_sHTTPType.clear();	

	m_dwDataSize = 0;

	if (bResetBuffers == true)
	{
		// reset means no memory allocated
		if (m_pbyData != NULL)
			free(m_pbyData);

		m_pbyData = NULL;
		m_dwAllocated = 0;
	}
	else if (bResetBuffers == false && dwInitialAllocSize > m_dwAllocated)
	{
		// if we are not resetting and we haven't enough memory for the initial block
		// then we need to get more memory
		BYTE* pbyTemp = (BYTE*) malloc(dwInitialAllocSize);
		if (pbyTemp != NULL)
		{
			if (m_pbyData != NULL)
				free(m_pbyData);

			m_pbyData = pbyTemp;
			m_dwAllocated = dwInitialAllocSize;
		}
	}
	
	m_dwInitialAllocSize = dwInitialAllocSize;
	m_dwAllocBlockSize = dwAllocBlockSize;

	return true;
}

bool CMyHTTPResponseData::AppendData(const char* pszData, DWORD dwSize, bool bHasHeader)
{
	if (m_bGotHeader == true || bHasHeader == false)
		return AppendBody((const BYTE*) pszData, dwSize);
	else
		return AppendHeader(pszData, dwSize);
}

bool CMyHTTPResponseData::AppendData(const BYTE* pbyData, DWORD dwSize, bool bHasHeader)
{
	return AppendData((const char*) pbyData, dwSize, bHasHeader);
}

HRESULT CMyHTTPResponseData::AppendData(IStream* pStream, DWORD dwSize, bool bHasHeader)
{
	if (m_bGotHeader == true || bHasHeader == false)
	{
		return AppendBody(pStream, dwSize);
	}
	else
	{		
		return AppendHeader(pStream, dwSize);
	}	
}

HRESULT CMyHTTPResponseData::AppendData(IStream* pStream, bool bHasHeader)
{
	if (pStream == NULL)
		return E_POINTER;	
	
	// get the amount of data waiting in the stream
	LARGE_INTEGER Offset = { 0, 0 };
	ULARGE_INTEGER nDataLength;
	HRESULT hr = pStream->Seek(Offset, STREAM_SEEK_CUR, &nDataLength);
	if (hr != S_OK)
	{
		return S_OK;
	}

	// make sure the stream pointer is at the beginning	
	hr = pStream->Seek(Offset, STREAM_SEEK_SET, NULL);
	if (hr != S_OK)
		return hr;	

	return AppendData(pStream, nDataLength.LowPart, bHasHeader);
}

bool CMyHTTPResponseData::AppendHeader(const char* pszData, DWORD dwSize)
{
	DWORD i = 0;
	for (; i < dwSize; i++)
	{
		m_sHeader += pszData[i];

		if (pszData[i] == '\n')
		{
			if (m_sHeader.find("\r\n\r\n") != string::npos)
			{
				m_bGotHeader = true;
				i++;
				break;
			}
		}
	}

	if (m_bGotHeader == true)
	{
		ParseHeader();

		// make sure we capture any other data in the buffer
		DWORD dwBytesRemaining = dwSize - i;
		if (dwBytesRemaining > 0)
			AppendBody((const BYTE*) pszData + i, dwBytesRemaining);
	}

	return true;
}

HRESULT CMyHTTPResponseData::AppendHeader(IStream* pStream, DWORD dwSize)
{
	if (pStream == NULL || dwSize == 0)
		return E_INVALIDARG;	

	// try to allocate from the stack first
	BYTE* pbyData = NULL;
	if (dwSize <= 512)
	{
		pbyData = (BYTE*) alloca(dwSize);	
	}

	// we need a managed array type incase we decide to allocate on the heap
	Ripcord::Types::AutoArray<BYTE> pAutoData;

	if (pbyData == NULL)
	{
		pbyData = new BYTE[dwSize];
		if (pbyData == NULL)
			return E_OUTOFMEMORY;

		// attach the heap allocated block to a managed type
		pAutoData = Ripcord::Types::AutoArray<BYTE>(pbyData);
	}		

	// read from the stream
	ULONG nRead = 0;
	HRESULT hr = pStream->Read(pbyData, (ULONG) dwSize, &nRead);	
	if (hr == S_OK)
	{			
		ATLASSERT(dwSize == nRead);

		// append the data
		hr = AppendHeader((const char*) pbyData, dwSize) == true ? S_OK : E_FAIL;
	}	

	return hr;
}

bool CMyHTTPResponseData::ReallocBlock(DWORD dwBlockSize)
{
	if (m_pbyData == NULL)
	{
		m_pbyData = (BYTE*) malloc(m_dwInitialAllocSize);
		if (m_pbyData == NULL)
			return false;

		m_dwAllocated = m_dwInitialAllocSize;
	}
	else
	{
		if (dwBlockSize < m_dwAllocBlockSize)
			dwBlockSize = m_dwAllocBlockSize;

		BYTE* pbyData = (BYTE*) realloc(m_pbyData, m_dwAllocated + dwBlockSize);
		if (pbyData == NULL)
			return false;

		m_pbyData = pbyData;
		m_dwAllocated += dwBlockSize;
	}

	return true;
}

bool CMyHTTPResponseData::AppendBody(const BYTE* pbyData, DWORD dwSize)
{
	if (pbyData == NULL || dwSize == 0)
		return false;

	if ((m_dwDataSize + dwSize) > m_dwAllocated)
	{
		ReallocBlock(dwSize);		
	}

	memcpy(m_pbyData + m_dwDataSize, pbyData, dwSize);
	m_dwDataSize += dwSize;

	return true;
}

HRESULT CMyHTTPResponseData::AppendBody(IStream* pStream, DWORD dwSize)
{
	if (pStream == NULL || dwSize == 0)
		return E_INVALIDARG;

	if ((m_dwDataSize + dwSize) > m_dwAllocated)
	{
		ReallocBlock(dwSize);		
	}

	ULONG nRead = 0;
	HRESULT hr = pStream->Read(m_pbyData + m_dwDataSize, (ULONG) dwSize, &nRead);	
	if (hr == S_OK)
	{
		ATLASSERT(dwSize == nRead);
		m_dwDataSize += dwSize;
	}

	return hr;
}

const BYTE* CMyHTTPResponseData::GetBody(void) const
{
	return m_pbyData;
}

DWORD CMyHTTPResponseData::GetBodySize(void) const
{
	return m_dwDataSize;
}

bool CMyHTTPResponseData::GetHTTPType(string& sHTTPType)
{
	if (m_bGotHeader == false)
		return false;

	sHTTPType = m_sHTTPType;
	return true;
}

bool CMyHTTPResponseData::GetParam(const char* pszParam, string& sValue)
{
	if (m_bGotHeader == false || pszParam == NULL)
		return false;

	map<string, vector<string> >::const_iterator iter = m_Params.find(pszParam);
	if (iter == m_Params.end() || iter->second.size() != 1)
		return false;

	sValue = iter->second[0];
	return true;
}

bool CMyHTTPResponseData::SetParam(const char* pszParam, const char* pszValue)
{
	if (m_bGotHeader == false || pszParam == NULL || pszValue == NULL)
		return false;

	size_t nSize = m_Params[pszParam].size();
	if (nSize == 0)
	{
		// if's a new parameter we can safely add it
		m_Params[pszParam].push_back(pszValue);
		return true;
	}
	else if (nSize == 1)
	{
		// it it's an existing parameter and it only has one entry then we are safe to overwrite it
		m_Params[pszParam][0] = pszValue;
		return true;
	}
	else
	{
		// there is more than 1 existing entry, it's an error since we don't know which one to update
		return false;
	}	
}

bool CMyHTTPResponseData::ParseHeader(void)
{
	if (m_bGotHeader != true && m_sHeader.length() == 0)
		return false;

	int nHeaderSize = (int) strlen(m_sHeader.c_str());

	char* pszHeader = (char*) alloca(nHeaderSize + 1);
	memcpy(pszHeader, m_sHeader.c_str(), nHeaderSize + 1);	

	char* ppszLines[256];
	memset(ppszLines, NULL, sizeof(ppszLines));

	char* pszLineContext = NULL;
	char* pszLine = strtok_s(pszHeader, "\r\n", &pszLineContext);
	int i = 0;
	for (; i < sizeof(ppszLines) / sizeof(ppszLines[0]); i++)
	{
		if (pszLine == NULL)
			break;

		ppszLines[i] = pszLine;

		pszLine = strtok_s(NULL, "\r\n", &pszLineContext);
	}

	int nLines = i;
	if (nLines <= 0)
		return false;

	// we only support responses
	if (strncmp(ppszLines[0], "HTTP/", 5) != 0)
		return false;	
	
	char* pszHeaderContext = NULL;
	
	char* pszHTTPType = strtok_s(ppszLines[0], " ", &pszHeaderContext);
	if (pszHTTPType == NULL)
		return false;

	char* pszResponseCode = strtok_s(NULL, " ", &pszHeaderContext);
	if (pszResponseCode == NULL)
		return false;

	char* pszResponseString = strtok_s(NULL, "\r\n", &pszHeaderContext);
	if (pszResponseString == NULL)
		return false;

	m_sHTTPType = pszHTTPType;
	m_sResponseCode = pszResponseCode;
	m_sResponseString = pszResponseString;		
	
	for (i = 1; i < nLines; i++)
	{
		char* pszContext = NULL;
		char* pszParam = strtok_s(ppszLines[i], ":", &pszContext);
		if (pszParam == NULL)
			continue;

		char* pszValue = strtok_s(NULL, "\r\n", &pszContext);

		while (*pszValue != '\0' && *pszValue == ' ')
			pszValue++;

		m_Params[pszParam].push_back(pszValue);
	}

	return true;
}

bool CMyHTTPResponseData::SetBody(const BYTE* pbyData, DWORD dwSize)
{
	m_dwDataSize = 0;
	return AppendBody(pbyData, dwSize);
}

bool CMyHTTPResponseData::GetHTTPHeader(string& sHeader)
{
	if (m_bGotHeader == false)
		return false;

	sHeader.clear();
	sHeader.reserve(1024);

	sHeader += m_sHTTPType;
	sHeader += " ";
	sHeader += m_sResponseCode;
	sHeader += " ";
	sHeader += m_sResponseString;
	sHeader += "\r\n";	

	map<string, vector<string> >::const_iterator iter = m_Params.begin();
	while (iter != m_Params.end())
	{
		for (size_t i = 0; i < (int) iter->second.size(); i++)
		{
			sHeader += iter->first;
			sHeader += ": ";
			sHeader += iter->second[i];
			sHeader += "\r\n";
		}

		iter++;
	}

	sHeader += "\r\n";

	return true;
}

bool CMyHTTPResponseData::GetHTTPData(BYTE* pbyData, DWORD* pdwSize)
{
	if (pbyData == false || pdwSize == NULL)
		return false;

	// we may not have a header
	string sHeader;
	if (m_bGotHeader == true)
	{	
		if (GetHTTPHeader(sHeader) == false)
			return false;
	}

	DWORD dwHeaderSize = (DWORD) sHeader.size();
	DWORD dwTotalSize = dwHeaderSize + m_dwDataSize;
	if (dwTotalSize > *pdwSize)
		return false;

	if (dwHeaderSize > 0)
		memcpy(pbyData, sHeader.c_str(), dwHeaderSize);	

	if (m_dwDataSize > 0 && m_pbyData != NULL)
		memcpy(pbyData + dwHeaderSize, m_pbyData, m_dwDataSize);

	*pdwSize = dwHeaderSize + m_dwDataSize;

	return true;
}

int CMyHTTPResponseData::GetResponseCode(void)
{
	if (m_bGotHeader == false)
		return -1;

	return atoi(m_sResponseCode.c_str());
}

bool CMyHTTPResponseData::GetResponseCode(string& sResponseCode)
{
	if (m_bGotHeader == false)
		return false;

	sResponseCode = m_sResponseCode;
	return true;
}

bool CMyHTTPResponseData::GetResponseString(string& sResponseString)
{
	if (m_bGotHeader == false)
		return false;

	sResponseString = m_sResponseString;
	return true;
}

DWORD CMyHTTPResponseData::GetAllocatedBufferSize(void)
{
	return m_dwAllocated;
}