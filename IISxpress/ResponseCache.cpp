#include "stdafx.h"

#include <atlcom.h>

#include "ResponseCache.h"

bool MakeDWORDString(DWORD dwValue, char* pszBuffer, int nBufferSize)
{
	if (pszBuffer == NULL || nBufferSize < 8)
		return false;

	for (int i = 28; i >= 0; i -= 4)
	{
		BYTE byChar = (BYTE) (dwValue >> i);
		byChar &= 0x0f;

		if (byChar < 10)
		{
			*pszBuffer = byChar + 0x30;			
		}
		else
		{
			*pszBuffer = byChar + 0x60 - 9;
		}

		pszBuffer++;
	}	

	return true;
}

bool ResponseCache::CreateResponseCacheKey(LPCSTR pszId, LPCSTR pszURI, LPCSTR pszContentType, DWORD dwContentLength, 
							LPCSTR pszLastModified, LPCSTR pszAcceptEncoding, CAtlStringA& sKey)
{
	if (pszId == NULL || pszURI == NULL || pszContentType == NULL || pszLastModified == NULL || pszAcceptEncoding == NULL)
		return false;

	return CreateResponseCacheKey(pszId, (DWORD) strlen(pszId), 
		pszURI, (DWORD) strlen(pszURI), 
		pszContentType, (DWORD) strlen(pszContentType), 
		dwContentLength, 
		pszLastModified, (DWORD) strlen(pszLastModified), 
		pszAcceptEncoding, (DWORD) strlen(pszAcceptEncoding),
		sKey);
}

bool ResponseCache::CreateResponseCacheKey(LPCSTR pszId, 
							DWORD dwIdLength,
							LPCSTR pszURI, 
							DWORD dwURILength,
							LPCSTR pszContentType, 
							DWORD dwContentTypeLength,
							DWORD dwContentLength, 
							LPCSTR pszLastModified,
							DWORD dwLastModifiedLength,
							LPCSTR pszAcceptEncoding,
							DWORD dwAcceptEncodingLength,
							CAtlStringA& sKey)
{
	if (pszId == NULL || pszURI == NULL || pszContentType == NULL || pszLastModified == NULL || pszAcceptEncoding == NULL)
		return false;

	// calculate the maximum length of the key string
	DWORD dwKeyLength = dwIdLength + dwURILength + dwContentTypeLength + 8 + dwLastModifiedLength + dwAcceptEncodingLength + 1;

	// allocate the key string from the stack
	char* pszKey = (char*) alloca(dwKeyLength);
	if (pszKey == NULL)
		return false;

	char* pszKeyOffset = pszKey;

	memcpy(pszKeyOffset, pszId, dwIdLength);	
	pszKeyOffset += dwIdLength;

	memcpy(pszKeyOffset, pszURI, dwURILength);
	pszKeyOffset += dwURILength;

	memcpy(pszKeyOffset, pszContentType, dwContentTypeLength);
	pszKeyOffset += dwContentTypeLength;

	MakeDWORDString(dwContentLength, pszKeyOffset, dwKeyLength - (pszKeyOffset - pszKey));
	pszKeyOffset += 8;	

	memcpy(pszKeyOffset, pszLastModified, dwLastModifiedLength);
	pszKeyOffset += dwLastModifiedLength;

	memcpy(pszKeyOffset, pszAcceptEncoding, dwAcceptEncodingLength);
	pszKeyOffset += dwAcceptEncodingLength;

	// terminate the string
	*pszKeyOffset = '\0';

	// return the key to the caller
	sKey = pszKey;

	return true;
}

HRESULT ResponseCache::GetResponseCacheItemDeallocator(IMemoryCacheClient** ppClient)
{
	if (ppClient == NULL)
		return E_POINTER;

	CComObject<CResponseCacheItemFree>* pClient;
	HRESULT hr = CComObject<CResponseCacheItemFree>::CreateInstance(&pClient);
	if (hr == S_OK)
	{
		pClient->AddRef();
		*ppClient = pClient;
	}

	return hr;
}

DWORD volatile CResponseCacheItemFree::m_dwCacheCookie = 0;