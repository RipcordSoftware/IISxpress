// HTTPNotify.h : Declaration of the CHTTPNotify

#pragma once
#include "resource.h"       // main symbols

#ifdef _MANAGED
#include <vcclr.h>
#endif

// CHTTPNotify

template <typename T> 
class ATL_NO_VTABLE CHTTPNotify : 
	public CComObjectRootEx<CComMultiThreadModel>,	
	public IIISxpressHTTPNotify
{

#ifdef _MANAGED
	typedef gcroot<T^> NotifyPtr;
#else
	typedef T* NotifyPtr;
#endif

public:

	CHTTPNotify()
	{
#ifdef _MANAGED
		m_pNotify = nullptr;
#else
		m_pNotify = NULL;
#endif		
	}

DECLARE_NOT_AGGREGATABLE(CHTTPNotify)

BEGIN_COM_MAP(CHTTPNotify)
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

	STDMETHOD(OnResponseStart)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwFilterContext)
	{
		if (IsNull(m_pNotify) == false)
			return m_pNotify->OnResponseStart(pIISInfo, pRequestInfo, pResponseInfo, dwFilterContext);
		else
			return E_FAIL;
	}

	STDMETHOD(OnResponseComplete)(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
	{
		if (IsNull(m_pNotify) == false)
			return m_pNotify->OnResponseComplete(dwFilterContext, bCompressed, dwRawSize, dwCompressedSize);	
		else
			return E_FAIL;
	}

	STDMETHOD(OnResponseEnd)(DWORD dwFilterContext)
	{
		if (IsNull(m_pNotify) == false)
			return m_pNotify->OnResponseEnd(dwFilterContext);
		else
			return E_FAIL;
	}

	STDMETHOD(OnResponseAbort)(DWORD dwFilterContext)
	{
		if (IsNull(m_pNotify) == false)
			return m_pNotify->OnResponseAbort(dwFilterContext);
		else
			return E_FAIL;
	}

	STDMETHOD(OnCacheHit)(IISInfo* pIISInfo, RequestInfo* pRequestInfo, ResponseInfo* pResponseInfo, DWORD dwCompressedSize)
	{
		if (IsNull(m_pNotify) == false)
			return m_pNotify->OnCacheHit(pIISInfo, pRequestInfo, pResponseInfo, dwCompressedSize);
		else
			return E_FAIL;
	}

	void Init(NotifyPtr pNotify) { m_pNotify = pNotify; }

private:

#ifdef _MANAGED
	bool IsNull(System::Object^ o) { return o == nullptr ? true : false; }
#else
	bool IsNull(void* p) { return p == NULL ? true : false; }
#endif

	NotifyPtr m_pNotify;

};
