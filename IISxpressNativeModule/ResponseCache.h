#pragma once

#include <atlstr.h>
#include <atlcache.h>

struct ResponseCacheItem
{
	ResponseCacheItem() : dwContentLength(0), pbyData(NULL) {}	

	~ResponseCacheItem()
	{
		delete[] pbyData;
		pbyData = NULL;
	}

	CAtlStringA		sContentEncoding;
	CAtlStringA		sContentLength;
	DWORD			dwContentLength;
	BYTE*			pbyData;		
};

class ResponseCache
{
public:
	static bool CreateResponseCacheKey(LPCSTR pszId, 							
								LPCSTR pszURI, 
								LPCSTR pszContentType, 							
								DWORD dwContentLength, 
								LPCSTR pszLastModified,
								LPCSTR pszAcceptEncoding,
								CAtlStringA& sKey);

	static bool CreateResponseCacheKey(LPCSTR pszId, 
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
								CAtlStringA& sKey);

	static HRESULT GetResponseCacheItemDeallocator(IMemoryCacheClient** pClient);
};

class CResponseCacheItemFree :
    public IMemoryCacheClient,
    public CComObjectRootEx<CComMultiThreadModel>
{
	BEGIN_COM_MAP(CResponseCacheItemFree)
        COM_INTERFACE_ENTRY(IMemoryCacheClient)
    END_COM_MAP()

    STDMETHOD(Free)(const void *pvData)
    {
        ResponseCacheItem** p = (ResponseCacheItem**) pvData;
        delete *p;

		::InterlockedIncrement((LONG*) &m_dwCacheCookie);

        return S_OK;
    }

	static volatile DWORD m_dwCacheCookie;
};