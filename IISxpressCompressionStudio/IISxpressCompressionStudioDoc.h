// IISxpressCompressionStudioDoc.h : interface of the CIISxpressCompressionStudioDoc class
//


#pragma once


class CIISxpressCompressionStudioDoc : public CDocument
{
protected: // create from serialization only
	CIISxpressCompressionStudioDoc();
	DECLARE_DYNCREATE(CIISxpressCompressionStudioDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CIISxpressCompressionStudioDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	HRESULT GetIISxpressInterfaces(void);

	HRESULT GetHTTPRequest(IIISxpressHTTPRequest** ppHTTPRequest);
	HRESULT GetCompressionManager(ICompressionRuleManager** ppCompressionManager);
	HRESULT GetIISxpressServerRegSettings(IComIISxpressRegSettings** ppRegSettings);
	HRESULT GetProductLicenseManager(IProductLicenseManager** ppProductLicenseManager);

	void ResetResponseCache();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:	

	bool IsHTTPRequestValid(void);
	bool IsCompressionManagerValid(void);
	bool IsServerRegSettingsValid(void);
	bool IsProductLicenseManagerValid(void);

	CComAutoCriticalSection				m_csIISxpressInterfaces;
	CComPtr<IIISxpressHTTPRequest>		m_pHTTPRequest;
	CComPtr<ICompressionRuleManager>	m_pCompressionManager;
	CComPtr<IComIISxpressRegSettings>	m_pServerRegSettings;
	CComPtr<IProductLicenseManager>		m_pProductLicenseManager;

	DWORD								m_dwIsAliveCookie;

};


