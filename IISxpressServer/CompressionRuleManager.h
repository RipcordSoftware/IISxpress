// CompressionRuleManager.h : Declaration of the CCompressionRuleManager

#pragma once
#include "resource.h"       // main symbols

#include "IISxpressServer.h"
#include "IStaticFileRulesImpl.h"
#include "INeverCompressRulesImpl.h"
#include "ICompressionHistoryImpl.h"
#include "IIISxpressSettingsImpl.h"

// CCompressionRuleManager

class ATL_NO_VTABLE CCompressionRuleManager : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CCompressionRuleManager, &CLSID_CompressionRuleManager>,
	public ICompressionRuleManager,
	public IStaticFileRulesImpl,
	public INeverCompressRulesImpl,
	public ICompressionHistoryImpl,
	public IIISxpressSettingsImpl,
	public IConnectionPointContainerImpl<CCompressionRuleManager>,
	public IConnectionPointImpl<CCompressionRuleManager, &IID_INeverCompressRulesNotify>,
	public INotifyCookie
{

public:

	CCompressionRuleManager() : m_dwNotifyCookie(1)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMPRESSIONRULEMANAGER)

DECLARE_NOT_AGGREGATABLE(CCompressionRuleManager)

BEGIN_COM_MAP(CCompressionRuleManager)
	COM_INTERFACE_ENTRY(ICompressionRuleManager)
	COM_INTERFACE_ENTRY2(IStaticFileRules, IStaticFileRulesImpl)
	COM_INTERFACE_ENTRY2(IStaticFileRules2, IStaticFileRulesImpl)
	COM_INTERFACE_ENTRY2(ICustomCompressionMode, IStaticFileRulesImpl)
	COM_INTERFACE_ENTRY2(INeverCompressRules, INeverCompressRulesImpl)
	COM_INTERFACE_ENTRY2(ICompressionHistory, ICompressionHistoryImpl)
	COM_INTERFACE_ENTRY2(ICompressionHistory2, ICompressionHistoryImpl)
	COM_INTERFACE_ENTRY2(ICompressionStats, ICompressionHistoryImpl)
	COM_INTERFACE_ENTRY2(ICompressionStats2, ICompressionHistoryImpl)
	COM_INTERFACE_ENTRY(IIISxpressSettings)	
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(INotifyCookie)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CCompressionRuleManager)
	CONNECTION_POINT_ENTRY(IID_INeverCompressRulesNotify)
END_CONNECTION_POINT_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();	
	void FinalRelease();

public:
	
	// ICompressionRuleManager
	STDMETHOD(Init)(IStorage* pStorage, IComIISxpressRegSettings* pRegSettings);
	STDMETHOD(CanCompressFile)(LPCSTR pszPath, LPCSTR pszFilename, DWORD dwFileSize);

	// INotifyCookie
	STDMETHOD(GetCookie)(DWORD* pdwCookie);

private:

	STDMETHOD(UpgradeNeverRules)(INeverCompressRules* pNeverRules);	

	STDMETHOD(Fire_OnRuleChanged)(NeverRuleChangeHint hint, DWORD dwSource);

	volatile DWORD m_dwNotifyCookie;

};

OBJECT_ENTRY_AUTO(__uuidof(CompressionRuleManager), CCompressionRuleManager)
