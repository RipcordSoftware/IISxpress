// ProductLicenseManager.cpp : Implementation of CProductLicenseManager

#include "stdafx.h"
#include "ProductLicenseManager.h"

#include "ProductKeyRegistryGuids.h"

const static TCHAR* g_szRegistryKeys[] = 
{
	PRODUCTLICENCE_REGKEY1, PRODUCTLICENCE_REGKEY2, PRODUCTLICENCE_REGKEY3, PRODUCTLICENCE_REGKEY4, PRODUCTLICENCE_REGKEY5,
	PRODUCTLICENCE_REGKEY6, PRODUCTLICENCE_REGKEY7, PRODUCTLICENCE_REGKEY8, PRODUCTLICENCE_REGKEY9, PRODUCTLICENCE_REGKEY10
};

typedef struct tagBitmapInResourceHeader
{
	DWORD		dwBIHSize;
	DWORD		dwWidth;
	DWORD		dwHeight;
	WORD		wPlanes;
	WORD		wBPP;
	DWORD		dwCompressionType;
	DWORD		dwImageSize;	
} BitmapInResourceHeader;

// CProductLicenseManager

HRESULT CProductLicenseManager::FinalConstruct()
{
	// load the license key from the resource file
	HRSRC hrsrcKey = ::FindResource(NULL, MAKEINTRESOURCE(IDB_BITMAP_ABOUTKEY), RT_BITMAP);
	if (hrsrcKey != NULL)
	{
		// get the size of the resource as far as Windows is concerned
		DWORD dwKeyBitmapSize = ::SizeofResource(NULL, hrsrcKey);

		// load the resource
		HGLOBAL hKeyBitmap = ::LoadResource(NULL, hrsrcKey);
		if (hKeyBitmap != NULL)
		{
			void* pKeyBitmap = ::LockResource(hKeyBitmap);

			// get hold of the bitmap info
			BitmapInResourceHeader* pBitmap = (BitmapInResourceHeader*) pKeyBitmap;			

			// calculate the offset of the key and it's size
			DWORD dwKeyOffset = pBitmap->dwBIHSize + pBitmap->dwImageSize;
			DWORD dwKeySize = dwKeyBitmapSize - dwKeyOffset;

			char* pszKey = (char*) pKeyBitmap;
			pszKey += dwKeyOffset;

			m_sEncryptionKey.Empty();
			m_sEncryptionKey.Append(pszKey, dwKeySize);			
		}
	}

	// load the key up from the registry
	CStringA sProductKey;
	if (LoadProductKeyFromRegistry(sProductKey) == S_OK)
	{
		// check the format of the string
		ProductKeyData KeyData;
		if (CProductLicense::ParseProductKeyString(sProductKey, KeyData) == true)
		{
			// check the key
			bool bStrongKey = false;
			bool bRegistered = false;
			HRESULT hr = m_License.CheckProductKey(KeyData, 
					(const BYTE*) (const char*) m_sEncryptionKey, m_sEncryptionKey.GetLength(), 
					bStrongKey, bRegistered);

			// save the state of the key
			if (SUCCEEDED(hr) == TRUE)
			{
				m_pProductKey = new ProductKeyData;
				*m_pProductKey = KeyData;
			}
		}
	}

	return S_OK;
}

void CProductLicenseManager::FinalRelease() 
{
}

// returns:
//  S_OK			- the key is valid and has not yet expired
//  S_FALSE         - the key is valid but has expired
//  E_POINTER       - no license key present
STDMETHODIMP CProductLicenseManager::IsValidKey(void)
{
	if (m_pProductKey == NULL)
		return E_POINTER;

	bool bStrongKey = false;
	bool bRegistered = false;
	return m_License.CheckProductKey(*m_pProductKey, 
			(const BYTE*) (const char*) m_sEncryptionKey, m_sEncryptionKey.GetLength(),
			bStrongKey, bRegistered);
}

// returns:
//  S_OK			-	the key is registered
//  S_FALSE         -   the key is not registered
STDMETHODIMP CProductLicenseManager::IsRegisteredKey(void)
{
	if (m_pProductKey == NULL)
		return E_POINTER;

	bool bStrongKey = false;
	bool bRegistered = false;
	HRESULT hr = m_License.CheckProductKey(*m_pProductKey, 
			(const BYTE*) (const char*) m_sEncryptionKey, m_sEncryptionKey.GetLength(),
			bStrongKey, bRegistered);

	if (hr == S_OK)
	{
		return bRegistered == true ? S_OK : S_FALSE;
	}
	else
	{
		return hr;
	}
}

// returns:
//  S_OK			-	the key is valid and has not yet or will not expire
//  S_FALSE			-	tke key was valid but it has expired
//  E_INVALIDARG	-	the key is badly formatted
STDMETHODIMP CProductLicenseManager::SetKey(LPCSTR pszKey, BOOL bAllowTrial)
{
	// check the format of the string
	ProductKeyData KeyData;
	if (CProductLicense::ParseProductKeyString(pszKey, KeyData) == false)
	{
		return E_INVALIDARG;
	}

	bool bStrongKey = false;
	bool bRegistered = false;
	HRESULT hr = m_License.CheckProductKey(KeyData, 
			(const BYTE*) (const char*) m_sEncryptionKey, m_sEncryptionKey.GetLength(), 
			bStrongKey, bRegistered);

	// if there are no errors store the key data
	if (hr == S_OK)
	{
		// don't allow the key to be overwritten unless it's either strong or registered
		if (m_pProductKey != NULL && bStrongKey == false && bRegistered == false)
		{
			hr = E_INVALIDARG;
		}
		else if (bAllowTrial == FALSE && bRegistered == false)
		{
			// trials are not allowed
			hr = E_INVALIDARG;
		}
		else
		{
			delete m_pProductKey;
			m_pProductKey = new ProductKeyData;
			*m_pProductKey = KeyData;

			SaveProductKeyToRegistry();
		}
	}

	return hr;
}

// returns:
//  S_OK			- the expiry month/day is returned from the key
//  S_FALSE         - the key is valid but does not expire
//  E_POINTER       - no license key present
STDMETHODIMP CProductLicenseManager::GetExpiry(SYSTEMTIME* pstExpiry)
{
	if (pstExpiry == NULL)
		return E_INVALIDARG;

	if (m_pProductKey == NULL)
		return E_POINTER;

	HRESULT hr = m_License.GetKeyExpiry(*m_pProductKey, 
			(const BYTE*) (const char*) m_sEncryptionKey, 
			m_sEncryptionKey.GetLength(), *pstExpiry);

	// if it's a bad failure pretend the key isn't present
	if (FAILED(hr) == TRUE)
	{
		hr = E_POINTER;
	}

	return hr;
}

// open a product registry key, obfuscate with lots of other reads around the area of interest
HRESULT CProductLicenseManager::LoadProductKeyFromRegistry(CStringA& sProductKey)
{
	srand(::GetTickCount());

	for (int i = 0; i < PRODUCTLICENCE_MAXKEY;)
	{
		CString sPath = _T("CLSID\\");
		CRegKey ProductKey;		

		if ((rand() & 0x07) != 0)
		{
			UUID guid;
			::UuidCreate(&guid);

			TCHAR* pszGuid = NULL;
#ifndef _UNICODE
			::UuidToString(&guid, (unsigned char**) &pszGuid);
#else
			::UuidToString(&guid, (unsigned short**) &pszGuid);
#endif

			sPath += pszGuid;

#ifndef _UNICODE
			::RpcStringFree((unsigned char**) &pszGuid);
#else
			::RpcStringFree((unsigned short**) &pszGuid);
#endif

			if (ProductKey.Open(HKEY_CLASSES_ROOT, sPath, KEY_READ) == ERROR_SUCCESS)
			{
				// don't do anything here!!!!
			}
		}
		else
		{
			sPath += g_szRegistryKeys[i];
			
			if (ProductKey.Open(HKEY_CLASSES_ROOT, sPath, KEY_READ) == ERROR_SUCCESS)
			{
				TCHAR szKeyBuffer[64] = _T("");
				ULONG nKeyBufferSize = sizeof(szKeyBuffer) / sizeof(szKeyBuffer[0]);

				if (ProductKey.QueryStringValue(NULL, szKeyBuffer, &nKeyBufferSize) == ERROR_SUCCESS)
				{
					sProductKey = szKeyBuffer;

					// restore the string to it's original order
					sProductKey.MakeReverse();

					return S_OK;
				}
			}

			i++;
		}
	}

	return S_FALSE;
}

// delete the known registry keys, obfuscate with lots of other delets around the area of interest
HRESULT CProductLicenseManager::DeleteKnownRegistryKeys(void)
{
	CRegKey ProductKey;
	if (ProductKey.Open(HKEY_CLASSES_ROOT, _T("CLSID"), KEY_SET_VALUE) != ERROR_SUCCESS)
		return E_FAIL;

	srand(::GetTickCount());
	
	for (int i = 0; i < PRODUCTLICENCE_MAXKEY;)
	{
		if ((rand() & 0x07) != 0)
		{
			UUID guid;
			::UuidCreate(&guid);

			TCHAR* pszGuid = NULL;
#ifndef _UNICODE
			::UuidToString(&guid, (unsigned char**) &pszGuid);
#else
			::UuidToString(&guid, (unsigned short**) &pszGuid);
#endif

			CString sKey = pszGuid;

#ifndef _UNICODE
			::RpcStringFree((unsigned char**) &pszGuid);
#else
			::RpcStringFree((unsigned short**) &pszGuid);
#endif			
				
			ProductKey.DeleteSubKey(sKey);
		}
		else
		{
			ProductKey.DeleteSubKey(g_szRegistryKeys[i]);

			i++;
		}
	}

	return S_OK;
}

HRESULT CProductLicenseManager::SaveProductKeyToRegistry(void)
{
	if (m_pProductKey == NULL)
		return E_FAIL;

	// get the product key as a string
	CString sProductKey;
	m_License.ConvertProductKeyToFriendlyString(*m_pProductKey, sProductKey);
	
	srand(::GetTickCount());

	// calculate how many times to loop (between 20 and 40)
	int nMaxLoop = rand() % 20;
	nMaxLoop += 20;

	// calculate an index to write at
	int nInsertIndex = rand() % nMaxLoop;

	int nDeleteIndex = nInsertIndex / 2;

	for (int i = 0; i < nMaxLoop; i++)
	{		
		CRegKey ProductKey;		

		UUID guid;
		::UuidCreate(&guid);

		TCHAR* pszGuid = NULL;
#ifndef _UNICODE
		::UuidToString(&guid, (unsigned char**) &pszGuid);
#else
		::UuidToString(&guid, (unsigned short**) &pszGuid);
#endif

		CString sPath = _T("CLSID\\");
		sPath += pszGuid;

#ifndef _UNICODE
		::RpcStringFree((unsigned char**) &pszGuid);
#else
		::RpcStringFree((unsigned short**) &pszGuid);
#endif

		if (ProductKey.Open(HKEY_CLASSES_ROOT, sPath, KEY_READ) == ERROR_SUCCESS)
		{
			// don't do anything here!!!!
		}

		if (i == nDeleteIndex)
		{
			// delete the existing keys
			DeleteKnownRegistryKeys();
		}

		if (i == nInsertIndex)
		{					
			int nRegKeyIndex = i % PRODUCTLICENCE_MAXKEY;

			CString sPath = _T("CLSID\\");
			sPath += g_szRegistryKeys[nRegKeyIndex];

			if (ProductKey.Create(HKEY_CLASSES_ROOT, sPath) == ERROR_SUCCESS)
			{			
				// strip out the '-' from the string
				sProductKey.Replace(_T("-"), _T(""));

				// reverse the string to prevent 'find' in regedit being used
				sProductKey.MakeReverse();

				ProductKey.SetStringValue(_T(""), sProductKey);
			}
		}
	}

	return S_OK;
}