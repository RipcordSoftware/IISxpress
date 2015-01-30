#include "StdAfx.h"
#include "IISMetaBase.h"

GUID MY_CLSID_MSAdminBase_W = { 0xa9e69610, 0xb80d, 0x11d0, { 0xb9, 0xb9, 0x0, 0xa0, 0xc9, 0x22, 0xe7, 0x50 } };
GUID MY_IID_IMSAdminBase_W = { 0x70b51430, 0xb6ca, 0x11d0, { 0xb9, 0xb9, 0x0, 0xa0, 0xc9, 0x22, 0xe7, 0x50 } };

CIISMetaBaseData::CIISMetaBaseData(void)
{
	memset(&m_Data, 0, sizeof(m_Data));
}

CIISMetaBaseData::CIISMetaBaseData(const CIISMetaBaseData& Data)
{
	memset(&m_Data, 0, sizeof(m_Data));


	// copy the info
	m_Data = Data.m_Data;

	// set the correct buffer data
	m_Data.pbMDData = NULL;
	m_Data.dwMDDataLen = 0;

	// copy the data from the incoming record
	SetData(Data.m_Data.pbMDData, Data.m_Data.dwMDDataLen);

	// store the path
	m_sKeyPath = Data.m_sKeyPath;
}

CIISMetaBaseData::~CIISMetaBaseData(void)
{
	if (m_Data.pbMDData != NULL)
	{
		delete[] m_Data.pbMDData;
	}
}

bool CIISMetaBaseData::ReadData(IMSAdminBase* pAdminBase, const WCHAR* pszKeyPath, DWORD dwIdentifier)
{
	if (pAdminBase == NULL)
		return false;

	if (pszKeyPath != NULL)
		m_sKeyPath = pszKeyPath;

	if (dwIdentifier != -1)
		m_Data.dwMDIdentifier = dwIdentifier;

	m_Data.dwMDAttributes = 0;
	m_Data.dwMDDataType = 0;
	m_Data.dwMDUserType = 0;

	DWORD dwRequiredLength = 0;
	HRESULT hr = pAdminBase->GetData(METADATA_MASTER_ROOT_HANDLE, m_sKeyPath.c_str(), &m_Data, &dwRequiredLength);
	if ((hr & ERROR_INSUFFICIENT_BUFFER) == ERROR_INSUFFICIENT_BUFFER)
	{
		if (m_Data.pbMDData != NULL)
		{
			delete[] m_Data.pbMDData;
		}

		m_Data.pbMDData = new BYTE[dwRequiredLength];
		m_Data.dwMDDataLen = dwRequiredLength;

		hr = pAdminBase->GetData(METADATA_MASTER_ROOT_HANDLE, pszKeyPath, &m_Data, &dwRequiredLength);
	}

	if (hr == S_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CIISMetaBaseData::WriteData(IMSAdminBase* pAdminBase, BYTE* pbyData, DWORD dwSize)
{
	if (pAdminBase == NULL)
		return false;

	if (pbyData != NULL)
	{
		if (SetData(pbyData, dwSize) == false)
			return false;
	}

	METADATA_HANDLE hMetaData = NULL;
	HRESULT hr = pAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE, m_sKeyPath.c_str(), MD_ACCESS_WRITE, METABASE_TIMEOUT, &hMetaData);
	if (hr == S_OK)
	{
		hr = pAdminBase->SetData(hMetaData, CComBSTR(), &m_Data);

		pAdminBase->CloseKey(hMetaData);

		if (hr == S_OK)
		{
			//pAdminBase->SaveData();	
		}
	}

	return hr == S_OK ? true : false;
}

DWORD CIISMetaBaseData::GetIdentifier(void)
{
	return m_Data.dwMDIdentifier;
}

bool CIISMetaBaseData::SetIdentifier(DWORD dwIdentifier)
{
	m_Data.dwMDIdentifier = dwIdentifier;
	return true;
}

void CIISMetaBaseData::GetKeyPath(std::wstring& sKeyPath)
{
	sKeyPath = m_sKeyPath;
}

void CIISMetaBaseData::SetKeyPath(LPCWSTR pszKeyPath)
{
	m_sKeyPath = pszKeyPath;
}

DWORD CIISMetaBaseData::GetDataSize(void)
{
	return m_Data.dwMDDataLen;
}

const BYTE* CIISMetaBaseData::GetData(void)
{
	return m_Data.pbMDData;
}

bool CIISMetaBaseData::SetData(const BYTE* pbyData, DWORD dwSize)
{
	if (pbyData == NULL)
		return false;

	if (dwSize < m_Data.dwMDDataLen)
	{
		memcpy(m_Data.pbMDData, pbyData, dwSize);
		m_Data.dwMDDataLen = dwSize;
	}
	else
	{
		if (m_Data.pbMDData != NULL)
		{
			delete[] m_Data.pbMDData;
		}

		m_Data.pbMDData = new BYTE[dwSize];
		memcpy(m_Data.pbMDData, pbyData, dwSize);
		m_Data.dwMDDataLen = dwSize;
	}

	return true;
}

DWORD CIISMetaBaseData::GetAttributes(void)
{
	return m_Data.dwMDAttributes;
}

bool CIISMetaBaseData::SetAttributes(DWORD dwAttribs)
{
	m_Data.dwMDAttributes = dwAttribs;
	return true;
}

DWORD CIISMetaBaseData::GetUserType(void)
{
	return m_Data.dwMDUserType;
}

bool CIISMetaBaseData::SetUserType(DWORD dwUserType)
{
	m_Data.dwMDUserType = dwUserType;
	return true;
}

DWORD CIISMetaBaseData::GetDataType(void)
{
	return m_Data.dwMDDataType;
}

bool CIISMetaBaseData::SetDataType(DWORD dwDataType)
{
	m_Data.dwMDDataType = dwDataType;
	return true;
}

bool CIISMetaBaseData::GetAsDWORD(DWORD* pdwData)
{
	if (pdwData != NULL && m_Data.dwMDDataType == DWORD_METADATA)
	{
		*pdwData = * (DWORD*) m_Data.pbMDData;
		return true;
	}
	else
	{
		return false;
	}
}

bool CIISMetaBaseData::SetAsDWORD(DWORD dwData)
{
	if (m_Data.dwMDDataType == DWORD_METADATA)
	{
		//* (DWORD*) m_Data.pbMDData = dwData;
		SetData((const BYTE*) &dwData, sizeof(dwData));
		return true;
	}
	else
	{
		return false;
	}
}

bool CIISMetaBaseData::GetAsString(std::wstring& sData)
{
	if (m_Data.dwMDDataType == STRING_METADATA || m_Data.dwMDDataType == EXPANDSZ_METADATA)
	{
		WCHAR* pszData = (WCHAR*) GetData();
		sData = pszData;
		return true;
	}
	else
	{
		return false;
	}
}

bool CIISMetaBaseData::SetAsString(const std::wstring& sData)
{
	if (m_Data.dwMDDataType == STRING_METADATA || m_Data.dwMDDataType == EXPANDSZ_METADATA)
	{
		return SetData((const BYTE*) sData.c_str(), (DWORD) (sData.size() + 1) * sizeof(WCHAR));
	}
	else
	{
		return false;
	}
}

bool CIISMetaBaseData::GetAsStringVector(std::vector<std::wstring>& saData)
{
	if (m_Data.dwMDDataType == MULTISZ_METADATA)
	{
		saData.clear();

		WCHAR* pszData = (WCHAR*) m_Data.pbMDData;

		while (*pszData != '\0')
		{
			std::wstring sData = pszData;
			pszData += sData.size() + 1;

			saData.push_back(sData);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CIISMetaBaseData::SetAsStringVector(const std::vector<std::wstring>& saData)
{
	if (m_Data.dwMDDataType == MULTISZ_METADATA)
	{
		bool bStatus = false;

		if (saData.size() == 0)
		{
			WCHAR szEmpty[2];
			szEmpty[0] = '\0';
			szEmpty[1] = '\0';

			bStatus = SetData((BYTE*) szEmpty, sizeof(szEmpty));
		}
		else
		{
			DWORD dwTotalSize = 0;

			// calculate the number of bytes required to store all the strings (including NULL terminators)
			for (int i = 0; i < (int) saData.size(); i++)
			{
				dwTotalSize += (DWORD) saData[i].size() + 1;
			}

			// we need space for a final terminating NULL
			dwTotalSize++;

			// allocate the buffer space
			WCHAR* pszBuffer = new WCHAR[dwTotalSize];
			WCHAR* pszData = pszBuffer;

			// copy the strings
			for (int i = 0; i < (int) saData.size(); i++)
			{
				wcscpy_s(pszData, dwTotalSize, saData[i].c_str());
				pszData += saData[i].size() + 1;
			}

			// make sure we have the final NULL
			*pszData++ = '\0';

			bStatus = SetData((BYTE*) pszBuffer, dwTotalSize * sizeof(WCHAR));

			delete[] pszBuffer;
		}

		return bStatus;
	}
	else
	{
		return false;
	}
}

void CIISMetaBaseData::SetFromRecord(const METADATA_RECORD& Record, const std::wstring& sKeyPath)
{
	// keep the information about the buffer we are managing
	BYTE* pbyBuffer = m_Data.pbMDData;
	DWORD dwBufferSize = m_Data.dwMDDataLen;

	// copy the info
	m_Data = Record;

	// restore the buffer information
	m_Data.pbMDData = pbyBuffer;
	m_Data.dwMDDataLen = dwBufferSize;

	// copy the data from the incoming record
	SetData(Record.pbMDData, Record.dwMDDataLen);

	// store the path
	m_sKeyPath = sKeyPath;
}

CIISMetaBase::CIISMetaBase(void)
{
}

CIISMetaBase::CIISMetaBase(IMSAdminBase* pAdminBase)
{
	m_pAdminBase = pAdminBase;
}

CIISMetaBase::~CIISMetaBase(void)
{
	Release();
}

bool CIISMetaBase::Init(void)
{
	// get rid of any existing metabase instances
	Release();

	CComPtr<IClassFactory> pClassFactory;
	HRESULT hr = CoGetClassObject(MY_CLSID_MSAdminBase_W, CLSCTX_SERVER, NULL, IID_IClassFactory, (void**) &pClassFactory);
	if (hr == S_OK)
	{
		hr = pClassFactory->CreateInstance(NULL, MY_IID_IMSAdminBase_W, (void **) &m_pAdminBase);
	}

	return hr == S_OK ? true : false;
}

void CIISMetaBase::Release(void)
{
	if (m_pAdminBase != NULL)
	{
		m_pAdminBase.Release();
	}
}

bool CIISMetaBase::EnumKeys(LPCTSTR pszPath, std::vector<std::wstring>& saKeys)
{
	if (pszPath == NULL || m_pAdminBase == NULL)
		return false;

	saKeys.clear();

	CComBSTR bsPath = pszPath;

	DWORD dwKey = 0;
	WCHAR wszKeyName[METADATA_MAX_NAME_LEN];
	while (1)
	{
		HRESULT hr = m_pAdminBase->EnumKeys(METADATA_MASTER_ROOT_HANDLE, bsPath, wszKeyName, dwKey);
		if (hr == ERROR_NO_MORE_ITEMS || hr != S_OK)
		{
			break;
		}

		dwKey++;

		saKeys.push_back(wszKeyName);
	};

	return true;
}

bool CIISMetaBase::EnumKeys(LPCTSTR pszPath, std::set<std::wstring>& Keys)
{
	Keys.clear();

	std::vector<std::wstring> saKeys;
	if (EnumKeys(pszPath, saKeys) == true)
	{
		int nKeys = (int) saKeys.size();
		for (int i = 0; i < nKeys; i++)
		{
			Keys.insert(saKeys[i]);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CIISMetaBase::EnumData(LPCTSTR pszPath, std::vector<CIISMetaBaseData>& EnumData)
{
	BYTE* pbyBuffer = new BYTE[64 * 1024];
	DWORD dwBufferSize = 64 * 1024;

	HRESULT hr = S_OK;
	for (DWORD dwDataIndex = 0; SUCCEEDED(hr); dwDataIndex++)
	{
		METADATA_RECORD MetaData;
		memset(&MetaData, 0, sizeof(MetaData));

		MetaData.pbMDData = pbyBuffer;
		MetaData.dwMDDataLen = dwBufferSize;

		CComBSTR bsDataPath = pszPath;

		DWORD dwRequiredLength = 0;
		hr = m_pAdminBase->EnumData(METADATA_MASTER_ROOT_HANDLE, bsDataPath, &MetaData, dwDataIndex, &dwRequiredLength);

		if ((hr & ERROR_INSUFFICIENT_BUFFER) == ERROR_INSUFFICIENT_BUFFER)
		{
			pbyBuffer = new BYTE[dwRequiredLength];
			dwBufferSize = dwRequiredLength;

			MetaData.pbMDData = pbyBuffer;
			MetaData.dwMDDataLen = dwBufferSize;

			dwRequiredLength = 0;
			hr = m_pAdminBase->EnumData(METADATA_MASTER_ROOT_HANDLE, bsDataPath, &MetaData, dwDataIndex, &dwRequiredLength);
		}

		if (SUCCEEDED(hr) == TRUE)
		{
			CIISMetaBaseData Data;
			Data.SetFromRecord(MetaData, std::wstring(bsDataPath));

			EnumData.push_back(Data);
		}
	}

	delete[] pbyBuffer;

	return true;
}

bool CIISMetaBase::EnumIdentifiers(LPCTSTR pszPath, std::set<DWORD>& Identifiers)
{
	BYTE* pbyBuffer = new BYTE[64 * 1024];	
	DWORD dwBufferSize = 64 * 1024;	

	HRESULT hr = S_OK;
	for (DWORD dwDataIndex = 0; SUCCEEDED(hr); dwDataIndex++)
	{
		METADATA_RECORD MetaData;
		memset(&MetaData, 0, sizeof(MetaData));

		MetaData.pbMDData = pbyBuffer;
		MetaData.dwMDDataLen = dwBufferSize;

		CComBSTR bsDataPath = pszPath;

		DWORD dwRequiredLength = 0;
		hr = m_pAdminBase->EnumData(METADATA_MASTER_ROOT_HANDLE, bsDataPath, &MetaData, dwDataIndex, &dwRequiredLength);

		if ((hr & ERROR_INSUFFICIENT_BUFFER) == ERROR_INSUFFICIENT_BUFFER)
		{
			pbyBuffer = new BYTE[dwRequiredLength];
			dwBufferSize = dwRequiredLength;

			MetaData.pbMDData = pbyBuffer;
			MetaData.dwMDDataLen = dwBufferSize;

			dwRequiredLength = 0;
			hr = m_pAdminBase->EnumData(METADATA_MASTER_ROOT_HANDLE, bsDataPath, &MetaData, dwDataIndex, &dwRequiredLength);
		}

		if (SUCCEEDED(hr) == TRUE)
		{
			Identifiers.insert(MetaData.dwMDIdentifier);
		}
	}

	delete[] pbyBuffer;

	return true;
}

bool CIISMetaBase::GetAdminBase(IMSAdminBase** ppAdminBase)
{
	if (ppAdminBase == NULL || m_pAdminBase == NULL)
		return false;

	return m_pAdminBase.CopyTo(ppAdminBase) == S_OK ? true : false;	 
} 