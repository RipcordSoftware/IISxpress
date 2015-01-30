#pragma once

class ATL_NO_VTABLE CIISxpressConsoleNotify : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIISxpressConsoleNotify, &CLSID_IISxpressConsoleNotify>,
	public IIISxpressHTTPNotify
{

public:

	CIISxpressConsoleNotify(void);
	~CIISxpressConsoleNotify(void);

DECLARE_NOT_AGGREGATABLE(CIISxpressConsoleNotify)

BEGIN_COM_MAP(CIISxpressConsoleNotify)
	COM_INTERFACE_ENTRY(IIISxpressHTTPNotify)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	STDMETHOD(OnResponseStart)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwFilterContext);
	STDMETHOD(OnResponseComplete)(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize);
	STDMETHOD(OnResponseEnd)(DWORD dwFilterContext);
	STDMETHOD(OnResponseAbort)(DWORD dwFilterContext);
	STDMETHOD(OnCacheHit)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwCompressedSize);

private:

	void OutputResponseInfo(LPCSTR pszURI, DWORD dwRawSize, DWORD dwCompressedSize);

	CComAutoCriticalSection				m_csActiveResponses;
	std::map<DWORD, CIISResponseData*>	m_ActiveResponses;
};
