// IISxpressCompressionStudioDoc.cpp : implementation of the CIISxpressCompressionStudioDoc class
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"

#include "IISxpressCompressionStudioDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIISxpressCompressionStudioDoc

IMPLEMENT_DYNCREATE(CIISxpressCompressionStudioDoc, CDocument)

BEGIN_MESSAGE_MAP(CIISxpressCompressionStudioDoc, CDocument)
END_MESSAGE_MAP()


// CIISxpressCompressionStudioDoc construction/destruction

CIISxpressCompressionStudioDoc::CIISxpressCompressionStudioDoc() : 	m_dwIsAliveCookie(0)
{
	// TODO: add one-time construction code here

}

CIISxpressCompressionStudioDoc::~CIISxpressCompressionStudioDoc()
{
}

BOOL CIISxpressCompressionStudioDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CIISxpressCompressionStudioDoc serialization

void CIISxpressCompressionStudioDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CIISxpressCompressionStudioDoc diagnostics

#ifdef _DEBUG
void CIISxpressCompressionStudioDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIISxpressCompressionStudioDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIISxpressCompressionStudioDoc commands

HRESULT CIISxpressCompressionStudioDoc::GetIISxpressInterfaces(void)
{
	m_csIISxpressInterfaces.Lock();

	// release the references we are holding
	m_pHTTPRequest.Release();
	m_pCompressionManager.Release();
	m_pServerRegSettings.Release();
	m_pProductLicenseManager.Release();

	m_csIISxpressInterfaces.Unlock();

	// get the active HTTPRequestUnk interface
	CComPtr<IUnknown> pHTTPRequestUnk;
	HRESULT hr = ::GetActiveObject(CLSID_IISxpressHTTPRequest, NULL, (IUnknown**) &pHTTPRequestUnk);
	if (hr != S_OK)
	{
		return hr;	
	}

	// get the active HTTPRequest interface
	CComQIPtr<IIISxpressHTTPRequest> pHTTPRequest = pHTTPRequestUnk;
	if (pHTTPRequest == NULL)
	{
		return E_FAIL;
	}

	// ask for the cookie
	DWORD dwIsAliveCookie = 0;
	hr = pHTTPRequest->IsAlive(&dwIsAliveCookie);
	if (hr != S_OK)
	{
		return hr;
	}

	// we always store the new cookie, regardless of it's previous value at this point
	DWORD dwOldIsAliveCookie = ::InterlockedExchange((LONG*) &m_dwIsAliveCookie, dwIsAliveCookie);

	if (dwOldIsAliveCookie == 0)
	{
		// the cookie hadn't been initialised before, we don't need to do anything
	}
	else if (dwOldIsAliveCookie != dwIsAliveCookie)
	{
		// the cookie has changed since last time, return fail to simulate a lost connection
		return E_FAIL;
	}

	CComPtr<IUnknown> pCompressionManagerUnk;
	hr = ::GetActiveObject(CLSID_CompressionRuleManager, NULL, (IUnknown**) &pCompressionManagerUnk);	
	if (hr != S_OK)
	{		
		return hr;
	}

	CComPtr<IUnknown> pRegSettingsUnk;
	hr = ::GetActiveObject(CLSID_ComIISxpressRegSettings, NULL, (IUnknown**) &pRegSettingsUnk);	
	if (hr != S_OK)
	{
		return hr;
	}

	CComPtr<IUnknown> pProductLicenseManagerUnk;
	hr = ::GetActiveObject(CLSID_ProductLicenseManager, NULL, (IUnknown**) &pProductLicenseManagerUnk);	
	if (hr != S_OK)
	{
		return hr;
	}

	m_csIISxpressInterfaces.Lock();

	m_pHTTPRequest.Release();
	m_pCompressionManager.Release();
	m_pServerRegSettings.Release();
	m_pProductLicenseManager.Release();

	hr = pHTTPRequestUnk.QueryInterface(&m_pHTTPRequest);
	if (hr != S_OK)
	{
		m_csIISxpressInterfaces.Unlock();
		return hr;
	}

	hr = pCompressionManagerUnk.QueryInterface(&m_pCompressionManager);
	if (hr != S_OK)
	{
		m_csIISxpressInterfaces.Unlock();
		return hr;
	}	

	hr = pRegSettingsUnk.QueryInterface(&m_pServerRegSettings);	
	if (hr != S_OK)
	{
		m_csIISxpressInterfaces.Unlock();
		return hr;
	}	

	hr = pProductLicenseManagerUnk.QueryInterface(&m_pProductLicenseManager);

	m_csIISxpressInterfaces.Unlock();

	return hr;
}

HRESULT CIISxpressCompressionStudioDoc::GetHTTPRequest(IIISxpressHTTPRequest** ppHTTPRequest)
{
	if (ppHTTPRequest == NULL)
		return E_POINTER;

	HRESULT hr = E_FAIL;	

	if (IsHTTPRequestValid() == false && GetIISxpressInterfaces() != S_OK)
	{
		// we can't get the compression manager, so we just return E_FAIL
	}
	else
	{
		m_csIISxpressInterfaces.Lock();		
		hr = m_pHTTPRequest.QueryInterface(ppHTTPRequest);
		m_csIISxpressInterfaces.Unlock();
	}

	return hr;
}

HRESULT CIISxpressCompressionStudioDoc::GetCompressionManager(ICompressionRuleManager** ppCompressionManager)
{
	if (ppCompressionManager == NULL)
		return E_POINTER;

	HRESULT hr = E_FAIL;	

	if (IsCompressionManagerValid() == false && GetIISxpressInterfaces() != S_OK)
	{
		// we can't get the compression manager, so we just return E_FAIL
	}
	else
	{
		m_csIISxpressInterfaces.Lock();		
		hr = m_pCompressionManager.QueryInterface(ppCompressionManager);
		m_csIISxpressInterfaces.Unlock();
	}

	return hr;
}

HRESULT CIISxpressCompressionStudioDoc::GetIISxpressServerRegSettings(IComIISxpressRegSettings** ppRegSettings)
{
	if (ppRegSettings == NULL)
		return E_POINTER;

	HRESULT hr = E_FAIL;	

	if (IsServerRegSettingsValid() == false && GetIISxpressInterfaces() != S_OK)
	{
		// we can't get the server reg settings, so we just return E_FAIL
	}
	else
	{
		m_csIISxpressInterfaces.Lock();		
		hr = m_pServerRegSettings.QueryInterface(ppRegSettings);
		m_csIISxpressInterfaces.Unlock();
	}

	return hr;
}

HRESULT CIISxpressCompressionStudioDoc::GetProductLicenseManager(IProductLicenseManager** ppProductLicenseManager)
{
	if (ppProductLicenseManager == NULL)
		return E_POINTER;

	HRESULT hr = E_FAIL;	

	if (IsProductLicenseManagerValid() == false && GetIISxpressInterfaces() != S_OK)
	{
		// we can't get the product license manager, so we just return E_FAIL
	}
	else
	{
		m_csIISxpressInterfaces.Lock();		
		hr = m_pProductLicenseManager.QueryInterface(ppProductLicenseManager);
		m_csIISxpressInterfaces.Unlock();
	}

	return hr;
}

bool CIISxpressCompressionStudioDoc::IsCompressionManagerValid(void)
{
	bool bStatus = false;

	m_csIISxpressInterfaces.Lock();

	if (m_pCompressionManager != NULL)
	{
		CComPtr<ICompressionRuleManager> pTemp;
		if (m_pCompressionManager.QueryInterface(&pTemp) == S_OK)
			bStatus = true;
	}

	m_csIISxpressInterfaces.Unlock();

	return bStatus;
}

bool CIISxpressCompressionStudioDoc::IsHTTPRequestValid(void)
{
	bool bStatus = false;

	m_csIISxpressInterfaces.Lock();

	if (m_pHTTPRequest != NULL)
	{
		DWORD dwData = 0;
		if (m_pHTTPRequest->IsAlive(&dwData) == S_OK)
			bStatus = true;
	}

	m_csIISxpressInterfaces.Unlock();

	return bStatus;
}

bool CIISxpressCompressionStudioDoc::IsServerRegSettingsValid(void)
{
	bool bStatus = false;

	m_csIISxpressInterfaces.Lock();

	if (m_pServerRegSettings != NULL)
	{
		CComPtr<IComIISxpressRegSettings> pTemp;
		if (m_pServerRegSettings.QueryInterface(&pTemp) == S_OK)
			bStatus = true;
	}

	m_csIISxpressInterfaces.Unlock();

	return bStatus;
}

bool CIISxpressCompressionStudioDoc::IsProductLicenseManagerValid(void)
{
	bool bStatus = false;

	m_csIISxpressInterfaces.Lock();

	if (m_pProductLicenseManager != NULL)
	{
		CComPtr<IProductLicenseManager> pTemp;
		if (m_pProductLicenseManager.QueryInterface(&pTemp) == S_OK)
			bStatus = true;
	}

	m_csIISxpressInterfaces.Unlock();

	return bStatus;
}

void CIISxpressCompressionStudioDoc::ResetResponseCache()
{
	CIISxpressRegSettingsEx reg;
	reg.Init(IISXPRESSFILTER_REGKEY, false, false);
	reg.SetCacheStateCookie(::GetTickCount());
}