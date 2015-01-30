// IISxpressTrayPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "IISxpressTrayPropertySheet.h"

#include "IISxpressRegSettingsEx.h"

#include "resource.h"

#include <shellapi.h>

#include <atlpath.h>

// CIISxpressTrayPropertySheet

IMPLEMENT_DYNAMIC(CIISxpressTrayPropertySheet, CPropertySheet)

IMPLEMENT_MENUXP(CIISxpressTrayPropertySheet, CPropertySheet);

CIISxpressTrayPropertySheet::CIISxpressTrayPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_bTimerRunning = false;
	m_dwLastTimerEvent = 0;

	m_hSMStatus = NULL;

	m_pcClose = NULL;
	m_pcHelp = NULL;
	m_pcBetaVersionString = NULL;

	m_dwIsAliveCookie = 0;

	// we haven't set the caption yet
	m_bCaptionSet = false;

	// the IIS7 config file members
	CIIS7XMLConfigHelper::GetApplicationHostConfigPath(m_sConfigFilePath);
	m_ftConfigFile.dwHighDateTime = 0;
	m_ftConfigFile.dwLowDateTime = 0;
}

CIISxpressTrayPropertySheet::CIISxpressTrayPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_bTimerRunning = false;
	m_dwLastTimerEvent = 0;

	m_hSMStatus = NULL;

	m_pcClose = NULL;
	m_pcHelp = NULL;
	m_pcBetaVersionString = NULL;

	m_dwIsAliveCookie = 0;

	// we haven't set the caption yet
	m_bCaptionSet = false;
}

CIISxpressTrayPropertySheet::~CIISxpressTrayPropertySheet()
{
	if (m_pcClose != NULL)
	{
		delete m_pcClose;
		m_pcClose = NULL;
	}

	if (m_pcHelp != NULL)
	{
		delete m_pcHelp;
		m_pcHelp = NULL;
	}

	if (m_pcBetaVersionString != NULL)
	{
		delete m_pcBetaVersionString;
		m_pcBetaVersionString = NULL;
	}
}


BEGIN_MESSAGE_MAP(CIISxpressTrayPropertySheet, CPropertySheet)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(IDC_BUTTON_CLOSE, OnClose)
	ON_MESSAGE(WM_TRAYMSG, OnTrayMsg)
	ON_COMMAND(ID_TRAYMENU_STATUS, OnTrayStatus)
	ON_COMMAND(ID_TRAYMENU_EXIT, OnTrayExit)
	ON_COMMAND(ID_TRAYMENU_HELP, OnTrayHelp)
	ON_COMMAND(ID_TRAYMENU_ADV_CHECKFORUPDATES, OnTrayCheckForUpdates)
	ON_WM_SYSCOMMAND()
	ON_MENUXP_MESSAGES()
END_MESSAGE_MAP()


// CIISxpressTrayPropertySheet message handlers

BOOL CIISxpressTrayPropertySheet::OnInitDialog(void)
{
	BOOL bStatus = CPropertySheet::OnInitDialog();	

	// load the tray's help bitmap
	if (LoadPNG(_T("status16.png"), m_bmpTrayMenuStatus) == true &&
		LoadPNG(_T("help16.png"), m_bmpTrayMenuHelp) == true)
	{		
		CMenuXP::SetXPLookNFeel(this);	
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_STATUS, &m_bmpTrayMenuStatus);
		CMenuXP::SetMenuItemImage(ID_TRAYMENU_HELP, &m_bmpTrayMenuHelp);
	}

	// recover the prevous screen coordinates and display cookie from the registry
	CIISxpressTrayApp* pApp = (CIISxpressTrayApp*) ::AfxGetApp();
	const int nInvalidPos = 1 << 31;
	int nXPos = pApp->GetProfileInt(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_WINDOWPOSX, nInvalidPos);
	int nYPos = pApp->GetProfileInt(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_WINDOWPOSY, nInvalidPos);
	CString sOldDisplayCookie = pApp->GetProfileString(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_DISPLAYCOOKIE, _T(""));

	// make the current display cookie
	CString sDisplayCookie;
	MakeDisplayCookie(sDisplayCookie);

	// if we have valid coords and a matching display cookie then attempt to position the window
	if (nXPos != nInvalidPos && nYPos != nInvalidPos && sDisplayCookie == sOldDisplayCookie)
	{
		// get the min X and Y screen coords
		int nXMin = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
		int nYMin = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
		
		// get the virtual screen width and height
		int nWidth = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int nHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		// get the virtual screen rectangle
		CRect rcScreen(nXMin, nYMin, nXMin + nWidth, nYMin + nHeight);

		// get the sheet rectangle
		CRect rcSheet;
		GetWindowRect(rcSheet);
		rcSheet.MoveToXY(nXPos, nYPos);

		// get the intersection between the screen and the sheet rectangles
		CRect rcIntersect;
		rcIntersect.IntersectRect(rcScreen, rcSheet);

		// if we have a valid intersection then move the window to the stored position
		if (rcIntersect.IsRectNull() == FALSE)
		{
			SetWindowPos(NULL, nXPos, nYPos, 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);
		}
	}

	// add 'always on top' to the system menu
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		ASSERT((IDM_ALWAYSONTOP & 0x0f) == 0);
		ASSERT(IDM_ALWAYSONTOP < 0xF000);

		CString sAlwaysOnTop;
		sAlwaysOnTop.LoadString(IDM_ALWAYSONTOP);
		if (sAlwaysOnTop.IsEmpty() == FALSE)
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ALWAYSONTOP, sAlwaysOnTop);
		}
	}

	// load the app icon
	HICON hIcon = ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICONCOMPASS));
	if (hIcon != NULL)
	{
		SetIcon(hIcon, TRUE);
		SetIcon(hIcon, FALSE);
	}
	
	m_TrayIcon.CreateTrayIcon(GetSafeHwnd(), ::GetTickCount(), IDI_ICONCOMPASS, IDS_IISXPRESS_APPNAME);

	// switch on the context help in the caption bar
	ModifyStyleEx(0, WS_EX_CONTEXTHELP);	

	// get hold of the tab child window
	CWnd* pTabs = GetWindow(GW_CHILD);

	// get the size of the tab window (we need to know where the right-hand edge is for later)
	CRect rcTabs;
	pTabs->GetWindowRect(rcTabs);
	ScreenToClient(rcTabs);

	// load a string from the resources so we can calculate the button size
	CString sSpacer;
	sSpacer.LoadString(IDS_SHEETBUTTONSPACERSTRING);

	// get the sheet DC and ask it to calculate the space required for a string
	CDC* pDC = GetDC();
	CSize FontSize = pDC->GetTextExtent(sSpacer);

	// we need constants for the size of the button border and the 
	// surrounding space around the button
	const int nButtonBorder = 8;
	const int nButtonSurround = 10;

	// since we need to resize the window calculate the amount of extra space we need
	// for the buttons
	int nWindowExtra = FontSize.cy;
	nWindowExtra += nButtonBorder;		// for the button
	nWindowExtra += nButtonSurround;	// for the button surround

	// calculate the button width and height we are going to use
	int nButtonWidth = FontSize.cx;
	int nButtonHeight = FontSize.cy + nButtonBorder;

	// get the extents of the sheet itself
	CRect rcSheet;
	GetWindowRect(rcSheet);

	// resize the sheet to accomodate the buttons
	MoveWindow(rcSheet.left, rcSheet.top, rcSheet.Width(), rcSheet.Height() + nWindowExtra);

	// get the new rect
	GetWindowRect(rcSheet);

	// turn the sheet into client coords
	ScreenToClient(rcSheet);

	// calculate where we are going to place the help button
	CRect rcHelpButton;
	rcHelpButton.top = rcTabs.bottom + ((rcSheet.bottom - rcTabs.bottom - nButtonHeight) / 2);
	rcHelpButton.bottom = rcHelpButton.top + nButtonHeight;
	rcHelpButton.right = rcTabs.right;
	rcHelpButton.left = rcHelpButton.right - nButtonWidth;

	// calculate where we are going to place the close button
	CRect rcCloseButton;
	rcCloseButton.top = rcHelpButton.top;
	rcCloseButton.bottom = rcHelpButton.bottom;
	rcCloseButton.left = rcHelpButton.left - nButtonWidth - nButtonSurround;
	rcCloseButton.right = rcHelpButton.right - nButtonWidth - nButtonSurround;

	// calculate where we are going to place the beta version string
	CRect rcBetaVersionString;
	rcBetaVersionString.top = rcHelpButton.top;
	rcBetaVersionString.left = 10;
	rcBetaVersionString.bottom = rcBetaVersionString.top + 20;
	rcBetaVersionString.right = rcBetaVersionString.left + 160;

	// we need to use the dialog font in the button controls
	CFont* pFont = GetFont();

	// load the close button text from the resources
	CString sClose;
	sClose.LoadString(IDS_CLOSEBUTTONTEXT);

	// create the close button first since we want it before help in the tab order
	m_pcClose = new CButton();
	m_pcClose->Create(sClose, WS_CHILD | WS_VISIBLE | WS_TABSTOP, rcCloseButton, this, IDC_BUTTON_CLOSE);	
	m_pcClose->SetFont(pFont, TRUE);	

	// load the help button text from the resources
	CString sHelp;
	sHelp.LoadString(IDS_HELPBUTTONTEXT);

	// create the help button
	m_pcHelp = new CButton();
	m_pcHelp->Create(sHelp, WS_CHILD | WS_VISIBLE | WS_TABSTOP, rcHelpButton, this, IDHELP);	
	m_pcHelp->SetFont(pFont, TRUE);

	// load the beta version string
	CString sBetaVersionString;
	if (sBetaVersionString.LoadString(IDS_BETA_VERSIONSTRING) == TRUE && sBetaVersionString.GetLength() > 1)
	{
		// create the static beta version string
		m_pcBetaVersionString = new CStatic();
		m_pcBetaVersionString->Create(sBetaVersionString, WS_CHILD | WS_VISIBLE | SS_LEFT, rcBetaVersionString, this);
		m_pcBetaVersionString->SetFont(pFont, TRUE);
	}

	// kick the pages into life
	int nPages = GetPageCount();
	for (int i = nPages - 1; i >= 0; i--)
	{
		CPropertyPage* pPage = GetPage(i);
		SetActivePage(i);
	}	

	if (ConnectToServer() == true)
	{
		// if we connected to the server then set the caption (it is done in the timer loop anyway
		// but it looks cleaner if it is set straight away)
		SetCaptionEdition();
		m_bCaptionSet = true;
	}

	StartTimer();

	return bStatus;
}

HRESULT CIISxpressTrayPropertySheet::GetIISxpressInterfaces(void)
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

HRESULT CIISxpressTrayPropertySheet::GetHTTPRequest(IIISxpressHTTPRequest** ppHTTPRequest)
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

HRESULT CIISxpressTrayPropertySheet::GetCompressionManager(ICompressionRuleManager** ppCompressionManager)
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

HRESULT CIISxpressTrayPropertySheet::GetIISxpressServerRegSettings(IComIISxpressRegSettings** ppRegSettings)
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

HRESULT CIISxpressTrayPropertySheet::GetProductLicenseManager(IProductLicenseManager** ppProductLicenseManager)
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

bool CIISxpressTrayPropertySheet::IsCompressionManagerValid(void)
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

bool CIISxpressTrayPropertySheet::IsHTTPRequestValid(void)
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

bool CIISxpressTrayPropertySheet::IsServerRegSettingsValid(void)
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

bool CIISxpressTrayPropertySheet::IsProductLicenseManagerValid(void)
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

void CIISxpressTrayPropertySheet::StartTimer(void)
{
	if (m_bTimerRunning == false)
	{
		SetTimer(SHEET_TIMER_ID, SHEET_TIMER_INTERVAL, NULL);
		m_bTimerRunning = true;
	}
}

void CIISxpressTrayPropertySheet::StopTimer(void)
{
	if (m_bTimerRunning == true)
	{
		KillTimer(SHEET_TIMER_ID);
		m_bTimerRunning = false;
	}
}

void CIISxpressTrayPropertySheet::OnTimer(UINT_PTR nIDEvent)
{
	DWORD dwNow = ::GetTickCount();

	// only handle timer if more than the defined number of ms have elapsed
	if (nIDEvent == SHEET_TIMER_ID && dwNow >= (m_dwLastTimerEvent + SHEET_TIMER_INTERVAL))
	{
		m_dwLastTimerEvent = ::GetTickCount();

		// attempt to connect to the IISxpress server
		bool bConnected = ConnectToServer();

		// fire off the heartbeat to the client pages
		int nPage = GetPageCount();
		for (int i = 0; i < nPage; i++)
		{
			CPropertyPage* pPage = GetPage(i);
			if (pPage->IsKindOf(RUNTIME_CLASS(CIISxpressPageBase)) == TRUE)
			{
				CIISxpressPageBase* pPageBase = (CIISxpressPageBase*) GetPage(i);
				pPageBase->Heartbeat();
			}
		}

		UpdateTrayTip(bConnected);

		// if we haven't set the caption (applies at startup only) then do it now
		if (bConnected == true && m_bCaptionSet == false)
		{
			SetCaptionEdition();
			m_bCaptionSet = true;
		}
		else
		{
			// we are offline, so we want to set the caption next time the server comes up
			m_bCaptionSet = false;
		}
	}
}

void CIISxpressTrayPropertySheet::UpdateTrayTip(bool bConnected)
{
	// track the last update
	static DWORD dwLastFiles = -1;

	if (bConnected == true)
	{			
		CComPtr<ICompressionRuleManager> pCompressionManager;
		GetCompressionManager(&pCompressionManager);
		CComQIPtr<ICompressionStats> pStats = pCompressionManager;

		bool bTipUpdated = false;

		if (pStats != NULL)
		{				
			DWORD dwFiles = 0;
			DWORD dwRawSize = 0;
			DWORD dwCompressedSize = 0;
			if (pStats->GetStats(&dwFiles, &dwRawSize, &dwCompressedSize) == S_OK)
			{
				// we are going to update the tip
				bTipUpdated = true;

				// only update the tip if the number of files has changed
				if (dwLastFiles != dwFiles)
				{
					// calculate the ratio (prevent divide by zero)
					float fRatio = (float) (dwRawSize - dwCompressedSize);
					fRatio /= dwRawSize > 0 ? dwRawSize : 1;
					fRatio *= 100;

					// prevent going negative
					if (fRatio < 0.0f)
						fRatio = 0.0f;

					CString sRatio;
					sRatio.Format(_T("%.1f"), fRatio);

					// prevent problem of formatting as .0 (rather than 0.0)
					if (fRatio != 0.0f)
						CInternationalHelper::FormatFloatNumber(sRatio, 1);

					CString sFiles;
					sFiles.Format(_T("%u"), dwFiles);
					CInternationalHelper::FormatIntegerNumber(sFiles);

					CString sRawSize;
					sRawSize.Format(_T("%u"), dwRawSize);
					CInternationalHelper::FormatIntegerNumber(sRawSize);

					CString sCompressedSize;
					sCompressedSize.Format(_T("%u"), dwCompressedSize);
					CInternationalHelper::FormatIntegerNumber(sCompressedSize);

					CString sMsg;
					sMsg.Format(IDS_TRAYTIP_IISXPRESSRUNNING, sRatio, sFiles, sRawSize, sCompressedSize);

					m_TrayIcon.SetTrayString(sMsg);						
					
					::InterlockedExchange((LONG*) &dwLastFiles, dwFiles);
				}
			}
		}

		// for the paranoid case
		if (bTipUpdated == false)
		{
			m_TrayIcon.SetTrayString(IDS_IISXPRESS_APPNAME);
			::InterlockedExchange((LONG*) &dwLastFiles, -1);
		}
	}
	else
	{			
		m_TrayIcon.SetTrayString(IDS_TRAYTIP_IISXPRESSSTOPPPED);

		// we need to force a tip update when we come online again			
		::InterlockedExchange((LONG*) &dwLastFiles, -1);
	}
}

bool CIISxpressTrayPropertySheet::ConnectToServer(void)
{
	static bool bConnected = false;

	if (GetIISxpressInterfaces() == S_OK)
	{
		if (bConnected == false)
		{
			int nPage = GetPageCount();
			for (int i = 0; i < nPage; i++)
			{
				CPropertyPage* pPage = GetPage(i);
				if (pPage->IsKindOf(RUNTIME_CLASS(CIISxpressPageBase)) == TRUE)
				{
					CIISxpressPageBase* pPageBase = (CIISxpressPageBase*) GetPage(i);
					pPageBase->ServerOnline();
				}
			}

			bConnected = true;
		}
	}
	else
	{
		if (bConnected == true)
		{
			int nPage = GetPageCount();
			for (int i = 0; i < nPage; i++)
			{
				CPropertyPage* pPage = GetPage(i);
				if (pPage->IsKindOf(RUNTIME_CLASS(CIISxpressPageBase)) == TRUE)
				{
					CIISxpressPageBase* pPageBase = (CIISxpressPageBase*) GetPage(i);
					pPageBase->ServerOffline();
				}
			}
		}

		bConnected = false;
	}

	return bConnected;
}

SC_HANDLE CIISxpressTrayPropertySheet::GetServiceManager(void)
{
	if (m_hSMStatus == NULL)
	{
		m_hSMStatus = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT | STANDARD_RIGHTS_READ);
	}

	return m_hSMStatus;
}

void CIISxpressTrayPropertySheet::CloseServiceManager(void)
{
	if (m_hSMStatus != NULL)
	{
		::CloseServiceHandle(m_hSMStatus);
		m_hSMStatus = NULL;
	}
}

void CIISxpressTrayPropertySheet::OnDestroy(void)
{
	StopTimer();
	CloseServiceManager();

	m_TrayIcon.DeleteTrayIcon();

	// get the window location
	CRect rcSheet;
	GetWindowRect(rcSheet);

	// save the window location to the registry
	CIISxpressTrayApp* pApp = (CIISxpressTrayApp*) ::AfxGetApp();
	pApp->WriteProfileInt(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_WINDOWPOSX, rcSheet.left);
	pApp->WriteProfileInt(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_WINDOWPOSY, rcSheet.top);

	// save the display cookie
	CString sDisplayCookie;
	MakeDisplayCookie(sDisplayCookie);
	pApp->WriteProfileString(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_DISPLAYCOOKIE, sDisplayCookie);
}

void CIISxpressTrayPropertySheet::OnClose(void)
{
	ShowWindow(SW_HIDE);
}

LRESULT CIISxpressTrayPropertySheet::OnTrayMsg(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDBLCLK || lParam == NIN_BALLOONUSERCLICK)
	{
		if (ShowWindow(SW_SHOW) == 0)
		{
			SetActivePage(0);
		}

		SetForegroundWindow();
	}	
	else if (lParam == WM_RBUTTONDOWN)
	{		
		CMenu Menu;
		Menu.LoadMenu(IDR_MENUTRAY);
		CMenu* pSubMenu = Menu.GetSubMenu(0);

		CPoint pos;
		::GetCursorPos(&pos);

		SetForegroundWindow();

		MENUITEMINFO MenuItemInfo;
		MenuItemInfo.cbSize = sizeof(MENUITEMINFO);
		MenuItemInfo.fMask = MIIM_STATE;

		// make the 'Status' menu item bold
		if (pSubMenu->GetMenuItemInfo(ID_TRAYMENU_STATUS, &MenuItemInfo, FALSE) == TRUE)
		{
			MenuItemInfo.fState |= MFS_DEFAULT;
			pSubMenu->SetMenuItemInfo(ID_TRAYMENU_STATUS, &MenuItemInfo, FALSE);
		}

		pSubMenu->TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this, NULL);

		SendMessage(WM_NULL);
	}

	return 0;
}

void CIISxpressTrayPropertySheet::OnTrayStatus(void)
{
	if (ShowWindow(SW_SHOW) == 0)
	{
		SetActivePage(0);
	}	
}

void CIISxpressTrayPropertySheet::OnTrayExit(void)
{
	DestroyWindow();
}

void CIISxpressTrayPropertySheet::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		// if the user selects X from the top right of the window then this really means
		// hide not close
		ShowWindow(SW_HIDE);		
	}
	else if ((nID & 0xfff0) == IDM_ALWAYSONTOP)
	{
		// assume the menu item will not be checked
		UINT nCheck = MF_UNCHECKED;

		// get the extended style flags, if EX_TOPMOST is set then we are at the top of the window order
		DWORD dwExStyle = GetExStyle();
		if ((dwExStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST)
		{
			// we want rid of the topmost
			SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			// we want the menu item unchecked
			nCheck = MF_UNCHECKED;
		}
		else
		{
			// we want to be topmost
			SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			// we want the menu item checked
			nCheck = MF_CHECKED;
		}

		// update the system menu with the check state
		CMenu* pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu != NULL)
		{
			pSysMenu->CheckMenuItem(IDM_ALWAYSONTOP, MF_BYCOMMAND | nCheck);
		}
	}
	else
	{
		CPropertySheet::OnSysCommand(nID, lParam);
	}
}

void CIISxpressTrayPropertySheet::OnTrayHelp(void)
{
	TCHAR szModuleFileName[512];
	::GetModuleFileName(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(szModuleFileName[0]));

	CPathT<CString> HelpFileName = szModuleFileName;
	HelpFileName.RenameExtension(_T(".chm"));	

	// ask HtmlHelp to display the contents page
	::HtmlHelp(this->GetSafeHwnd(), HelpFileName, HH_DISPLAY_TOC, 0);
}

void CIISxpressTrayPropertySheet::SetCaptionEdition(void)
{
	CString sCaption;

	// get the request object
	CComPtr<IComIISxpressRegSettings> pRegSettings;
	HRESULT hr = GetIISxpressServerRegSettings(&pRegSettings);	

	// get the license mode
	LicenseMode licenseMode = LicenseModeCommunity;
	if (pRegSettings != NULL)
	{
		hr = pRegSettings->get_LicenseMode(&licenseMode);
	}

	if (licenseMode == LicenseModeCommunity)
	{
		sCaption.LoadString(IDS_IISXPRESSSTATUS_COMMUNITYEDITION);
	}
	else if (licenseMode == LicenseModeFull)
	{
		CComPtr<IProductLicenseManager> pLicenseManager;
		if (GetProductLicenseManager(&pLicenseManager) != S_OK || 
			pLicenseManager == NULL || 
			pLicenseManager->IsRegisteredKey() == S_OK)
		{
			sCaption.LoadString(IDS_IISXPRESSSTATUS);
		}
		else
		{
			licenseMode = LicenseModeEvaluation;
		}
	}
	else if (licenseMode == LicenseModeAHA)
	{
		sCaption.LoadString(IDS_IISXPRESSSTATUS_AHAEDITION);
	}

	if (licenseMode == LicenseModeEvaluation)
	{
		sCaption.LoadString(IDS_IISXPRESSSTATUS_EVALUATIONEDITION);
	}	
	
	SetWindowText(sCaption);
}

HRESULT CIISxpressTrayPropertySheet::GetIIS7ConfigDocument(IXMLDOMDocument** ppDoc)
{
	if (ppDoc == NULL)
		return E_POINTER;	

	HANDLE hConfigFile = ::CreateFile(m_sConfigFilePath, FILE_READ_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hConfigFile == INVALID_HANDLE_VALUE)
		return E_FAIL;	

	// get the last modified time of the config file
	FILETIME ftLastModified;
	::GetFileTime(hConfigFile, NULL, NULL, &ftLastModified);
	::CloseHandle(hConfigFile);
	hConfigFile = NULL;

	// if the config file has changed then we need to read it
	if (memcmp((void*) &m_ftConfigFile, &ftLastModified, sizeof(m_ftConfigFile)) != 0)
	{		
		CComPtr<IXMLDOMDocument> pDoc;
		HRESULT hr = pDoc.CoCreateInstance(L"MSXML.DOMDocument");
		if (hr != S_OK)
			return hr;

		// TODO: implement this or not?
		// switch async off
		//VARIANT_BOOL vbAsync = 0;
		//hr = pDoc->put_async(vbAsync);		

		VARIANT_BOOL bSuccess;
		hr = pDoc->load(CComVariant((CString::PCXSTR) m_sConfigFilePath), &bSuccess);
		if (hr != S_OK)
			return hr;

		m_pConfigDoc = pDoc;

		memcpy((void*) &m_ftConfigFile, &ftLastModified, sizeof(m_ftConfigFile));
	}

	m_pConfigDoc.CopyTo(ppDoc);

	return S_OK;
}

void CIISxpressTrayPropertySheet::VersionCheck(void)
{
	// load the message box caption
	CString sCaption;
	sCaption.LoadString(IDS_VERSIONCHECK_CAPTION);

	// load the message box prompt string
	CString sMsg;
	sMsg.LoadString(IDS_VERSIONCHECK_PROMPT);

	// show the message box
	if (MessageBox(sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	// get the file path
	TCHAR szFileName[MAX_PATH + 1] = _T("");
	::GetModuleFileName(NULL, szFileName, MAX_PATH);

	// we need a buffer for the version info
	BYTE byBuffer[4 * 1048];
	memset(byBuffer, 0, sizeof(byBuffer));

	// get the version info
	BOOL bStatus = ::GetFileVersionInfo(szFileName, 0, sizeof(byBuffer), byBuffer);

	VS_FIXEDFILEINFO* pFileInfo;
	UINT nFileInfoLength = 0;
	bStatus = ::VerQueryValue(byBuffer, _T("\\"), (void**) &pFileInfo, &nFileInfoLength);

	try
	{
		CWaitCursor Cursor;

		// we need a DOM
		CComPtr<IXMLDOMDocument> pDOM;
		HRESULT hr = pDOM.CoCreateInstance(L"MSXML.DOMDocument");	
		if (hr != S_OK)
			throw hr;

		// switch off async since we want an answer NOW!
		hr = pDOM->put_async(0);
		if (FAILED(hr) == TRUE)
			throw hr;

		CString sURL;
		GetUpdateURL(sURL);

		CComBSTR bsURL(sURL);

		// query the server
		VARIANT_BOOL vbSuccess = 0;
		hr = pDOM->load(CComVariant(bsURL), &vbSuccess);

		// we've done our business, restore the cursor
		Cursor.Restore();

		// check what happened
		if (hr != S_OK)
			throw hr;							
		
#if defined _M_IX86				
		LPCWSTR pszProcessor = L"x86";
#elif defined _M_X64				
		LPCWSTR pszProcessor = L"x64";
#else
#pragma message("The processor type for the version check could not be deduced")
#endif

		CAtlStringW sVersionNodeQuery;
		sVersionNodeQuery.Format(
			L"/versions/version[@processor='%s' and @platform='%u.%u' and @workstation='%u']",
			pszProcessor,
			g_VerInfo.dwMajorVersion, g_VerInfo.dwMinorVersion, 
			g_VerInfo.wProductType == VER_NT_WORKSTATION);

		CComBSTR bsVersionNodeQuery;
		bsVersionNodeQuery.Attach(sVersionNodeQuery.AllocSysString());

		// get the version node
		CComPtr<IXMLDOMNode> pVersionNode;
		hr = pDOM->selectSingleNode(bsVersionNodeQuery, &pVersionNode);
		if (hr != S_OK)
			throw E_FAIL;

		// get the version value
		CComPtr<IXMLDOMNode> pVersionValueNode;
		hr = pVersionNode->selectSingleNode(CComBSTR(L"@value"), &pVersionValueNode);
		if (hr != S_OK)
			throw E_FAIL;

		CComBSTR bsVersion;
		hr = pVersionValueNode->get_text(&bsVersion);
		if (hr != S_OK)
			throw E_FAIL;

		CComPtr<IXMLDOMNode> pDownloadNode;
		hr = pVersionNode->selectSingleNode(CComBSTR(L"@download"), &pDownloadNode);
		if (hr != S_OK)
			throw E_FAIL;

		CComBSTR bsDownloadURL;
		hr = pDownloadNode->get_text(&bsDownloadURL);	
		if (hr != S_OK)
			throw E_FAIL;

		DWORD dwUpgradeMajor = 0, dwUpgradeMinor = 0;
		if (MakeVersionDWORDs(bsVersion, &dwUpgradeMajor, &dwUpgradeMinor) != true)
			throw E_FAIL;

		// determine whether the available version is newer
		if (dwUpgradeMajor > pFileInfo->dwProductVersionMS ||
			(dwUpgradeMajor == pFileInfo->dwProductVersionMS && dwUpgradeMinor > pFileInfo->dwProductVersionLS))
		{
			CString sMsg;
			sMsg.LoadString(IDS_VERSIONCHECK_UPDATEAVAILABLE);

			// prompt the user to confirm download
			if (MessageBox(sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				// ask the shell to download
				::ShellExecute(::GetDesktopWindow(), _T("open"), 
					CString(bsDownloadURL), NULL, NULL, SW_SHOW);			
			}
		}
		else
		{
			CString sMsg;
			sMsg.LoadString(IDS_VERSIONCHECK_NOUPDATES);

			// there was no newer version available, let the user know
			MessageBox(sMsg, sCaption, MB_OK | MB_ICONINFORMATION);
		}
	}
	catch (HRESULT hr)
	{
		CString sMsg;
		sMsg.Format(IDS_VERSIONCHECK_FAILED, hr);

		// there was a fatal error, let the user know
		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);
	}
}

bool CIISxpressTrayPropertySheet::MakeVersionDWORDs(LPCWSTR pszVersionString, DWORD* pdwMajor, DWORD* pdwMinor) 
{
	if (pszVersionString == NULL || pdwMajor == NULL || pdwMinor == NULL)
		return false;	

	*pdwMajor = _wtoi(pszVersionString);
	*pdwMajor <<= 16;

	pszVersionString = wcschr(pszVersionString, '.');
	if (pszVersionString == NULL)
		return false;		

	pszVersionString++;

	*pdwMajor |= _wtoi(pszVersionString);

	pszVersionString = wcschr(pszVersionString, '.');
	if (pszVersionString == NULL)
		return false;

	pszVersionString++;

	*pdwMinor = _wtoi(pszVersionString);
	*pdwMinor <<= 16;

	pszVersionString = wcschr(pszVersionString, '.');
	if (pszVersionString == NULL)
		return false;

	pszVersionString++;

	*pdwMinor |= _wtoi(pszVersionString);

	return true;
}

void CIISxpressTrayPropertySheet::GetUpdateURL(CString& sURL)
{
	sURL = _T("http://www.ripcordsoftware.com/IISxpress/version.xml");

	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, IISXPRESSTRAY_REGKEY, KEY_READ) == ERROR_SUCCESS)
	{	
		TCHAR szBuffer[2048] = _T("\0");
		ULONG bufferSize = _countof(szBuffer);

		if (Config.QueryStringValue(IISXPRESSTRAY_UPDATEURL, szBuffer, &bufferSize) == ERROR_SUCCESS)
		{
			sURL = szBuffer;
		}
	}
}

void CIISxpressTrayPropertySheet::OnTrayCheckForUpdates()
{
	VersionCheck();
}

void CIISxpressTrayPropertySheet::ResetResponseCache()
{
	CIISxpressRegSettingsEx reg;
	reg.Init(IISXPRESSFILTER_REGKEY, false, false);
	reg.SetCacheStateCookie(::GetTickCount());
}

DWORD CIISxpressTrayPropertySheet::GetUpdateCookie()
{
	DWORD dwUpdateCookie = 0;

	CComPtr<ICompressionRuleManager> pCompressionManager;
	GetCompressionManager(&pCompressionManager);
	if (pCompressionManager != NULL)
	{			
		CComQIPtr<INotifyCookie> pNotifyCookie = pCompressionManager;
		if (pNotifyCookie != NULL)
		{
			pNotifyCookie->GetCookie(&dwUpdateCookie);
		}
	}

	return dwUpdateCookie;
}

void CIISxpressTrayPropertySheet::MakeDisplayCookie(CString& sCookie)
{
	sCookie.Format(_T("%d%d%d%d"), 
		::GetSystemMetrics(SM_XVIRTUALSCREEN), ::GetSystemMetrics(SM_YVIRTUALSCREEN),
		::GetSystemMetrics(SM_CXVIRTUALSCREEN), ::GetSystemMetrics(SM_CYVIRTUALSCREEN));
}