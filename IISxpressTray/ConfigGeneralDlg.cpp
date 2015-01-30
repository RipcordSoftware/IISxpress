// ServicesPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ConfigGeneralDlg.h"
#include "RegistryKeys.h"
#include "IISxpressRegSettings.h"

#include "RepairingFilterDlg.h"

#include "resource.h"

// CConfigPage dialog

IMPLEMENT_DYNAMIC(CConfigGeneralDlg, CHelpAwareDialogBase)

CConfigGeneralDlg::CConfigGeneralDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CConfigGeneralDlg::IDD, pParent)
{
}

CConfigGeneralDlg::~CConfigGeneralDlg()
{
}

void CConfigGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILTER_STATUS, m_cFilterStatus);
	DDX_Control(pDX, IDC_CHECK_FILTERENABLE, m_cFilterEnable);
	DDX_Control(pDX, IDC_EDIT_SERVER_STATUS, m_cIISxpressStatus);
	DDX_Control(pDX, IDC_BUTTON_IISXPRESSSERVERSTART, m_cIISxpresStart);
	DDX_Control(pDX, IDC_BUTTON_IISXPRESSSERVERSTOP, m_cIISxpressStop);		
	DDX_Control(pDX, IDC_COMBO_IISXPRESSSERVERCOMPLEVEL, m_cCompressionLevel);
	DDX_Control(pDX, IDC_BUTTON_RESETCOMPSTAT, m_cResetCompression);	
	DDX_Control(pDX, IDC_COMBO_IISXPRESSSERVERBZIP2MODE, m_cBZIP2Mode);
}


BEGIN_MESSAGE_MAP(CConfigGeneralDlg, CHelpAwareDialogBase)	
	ON_BN_CLICKED(IDC_CHECK_FILTERENABLE, OnBnClickedCheckFilterEnable)
	ON_BN_CLICKED(IDC_BUTTON_IISXPRESSSERVERSTART, OnBnClickedButtonIISxpressServerStart)
	ON_BN_CLICKED(IDC_BUTTON_IISXPRESSSERVERSTOP, OnBnClickedButtonIISxpressServerStop)
	ON_CBN_SELCHANGE(IDC_COMBO_IISXPRESSSERVERCOMPLEVEL, OnCbnSelchangeComboCompressionMode)
	ON_CBN_SELCHANGE(IDC_COMBO_IISXPRESSSERVERBZIP2MODE, OnCbnSelchangeComboBZIP2Mode)
	ON_BN_CLICKED(IDC_BUTTON_RESETCOMPSTAT, OnBnClickedResetCompressionStats)	
END_MESSAGE_MAP()


// CConfigGeneralDlg message handlers

BOOL CConfigGeneralDlg::OnInitDialog(void)
{
	BOOL bStatus = CHelpAwareDialogBase::OnInitDialog();	

	CIISxpressRegSettings RegSettings;
	RegSettings.Init(IISXPRESSFILTER_REGKEY, false);
	if (RegSettings.Load() == true)
	{
		if (RegSettings.GetEnabled() == true)
		{
			m_cFilterEnable.SetCheck(BST_CHECKED);
		}
		else
		{
			m_cFilterEnable.SetCheck(0);
		}		
	}	

	// ********************************************************************************

	CString sCompModeDefault;
	sCompModeDefault.LoadString(IDS_COMPMODE_DEFAULT);
	int nItem = m_cCompressionLevel.AddString(sCompModeDefault);
	m_cCompressionLevel.SetItemData(nItem, CompressionModeDefault);

	CString sCompModeSpeed;
	sCompModeSpeed.LoadString(IDS_COMPMODE_SPEED);
	nItem = m_cCompressionLevel.AddString(sCompModeSpeed);
	m_cCompressionLevel.SetItemData(nItem, CompressionModeSpeed);

	CString sCompModeSize;
	sCompModeSize.LoadString(IDS_COMPMODE_SIZE);
	nItem = m_cCompressionLevel.AddString(sCompModeSize);
	m_cCompressionLevel.SetItemData(nItem, CompressionModeSize);

	// *********************************************************************************

	CString sBZIP2ModeDisabled;
	sBZIP2ModeDisabled.LoadString(IDS_BZIP2MODE_DISABLED);
	nItem = m_cBZIP2Mode.AddString(sBZIP2ModeDisabled);
	m_cBZIP2Mode.SetItemData(nItem, BZIP2ModeDisabled);

	CString sBZIP2ModeStaticOnly;
	sBZIP2ModeStaticOnly.LoadString(IDS_BZIP2MODE_STATICONLY);
	nItem = m_cBZIP2Mode.AddString(sBZIP2ModeStaticOnly);
	m_cBZIP2Mode.SetItemData(nItem, BZIP2ModeStaticOnly);

	CString sBZIP2ModeAll;
	sBZIP2ModeAll.LoadString(IDS_BZIP2MODE_ALL);
	nItem = m_cBZIP2Mode.AddString(sBZIP2ModeAll);
	m_cBZIP2Mode.SetItemData(nItem, BZIP2ModeAll);

	// *********************************************************************************

	UpdateControls();

	return bStatus;
}

void CConfigGeneralDlg::ServerOnline(void)
{
}

void CConfigGeneralDlg::ServerOffline(void)
{
}

void CConfigGeneralDlg::Heartbeat(void)
{
	UpdateControls();
}

void CConfigGeneralDlg::UpdateControls(void)
{
	UpdateFilterState();
	UpdateIISxpressStatus();
}

void CConfigGeneralDlg::UpdateFilterState(void)
{
	CString sFilterState;
	sFilterState.LoadString(IDS_FILTERSTATE_UNKNOWN);

	// Windows 2003 and earlier
	if (g_VerInfo.dwMajorVersion < 6)
	{
		CIISMetaBase MetaBase;	
		CComPtr<IMSAdminBase> pAdminBase;

		CIISMetaBaseData FilterLoadOrder;	
		if (MetaBase.Init() == true &&
			MetaBase.GetAdminBase(&pAdminBase) == true &&
			FilterLoadOrder.ReadData(pAdminBase, L"/LM/W3SVC/Filters", MD_FILTER_LOAD_ORDER) == true)
		{
			std::wstring sLoadOrder;
			if (FilterLoadOrder.GetAsString(sLoadOrder) == true &&
				sLoadOrder.find(L"IISxpress") != std::wstring::npos)
			{
				CIISMetaBaseData IISxpressFilterState;
				if (IISxpressFilterState.ReadData(pAdminBase, L"/LM/W3SVC/Filters/IISxpress", MD_FILTER_STATE) == true)
				{
					DWORD dwFilterState = 0;
					IISxpressFilterState.GetAsDWORD(&dwFilterState);

					if (dwFilterState == MD_FILTER_STATE_LOADED)
					{
						sFilterState.LoadString(IDS_FILTERSTATE_RUNNING);
					}
					else
					{
						sFilterState.LoadString(IDS_FILTERSTATE_INSTALLED);
					}
				}
				else if (IISxpressFilterState.ReadData(pAdminBase, L"/LM/W3SVC/Filters/IISxpress", MD_FILTER_IMAGE_PATH) == true)
				{
					sFilterState.LoadString(IDS_FILTERSTATE_INSTALLED);
				}
				else
				{
					sFilterState.LoadString(IDS_FILTERSTATE_NOTINSTALLED);
				}
			}
			else
			{
				sFilterState.LoadString(IDS_FILTERSTATE_NOTINSTALLED);
			}
		}
	}
	else
	{
		CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

		CComPtr<IXMLDOMDocument> pDoc;
		HRESULT hr = pSheet->GetIIS7ConfigDocument(&pDoc);		

		if (hr == S_OK && pDoc != NULL)
		{
			CComPtr<IXMLDOMNode> pIISxpressModuleNode;
			hr = pDoc->selectSingleNode(
					L"/configuration/system.webServer/globalModules/add[@name='IISxpressNativeModule']",
					&pIISxpressModuleNode);

			if (hr == S_OK && pIISxpressModuleNode != NULL)
			{
				CComPtr<IXMLDOMNode> pIISxpressModuleLoadedNode;
				hr = pDoc->selectSingleNode(
					L"/configuration/location[@path='']/system.webServer/modules/add[@name='IISxpressNativeModule']",
					&pIISxpressModuleLoadedNode);

				if (hr == S_OK && pIISxpressModuleLoadedNode != NULL)
				{
					sFilterState.LoadString(IDS_FILTERSTATE_RUNNING);
				}
				else
				{
					sFilterState.LoadString(IDS_FILTERSTATE_INSTALLED);
				}
			}
		}
	}

	m_cFilterStatus.SetWindowText(sFilterState);
	m_cFilterStatus.SetSel(-1, 0);
}

void CConfigGeneralDlg::OnBnClickedCheckFilterEnable()
{
	bool bEnabled = false;
	if ((m_cFilterEnable.GetCheck() & BST_CHECKED) == BST_CHECKED)
	{
		bEnabled = true;
	}	
	
	CIISxpressRegSettings RegSettings;
	RegSettings.Init(IISXPRESSFILTER_REGKEY, false);
	if (RegSettings.Load() == true)
	{
		if (bEnabled == true)
		{
			RegSettings.SetEnabled(true, true);
		}
		else
		{
			RegSettings.SetEnabled(false, true);

			CString sCaption;
			sCaption.LoadString(IDS_IISXPRESSSTATUS);

			CString sMsg;
			sMsg.LoadString(IDS_STRING_USERDISABLEDIISXPRESSFILTER);

			MessageBox(sMsg, sCaption, MB_OK | MB_ICONWARNING);					
		}		
	}
}

void CConfigGeneralDlg::UpdateIISxpressStatus(void)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CString sIISxpressState;
	sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_NOTINSTALLED);

	int nRunning = -1;

	// if an NT based platform we deal with the service manager
	if (g_VerInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT)
	{
		SC_HANDLE hSMStatus = pSheet->GetServiceManager();

		if (hSMStatus != NULL)
		{
			SC_HANDLE hIISxpress = ::OpenService(hSMStatus, IISXPRESS_SERVICENAME, SERVICE_QUERY_STATUS);
			if (hIISxpress != NULL)
			{
				sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_UNKNOWN);

				SERVICE_STATUS Status;
				if (::QueryServiceStatus(hIISxpress, &Status) == TRUE)
				{
					if (Status.dwCurrentState == SERVICE_RUNNING)
					{						
						sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_RUNNING);
						nRunning = 1;
					}
					else if (Status.dwCurrentState == SERVICE_STOPPED)
					{
						sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_STOPPED);
						nRunning = 0;
					}
					else if (Status.dwCurrentState == SERVICE_START_PENDING)
					{
						sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_STARTING);
						nRunning = -1;
					}
					else if (Status.dwCurrentState == SERVICE_STOP_PENDING)
					{
						sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_STOPPING);
						nRunning = -1;
					}
					else if (Status.dwCurrentState == SERVICE_PAUSED || Status.dwCurrentState == SERVICE_PAUSE_PENDING)
					{
						sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_PAUSED);
						nRunning = -1;
					}
					else if (Status.dwCurrentState == SERVICE_CONTINUE_PENDING)
					{
						sIISxpressState.LoadString(IDS_IISXPRESS_SERVERSTATE_CONTINUING);
						nRunning = -1;
					}
				}

				::CloseServiceHandle(hIISxpress);
			}
		}
	}

	m_cIISxpressStatus.SetWindowText(sIISxpressState);
	m_cIISxpressStatus.SetSel(-1, 0);

	if (pSheet != NULL)
	{		
		// reset the compression mode
		m_cCompressionLevel.SetCurSel(-1);

		// set the current compression mode
		CComPtr<IIISxpressHTTPRequest> pRequest;
		if (pSheet->GetHTTPRequest(&pRequest) == S_OK && pRequest != NULL)
		{
			CComQIPtr<IIISxpressSettings> pSettings = pRequest;

			ATLASSERT(pSettings != NULL);
			if (pSettings != NULL)
			{
				CompressionMode compressionMode = CompressionModeDefault;
				HRESULT hr = pSettings->get_CompressionMode(&compressionMode);
				if (hr == S_OK)
				{
					for (int i = 0; i < m_cCompressionLevel.GetCount(); i++)
					{
						DWORD dwItemMode = (DWORD) m_cCompressionLevel.GetItemData(i);

						if (compressionMode == dwItemMode)
						{
							m_cCompressionLevel.SetCurSel(i);
							break;
						}
					}
				}
			}
		}

		CComPtr<IComIISxpressRegSettings> pServerRegSettings;
		pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);

		BZIP2Mode bzip2Mode = BZIP2ModeStaticOnly;
		if (pServerRegSettings != NULL)
		{
			pServerRegSettings->get_BZIP2Mode(&bzip2Mode);
		}

		m_cBZIP2Mode.SetCurSel(-1);		

		for (int i = 0; i < m_cBZIP2Mode.GetCount(); i++)
		{
			DWORD dwItemMode = (DWORD) m_cBZIP2Mode.GetItemData(i);

			if (bzip2Mode == dwItemMode)
			{
				m_cBZIP2Mode.SetCurSel(i);
				break;
			}
		}
	}

	if (nRunning == 0)
	{
		// not running
		m_cIISxpresStart.EnableWindow(TRUE);
		m_cIISxpressStop.EnableWindow(FALSE);		
	}
	else if (nRunning > 0)
	{
		// running
		m_cIISxpresStart.EnableWindow(FALSE);
		m_cIISxpressStop.EnableWindow(TRUE);		
	}
	else
	{
		// probably not installed
		m_cIISxpresStart.EnableWindow(FALSE);
		m_cIISxpressStop.EnableWindow(FALSE);		
	}

	// enable/disable the compression level and reset buttons
	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{
		m_cCompressionLevel.EnableWindow(TRUE);
		m_cBZIP2Mode.EnableWindow(TRUE);
		m_cResetCompression.EnableWindow(TRUE);
	}
	else
	{		
		m_cCompressionLevel.EnableWindow(FALSE);
		m_cBZIP2Mode.EnableWindow(FALSE);
		m_cResetCompression.EnableWindow(FALSE);
	}
}

void CConfigGeneralDlg::OnBnClickedButtonIISxpressServerStart()
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	if (g_VerInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT)
	{
		SC_HANDLE hSMStatus = pSheet->GetServiceManager();
		if (hSMStatus != NULL)
		{
			SC_HANDLE hIISxpress = ::OpenService(hSMStatus, IISXPRESS_SERVICENAME, SERVICE_START);
			if (hIISxpress != NULL)
			{
				::StartService(hIISxpress, 0, NULL);			
				::CloseServiceHandle(hIISxpress);

				UpdateIISxpressStatus();
			}
		}
		else
		{
			CString sCaption;
			sCaption.LoadString(IDS_IISXPRESS_APPNAME);

			CString sMsg;
			sMsg.LoadString(IDS_ERROR_CANTCONNECTSERVICEMANAGER_START);

			MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION | MB_OK);
		}

		// don't need this
		//::CloseServiceHandle(hSMStatus);
	}
}

void CConfigGeneralDlg::OnBnClickedButtonIISxpressServerStop()
{
	CString sCaption;
	sCaption.LoadString(IDS_IISXPRESSSTATUS);

	CString sMsg;
	sMsg.LoadString(IDS_STRING_REALLYSTOPIISXPRESS);

	if (MessageBox(sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;	

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	if (g_VerInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT)
	{
		SC_HANDLE hSMStatus = pSheet->GetServiceManager();
		if (hSMStatus != NULL)
		{
			SC_HANDLE hIISxpress = ::OpenService(hSMStatus, IISXPRESS_SERVICENAME, SERVICE_STOP);
			if (hIISxpress != NULL)
			{
				SERVICE_STATUS Status;				
				::ControlService(hIISxpress, SERVICE_CONTROL_STOP, &Status);
				::CloseServiceHandle(hIISxpress);

				UpdateIISxpressStatus();
			}
		}
		else
		{
			CString sCaption;
			sCaption.LoadString(IDS_IISXPRESS_APPNAME);

			CString sMsg;
			sMsg.LoadString(IDS_ERROR_CANTCONNECTSERVICEMANAGER_STOP);

			MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION | MB_OK);
		}

		// don't need this
		//::CloseServiceHandle(hSMStatus);
	}
}

void CConfigGeneralDlg::OnCbnSelchangeComboCompressionMode()
{
	int nSel = m_cCompressionLevel.GetCurSel();
	if (nSel == CB_ERR)
	{
		return;
	}

	// get the sheet
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
	if (pSheet == NULL)
	{
		return;
	}

	// get the HTTP request object
	CComPtr<IIISxpressHTTPRequest> pRequest;
	if (pSheet->GetHTTPRequest(&pRequest) != S_OK || pRequest == NULL)
	{
		return;
	}

	CComQIPtr<IIISxpressSettings> pSettings = pRequest;
	ATLASSERT(pSettings != NULL);
	if (pSettings == NULL)
	{
		return;
	}

	// get the compression mode the user has selected
	CompressionMode compressionMode = (CompressionMode) m_cCompressionLevel.GetItemData(nSel);

	HRESULT hr = pSettings->put_CompressionMode(compressionMode);
	if (hr != S_OK)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERROR_CANTSETCOMPRESSIOMODE);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);
	}
	else
	{
		// we need to reset the compression cache since the config has changed		
		CIISxpressTrayPropertySheet::ResetResponseCache();
	}
}

void CConfigGeneralDlg::OnCbnSelchangeComboBZIP2Mode()
{
	int nSel = m_cBZIP2Mode.GetCurSel();
	if (nSel == CB_ERR)
	{
		return;
	}

	// get the sheet
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
	if (pSheet == NULL)
	{
		return;
	}

	CComPtr<IComIISxpressRegSettings> pServerRegSettings;
	pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);

	if (pServerRegSettings == NULL)
	{
		return;
	}

	// get the compression mode the user has selected
	BZIP2Mode bzip2Mode = (BZIP2Mode) m_cBZIP2Mode.GetItemData(nSel);

	HRESULT hr = pServerRegSettings->put_BZIP2Mode(bzip2Mode);
	if (hr != S_OK)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERROR_CANTSETBZIP2MODE);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);
	}
	else
	{
		// we need to reset the compression cache since the config has changed		
		CIISxpressTrayPropertySheet::ResetResponseCache();
	}
}

void CConfigGeneralDlg::OnBnClickedResetCompressionStats()
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	if (pSheet->GetCompressionManager(&pCompressionManager) != S_OK)
		return;

	CComQIPtr<ICompressionStats> pStats = pCompressionManager;
	ASSERT(pStats != NULL);

	if (pStats == NULL)
		return;

	// we have reset the stats, we need to tell the general page about it
	if (pStats->ResetStats() == S_OK)
	{
		for (int i = 0; i < pSheet->GetPageCount(); i++)
		{
			CPropertyPage* pPage = pSheet->GetPage(i);
			if (pPage->IsKindOf(RUNTIME_CLASS(CGeneralPropertyPage)) == TRUE)
			{
				pPage->PostMessage(WM_UPDATEACTIVITYCONTROLS);
				break;
			}
		}
	}
}