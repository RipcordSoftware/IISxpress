// ComIISxpressRegSettings.h : Declaration of the CComIISxpressRegSettings

#pragma once
#include "resource.h"       // main symbols

#include "IISxpressServer.h"
#include "IISxpressRegSettingsEx.h"

// CComIISxpressRegSettings

class ATL_NO_VTABLE CComIISxpressRegSettings : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CComIISxpressRegSettings, &CLSID_ComIISxpressRegSettings>,
	public IComIISxpressRegSettings
{

public:

	CComIISxpressRegSettings()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMIISXPRESSREGSETTINGS)

DECLARE_NOT_AGGREGATABLE(CComIISxpressRegSettings)

BEGIN_COM_MAP(CComIISxpressRegSettings)
	COM_INTERFACE_ENTRY(IComIISxpressRegSettings)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	bool Init(LPCTSTR pszRegPath);

	STDMETHOD(get_Enabled)(BOOL* pbEnabled);
	STDMETHOD(put_Enabled)(BOOL bEnabled);
	STDMETHOD(get_DebugEnabled)(BOOL* pbEnabled);
	STDMETHOD(put_DebugEnabled)(BOOL bEnabled);
	STDMETHOD(get_LoggingLevel)(DWORD* pdwLevel);
	STDMETHOD(put_LoggingLevel)(DWORD dwLevel);	
	STDMETHOD(get_CheckPhysicalMemory)(BOOL* pbCheck);
	STDMETHOD(get_CheckCPULoad)(BOOL* pbCheck);
	STDMETHOD(get_CheckNeverRules)(BOOL* pbCheck);
	STDMETHOD(get_CheckUserRules)(BOOL* pbCheck);
	STDMETHOD(get_LicenseMode)(LicenseMode* pMode);
	STDMETHOD(put_LicenseMode)(LicenseMode Mode);
	STDMETHOD(get_BZIP2Mode)(BZIP2Mode* pMode);
	STDMETHOD(put_BZIP2Mode)(BZIP2Mode mode);

	STDMETHOD(get_AHAEnabled)(DWORD* pdwEnabled);
	STDMETHOD(put_AHAEnabled)(DWORD dwEnabled);
	STDMETHOD(get_AHADetectOverheat)(DWORD* pdwDetectOverheat);
	STDMETHOD(put_AHADetectOverheat)(DWORD dwDetectOverheat);

private:

	CIISxpressRegSettingsEx		m_RegSettings;

};

OBJECT_ENTRY_AUTO(__uuidof(ComIISxpressRegSettings), CComIISxpressRegSettings)
