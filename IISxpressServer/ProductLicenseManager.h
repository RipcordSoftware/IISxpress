// ProductLicenseManager.h : Declaration of the CProductLicenseManager

#pragma once
#include "resource.h"       // main symbols

#include "IISxpressServer.h"

#include "ProductLicense.h"

#include <vector>

// CProductLicenseManager

class ATL_NO_VTABLE CProductLicenseManager : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CProductLicenseManager, &CLSID_ProductLicenseManager>,
	public IProductLicenseManager
{
public:

	CProductLicenseManager()
	{
		m_pProductKey = NULL;
	}

	~CProductLicenseManager()
	{
		delete m_pProductKey;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PRODUCTLICENSEMANAGER)

DECLARE_NOT_AGGREGATABLE(CProductLicenseManager)

BEGIN_COM_MAP(CProductLicenseManager)
	COM_INTERFACE_ENTRY(IProductLicenseManager)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	
	void FinalRelease();	

public:

	STDMETHOD(IsValidKey)(void);	
	STDMETHOD(IsRegisteredKey)(void);
	STDMETHOD(SetKey)(LPCSTR pszKey, BOOL bAllowTrial);
	STDMETHOD(GetExpiry)(SYSTEMTIME* pstExpiry);

private:

	HRESULT LoadProductKeyFromRegistry(CStringA& sProductKey);
	HRESULT SaveProductKeyToRegistry(void);
	HRESULT DeleteKnownRegistryKeys(void);

	CProductLicense		m_License;
	CStringA			m_sEncryptionKey;

	ProductKeyData*		m_pProductKey;

};

OBJECT_ENTRY_AUTO(__uuidof(ProductLicenseManager), CProductLicenseManager)
