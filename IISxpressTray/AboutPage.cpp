// AboutPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AboutPage.h"

#include "resource.h"

#include "EnterProductKeyDlg.h"

#include "IISxpressInstallerExtensions.h"

#pragma warning (disable: 4312)

#define LINKEDREGSTATUS_ID 9999

// CAboutPage dialog

IMPLEMENT_DYNAMIC(CAboutPage, CIISxpressPageBase)

CAboutPage::CAboutPage()
	: CIISxpressPageBase(CAboutPage::IDD), m_hOldCursor(NULL)
{
	m_pcRegStatus = NULL;	
}

CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CIISxpressPageBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COPYRIGHTWARNING, m_cCopyrightWarning);
	DDX_Control(pDX, IDC_STATIC_RIPCORDSOFTWAREHOTSPOT, m_cRipcordSoftwareHotspot);
	DDX_Control(pDX, IDC_STATIC_REGSTATUS, m_cPlainRegStatus);
	DDX_Control(pDX, IDC_BUTTON_PURCHASE, m_cPurchase);
	DDX_Control(pDX, IDC_BUTTON_ENTERKEY, m_cEnterKey);
}

BEGIN_MESSAGE_MAP(CAboutPage, CIISxpressPageBase)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PURCHASE, OnBnClickedButtonPurchase)
	ON_BN_CLICKED(IDC_BUTTON_ENTERKEY, OnBnClickedButtonEnterKey)	
	ON_NOTIFY(NM_CLICK, LINKEDREGSTATUS_ID, OnURLClick)
	ON_NOTIFY(NM_RETURN, LINKEDREGSTATUS_ID, OnURLClick)
	ON_BN_CLICKED(IDC_BUTTON_CHECKFORUPDATES, OnBnClickedButtonCheckForUpdates)
END_MESSAGE_MAP()


// CAboutPage message handlers

BOOL CAboutPage::OnInitDialog(void)
{
	BOOL bStatus = CIISxpressPageBase::OnInitDialog();

	CString sCopyright;
	sCopyright.LoadString(IDS_MSG_COPYRIGHTWARNING);
	m_cCopyrightWarning.SetWindowText(sCopyright);

	HINSTANCE hInst = (HINSTANCE) ::GetWindowLongPtr(this->m_hWnd, GWLP_HINSTANCE);
	m_hHandCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSORHAND));

	// if we are running XP or above then we can use the LinkCtrl
	if ((g_VerInfo.dwMajorVersion == 5 && g_VerInfo.dwMinorVersion >= 1) || g_VerInfo.dwMajorVersion >= 6)
	{
		CRect rcRegStatus;
		m_cPlainRegStatus.GetWindowRect(rcRegStatus);
		ScreenToClient(rcRegStatus);

		m_cLinkedRegStatus.Create(WS_CHILD | WS_TABSTOP, rcRegStatus, this, LINKEDREGSTATUS_ID);
		m_pcRegStatus = &m_cLinkedRegStatus;

		// for some reason Vista uses a bold/larger font - so get rid of it
		if (g_VerInfo.dwMajorVersion >= 6)
		{			
			m_pcRegStatus->SetFont(GetFont());
		}
	}
	else
	{
		m_pcRegStatus = &m_cPlainRegStatus;
	}

	return bStatus; 
}

HBRUSH CAboutPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	/*if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		//return (HBRUSH) ::GetStockObject(NULL_BRUSH);
		return (HBRUSH) ::GetStockObject(WHITE_BRUSH);
	}
	else if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH) ::GetStockObject(NULL_BRUSH);
	}
	else
	{
		return CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	}*/

	//pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH) ::GetStockObject(WHITE_BRUSH);
}


void CAboutPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rcHotspot;
	m_cRipcordSoftwareHotspot.GetWindowRect(&rcHotspot);
	this->ScreenToClient(rcHotspot);

	if (rcHotspot.PtInRect(point) == TRUE)
	{
		::ShellExecute(::GetDesktopWindow(), _T("open"), _T("http://www.ripcordsoftware.com"), NULL, NULL, SW_SHOW);
	}
}

BOOL CAboutPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	GetCursorPos(&point);

	CRect rcHotspot;
	m_cRipcordSoftwareHotspot.GetWindowRect(&rcHotspot);		

	if (rcHotspot.PtInRect(point) == TRUE && m_hHandCursor != NULL)
	{
		HCURSOR hOldCursor = SetCursor(m_hHandCursor);		
		if (hOldCursor != m_hHandCursor)
		{
			m_hOldCursor = hOldCursor;
		}

		return 1;
	}	
	else if (m_hOldCursor != NULL)
	{
		SetCursor(m_hOldCursor);		
	}

	return 0;
}


void CAboutPage::OnDestroy(void)
{
	if (m_hHandCursor != NULL)
	{
		::DestroyCursor(m_hHandCursor);
	}
}

void CAboutPage::OnBnClickedButtonPurchase()
{
	::ShellExecute(::GetDesktopWindow(), _T("open"), 
		_T("http://www.ripcordsoftware.com/IISxpress/purchase.aspx"), NULL, NULL, SW_SHOW);
}

void CAboutPage::OnBnClickedButtonEnterKey()
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<IProductLicenseManager> pProductLicenseManager;
	if (pSheet->GetProductLicenseManager(&pProductLicenseManager) != S_OK ||
		pProductLicenseManager == NULL)
	{
		return;
	}

	CComPtr<IIISxpressHTTPRequest> pRequest;
	pSheet->GetHTTPRequest(&pRequest);

	CComPtr<IComIISxpressRegSettings> pRegSettings;
	pSheet->GetIISxpressServerRegSettings(&pRegSettings);

	::AfxInitRichEdit2();

	CEnterProductKeyDlg dlg;
	dlg.Init(pProductLicenseManager, pRegSettings);
	if (dlg.DoModal() == IDOK)
	{
		// quietly update the registration controls
		UpdateRegistrationControls(true);
	}
}

void CAboutPage::UpdateRegistrationControls(bool bQuiet)
{	
	if (::IsWindow(GetSafeHwnd()) == FALSE)
		return;

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<IIISxpressHTTPRequest> pRequest;
	HRESULT hr = pSheet->GetHTTPRequest(&pRequest);

	CComPtr<IComIISxpressRegSettings> pRegSettings;
	hr = pSheet->GetIISxpressServerRegSettings(&pRegSettings);

	// if we don't have a connection to the server then just hide everything
	if (hr != S_OK || pRequest == NULL || pRegSettings == NULL)
	{
		m_pcRegStatus->ShowWindow(SW_HIDE);
		m_cPurchase.ShowWindow(SW_HIDE);
		m_cEnterKey.ShowWindow(SW_HIDE);
		return;
	}
		
	// we have some kind of connection to the server, so show all the controls
	m_pcRegStatus->ShowWindow(SW_SHOW);
	m_cPurchase.ShowWindow(SW_SHOW);
	m_cEnterKey.ShowWindow(SW_SHOW);

	// get the license mode
	LicenseMode licenseMode = LicenseModeCommunity;
	hr = pRegSettings->get_LicenseMode(&licenseMode);

	if (licenseMode == LicenseModeCommunity)
	{
		// community license
		CString sNoProductKey;	
		sNoProductKey.LoadString(IDS_STRING_COMMUNITYEDITION);
		m_pcRegStatus->SetWindowText(sNoProductKey);			

		// disable the purchase and enter buttons on workstations, otherwise they are on display
		if (g_VerInfo.wProductType == VER_NT_WORKSTATION)
		{
			m_cPurchase.EnableWindow(FALSE);
			m_cEnterKey.EnableWindow(FALSE);	
		}
		else
		{
			m_cPurchase.EnableWindow(TRUE);
			m_cEnterKey.EnableWindow(TRUE);	
		}
	}
	else if (licenseMode == LicenseModeAHA)
	{
		CString sAHAEdition;	
		if (m_pcRegStatus->IsKindOf(RUNTIME_CLASS(CLinkCtrl)) == TRUE)
		{
			sAHAEdition.LoadString(IDS_AHAEDITION_LINKED);
		}
		else
		{
			sAHAEdition.LoadString(IDS_AHAEDITION);
		}

		m_pcRegStatus->SetWindowText(sAHAEdition);			

		// hide the purchase and enter buttons, otherwise they are on display
		m_cPurchase.ShowWindow(SW_HIDE);
		m_cEnterKey.ShowWindow(SW_HIDE);	
	}
	else if (licenseMode == LicenseModeFull)
	{
		CComPtr<IProductLicenseManager> pProductLicenseManager;
		hr = pSheet->GetProductLicenseManager(&pProductLicenseManager);
		if (hr == S_OK && pProductLicenseManager->IsRegisteredKey() == S_OK)
		{
			// the product is registered
			CString sRegistered;
			if (m_pcRegStatus->IsKindOf(RUNTIME_CLASS(CLinkCtrl)) == TRUE)
			{
				sRegistered.LoadString(IDS_STRING_PRODUCTREGISTERED_LINKED);
			}
			else
			{
				sRegistered.LoadString(IDS_STRING_PRODUCTREGISTERED);
			}

			m_pcRegStatus->SetWindowText(sRegistered);
			
			// we need to disable the purchase and enter key buttons
			m_cPurchase.EnableWindow(FALSE);
			m_cEnterKey.EnableWindow(FALSE);			
		}		
		else
		{
			// something is wrong with the license key, default back to evaluation
			licenseMode = LicenseModeEvaluation;
		}
	}		

	if (licenseMode == LicenseModeEvaluation)
	{
		// evaluation license, enable the purchase and enter buttons
		CString sNoProductKey;	
		sNoProductKey.LoadString(IDS_STRING_EVALUATIONEDITION);
		m_pcRegStatus->SetWindowText(sNoProductKey);			

		m_cPurchase.EnableWindow(TRUE);
		m_cEnterKey.EnableWindow(TRUE);			
	}

	// display the nag window if the user is admin
	if (bQuiet == false && 
		(licenseMode == LicenseModeCommunity || licenseMode == LicenseModeEvaluation) &&		
		IsUserAdmin() == 1)
	{
		// we can safely assume version 4 of the shell for now
		DWORD dwShellMajorVersion = 4;

		// get the version of the shell
		typedef HRESULT CALLBACK FNDLLGETVERSION(DLLVERSIONINFO *pdvi);
		HMODULE hShell = ::LoadLibrary(_T("shell32.dll"));
		if (hShell != NULL)
		{
			FNDLLGETVERSION* pfnDllGetVersion = (FNDLLGETVERSION*) ::GetProcAddress(hShell, "DllGetVersion");
			if (pfnDllGetVersion != NULL)
			{
				DLLVERSIONINFO VersionInfo;
				VersionInfo.cbSize = sizeof(DLLVERSIONINFO);

				if (pfnDllGetVersion(&VersionInfo) == S_OK)
				{
					dwShellMajorVersion = VersionInfo.dwMajorVersion;				
				}
			}

			::FreeLibrary(hShell);
		}

		// only when the shell is >= 5.x do we display the nag balloon
		if (dwShellMajorVersion >= 5)
		{
			LPCTSTR pszCommandLine = ::GetCommandLine();		
			if (pszCommandLine != NULL && _tcsstr(pszCommandLine, _T("/installer")) == NULL)
			{
				NOTIFYICONDATA nid;
				memset(&nid, 0, sizeof(nid));	

				nid.cbSize = sizeof(nid);
				nid.hWnd = pSheet->GetSafeHwnd();
				nid.uID = pSheet->GetTrayNotifyId();
				nid.uFlags = NIF_INFO;			
				nid.uTimeout = 10000;
				nid.dwInfoFlags = NIIF_INFO;

				CString sAppName;
				sAppName.LoadString(IDS_IISXPRESS_APPNAME);

				CString sInfo;	
				if (licenseMode == LicenseModeCommunity)				
					sInfo.LoadString(IDS_STRING_COMMUNITYEDITION_NOTIFY);
				else
					sInfo.LoadString(IDS_STRING_EVALUATIONEDITION_NOTIFY);

				_tcscpy_s(nid.szInfoTitle, _countof(nid.szInfoTitle), sAppName);
				_tcscpy_s(nid.szInfo, _countof(nid.szInfo), sInfo);						

				::Shell_NotifyIcon(NIM_MODIFY, &nid);
			}
		}
	}
}

void CAboutPage::ServerOnline(void)
{
	UpdateRegistrationControls();
}

void CAboutPage::ServerOffline(void)
{
	if (::IsWindow(GetSafeHwnd()) == TRUE)
	{
		m_pcRegStatus->ShowWindow(SW_HIDE);
		m_cPurchase.ShowWindow(SW_HIDE);
		m_cEnterKey.ShowWindow(SW_HIDE);
	}
}

void CAboutPage::OnURLClick(NMHDR* pNotifyStruct, LRESULT * result)
{
	NMLINK* pNmLink = (NMLINK*) pNotifyStruct;
	if (pNmLink->item.szUrl != NULL && _tcslen(pNmLink->item.szUrl) > 0)
	{
		::ShellExecute(::GetDesktopWindow(), _T("open"), pNmLink->item.szUrl, _T(""), NULL, SW_SHOW);
	}
}
void CAboutPage::OnBnClickedButtonCheckForUpdates()
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();
	pSheet->VersionCheck();
}
