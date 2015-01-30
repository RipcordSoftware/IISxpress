#pragma once

#include <winsvc.h>

#include "IISxpressTrayIcon.h"

#include "MenuXP\MenuXP.h"

#define WM_TRAYMSG	(WM_APP + 1)

#define SHEET_TIMER_ID				1234
#define SHEET_TIMER_INTERVAL		5000

// CIISxpressTrayPropertySheet

class CIISxpressTrayPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CIISxpressTrayPropertySheet)

public:

	CIISxpressTrayPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CIISxpressTrayPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CIISxpressTrayPropertySheet();

	HRESULT GetIISxpressInterfaces(void);

	HRESULT GetHTTPRequest(IIISxpressHTTPRequest** ppHTTPRequest);
	HRESULT GetCompressionManager(ICompressionRuleManager** ppCompressionManager);
	HRESULT GetIISxpressServerRegSettings(IComIISxpressRegSettings** ppRegSettings);
	HRESULT GetProductLicenseManager(IProductLicenseManager** ppProductLicenseManager);

	SC_HANDLE GetServiceManager(void);
	void CloseServiceManager(void);

	UINT GetTrayNotifyId(void) { return m_TrayIcon.GetNotifyId(); }	

	void SetCaptionEdition(void);

	void VersionCheck(void);

	HRESULT GetIIS7ConfigDocument(IXMLDOMDocument** ppDoc);	

	static void ResetResponseCache();

	DWORD GetUpdateCookie();

protected:

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog(void);
	void OnTimer(UINT_PTR nIDEvent);
	void OnDestroy(void);
	void OnSysCommand(UINT nID, LPARAM lParam);
	void OnClose(void);

	LRESULT OnTrayMsg(WPARAM wParam, LPARAM lParam);
	void OnTrayStatus(void);
	void OnTrayExit(void);
	void OnTrayHelp(void);
	void OnTrayCheckForUpdates(void);

	DECLARE_MENUXP()

private:

	bool ConnectToServer(void);

	void StartTimer(void);
	void StopTimer(void);

	void UpdateTrayTip(bool bConnected);

	bool MakeVersionDWORDs(LPCWSTR pszVersionString, DWORD* pdwMajor, DWORD* pdwMinor);

	static void MakeDisplayCookie(CString& sCookie);

	void GetUpdateURL(CString& sURL);

	bool	m_bTimerRunning;
	DWORD	m_dwLastTimerEvent;

	bool IsHTTPRequestValid(void);
	bool IsCompressionManagerValid(void);
	bool IsServerRegSettingsValid(void);
	bool IsProductLicenseManagerValid(void);

	CComAutoCriticalSection				m_csIISxpressInterfaces;
	CComPtr<IIISxpressHTTPRequest>		m_pHTTPRequest;
	CComPtr<ICompressionRuleManager>	m_pCompressionManager;
	CComPtr<IComIISxpressRegSettings>	m_pServerRegSettings;
	CComPtr<IProductLicenseManager>		m_pProductLicenseManager;

	SC_HANDLE							m_hSMStatus;

	CButton*							m_pcClose;
	CButton*							m_pcHelp;		
	CStatic*							m_pcBetaVersionString;

	DWORD								m_dwIsAliveCookie;

	CIISxpressTrayIcon					m_TrayIcon;

	CImage								m_bmpTrayMenuStatus;
	CImage								m_bmpTrayMenuHelp;

	// keep track of whether the caption has been set the first time or not
	bool								m_bCaptionSet;

	// IIS7 config members	
	CAtlString							m_sConfigFilePath;
	volatile FILETIME					m_ftConfigFile;
	CComPtr<IXMLDOMDocument>			m_pConfigDoc;
};