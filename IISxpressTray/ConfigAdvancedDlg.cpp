// ServicesPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ConfigAdvancedDlg.h"
#include "RegistryKeys.h"
#include "IISxpressRegSettings.h"
#include "IISxpressInstallerExtensions.h"

#include "RepairingFilterDlg.h"

#include "resource.h"

// CConfigAdvancedDlg dialog

IMPLEMENT_DYNAMIC(CConfigAdvancedDlg, CHelpAwareDialogBase)

CConfigAdvancedDlg::CConfigAdvancedDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CConfigAdvancedDlg::IDD, pParent), m_dwNumberOfCores(1), m_chkCPUs(8)
{
	for (size_t i = 0; i < m_chkCPUs.size(); i++)
		m_chkCPUs[i] = NULL;
}

CConfigAdvancedDlg::~CConfigAdvancedDlg()
{
}

void CConfigAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LOGGINGLEVEL, m_cFilterLoggingLevel);
	DDX_Control(pDX, IDC_COMBO_IISXPRESSSERVERLOGGINGLEVEL, m_cServerLoggingLevel);		
	DDX_Control(pDX, IDC_CHECK_ENABLECPU1, m_cCPU1);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU2, m_cCPU2);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU3, m_cCPU3);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU4, m_cCPU4);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU5, m_cCPU5);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU6, m_cCPU6);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU7, m_cCPU7);
	DDX_Control(pDX, IDC_CHECK_ENABLECPU8, m_cCPU8);
	DDX_Control(pDX, IDC_RADIO_AFFINITYMODE_NONE, m_cAffinityModeNone);
	DDX_Control(pDX, IDC_RADIO_AFFINITYMODE_AUTO, m_cAffinityModeAutomatic);
	DDX_Control(pDX, IDC_RADIO_AFFINITYMODE_MANUAL, m_cAffinityModeManual);
}


BEGIN_MESSAGE_MAP(CConfigAdvancedDlg, CHelpAwareDialogBase)	
	ON_BN_CLICKED(IDC_BUTTON_REPAIR, OnBnClickedButtonRepair)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGGINGLEVEL, OnCbnSelchangeComboFilterLogginglevel)
	ON_CBN_SELCHANGE(IDC_COMBO_IISXPRESSSERVERLOGGINGLEVEL, OnCbnSelchangeComboIISxpressServerLoggingLevel)	
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU1, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU1)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU2, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU2)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU3, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU3)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU4, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU4)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU5, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU5)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU6, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU6)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU7, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU7)
	ON_BN_CLICKED(IDC_CHECK_ENABLECPU8, &CConfigAdvancedDlg::OnBnClickedCheckEnableCPU8)
	ON_BN_CLICKED(IDC_RADIO_AFFINITYMODE_NONE, &CConfigAdvancedDlg::OnBnClickedRadioAffinityModeNone)
	ON_BN_CLICKED(IDC_RADIO_AFFINITYMODE_AUTO, &CConfigAdvancedDlg::OnBnClickedRadioAffinityModeAuto)
	ON_BN_CLICKED(IDC_RADIO_AFFINITYMODE_MANUAL, &CConfigAdvancedDlg::OnBnClickedRadioAffinityModeManual)
END_MESSAGE_MAP()


// CConfigAdvancedDlg message handlers

BOOL CConfigAdvancedDlg::OnInitDialog(void)
{
	BOOL bStatus = CHelpAwareDialogBase::OnInitDialog();

	DWORD dwFilterLoggingLevel = -1;

	CIISxpressRegSettings RegSettings;
	RegSettings.Init(IISXPRESSFILTER_REGKEY, false);
	if (RegSettings.Load() == true)
	{		
		dwFilterLoggingLevel = RegSettings.GetLoggingLevel();
	}

	CString sLoggingLevelNone;
	sLoggingLevelNone.LoadString(IDS_LOGGINGLEVEL_NONE);
	int nItem = m_cFilterLoggingLevel.AddString(sLoggingLevelNone);
	m_cFilterLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_NONE);
	if (dwFilterLoggingLevel == IISXPRESS_LOGGINGLEVEL_NONE)
		m_cFilterLoggingLevel.SetCurSel(nItem);

	CString sLoggingLevelBasic;
	sLoggingLevelBasic.LoadString(IDS_LOGGINGLEVEL_BASIC);
	nItem = m_cFilterLoggingLevel.AddString(sLoggingLevelBasic);
	m_cFilterLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_BASIC);
	if (dwFilterLoggingLevel == IISXPRESS_LOGGINGLEVEL_BASIC)
		m_cFilterLoggingLevel.SetCurSel(nItem);

	CString sLoggingLevelEnhanced;
	sLoggingLevelEnhanced.LoadString(IDS_LOGGINGLEVEL_ENHANCED);
	nItem = m_cFilterLoggingLevel.AddString(sLoggingLevelEnhanced);
	m_cFilterLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_ENH);
	if (dwFilterLoggingLevel == IISXPRESS_LOGGINGLEVEL_ENH)
		m_cFilterLoggingLevel.SetCurSel(nItem);

	CString sLoggingLevelFull;
	sLoggingLevelFull.LoadString(IDS_LOGGINGLEVEL_FULL);
	nItem = m_cFilterLoggingLevel.AddString(sLoggingLevelFull);
	m_cFilterLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_FULL);
	if (dwFilterLoggingLevel == IISXPRESS_LOGGINGLEVEL_FULL)
		m_cFilterLoggingLevel.SetCurSel(nItem);

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<IComIISxpressRegSettings> pServerRegSettings;
	pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);

	DWORD dwServerLoggingLevel = -1;
	if (pServerRegSettings != NULL)
	{
		pServerRegSettings->get_LoggingLevel(&dwServerLoggingLevel);
	}

	nItem = m_cServerLoggingLevel.AddString(sLoggingLevelNone);
	m_cServerLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_NONE);

	nItem = m_cServerLoggingLevel.AddString(sLoggingLevelBasic);
	m_cServerLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_BASIC);

	nItem = m_cServerLoggingLevel.AddString(sLoggingLevelEnhanced);
	m_cServerLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_ENH);

	nItem = m_cServerLoggingLevel.AddString(sLoggingLevelFull);
	m_cServerLoggingLevel.SetItemData(nItem, IISXPRESS_LOGGINGLEVEL_FULL);	

	// allow for easy manipulation of the CPU checkboxes
	m_chkCPUs[0] = &m_cCPU1;
	m_chkCPUs[1] = &m_cCPU2;
	m_chkCPUs[2] = &m_cCPU3;
	m_chkCPUs[3] = &m_cCPU4;
	m_chkCPUs[4] = &m_cCPU5;
	m_chkCPUs[5] = &m_cCPU6;
	m_chkCPUs[6] = &m_cCPU7;
	m_chkCPUs[7] = &m_cCPU8;

	// get info on the system (we are mainly interested in the number of CPU cores)
	SYSTEM_INFO SysInfo;
	::GetSystemInfo(&SysInfo);
	
	// allow for core switching
	m_dwNumberOfCores = SysInfo.dwNumberOfProcessors;
	if (m_dwNumberOfCores > 8)
		m_dwNumberOfCores = 8;

	// as a default for manual mode force all cores to be selected
	for (DWORD i = 0; i < m_dwNumberOfCores; i++)
	{
		m_chkCPUs[i]->SetCheck(BST_CHECKED);
	}

	UpdateControls();

	return bStatus;
}

void CConfigAdvancedDlg::ServerOnline(void)
{
}

void CConfigAdvancedDlg::ServerOffline(void)
{
}

void CConfigAdvancedDlg::Heartbeat(void)
{
	UpdateControls();
}

void CConfigAdvancedDlg::UpdateControls(void)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	m_cServerLoggingLevel.SetCurSel(-1);

	if (pSheet != NULL)
	{		
		CComPtr<IIISxpressHTTPRequest> pRequest;
		HRESULT hr = pSheet->GetHTTPRequest(&pRequest);

		CComQIPtr<IIISxpressSettings> pSettings = pRequest;

		if (hr == S_OK && m_dwNumberOfCores > 1 && pSettings != NULL)
		{			
			// make sure the controls are enabled
			m_cAffinityModeAutomatic.EnableWindow(TRUE);
			m_cAffinityModeManual.EnableWindow(TRUE);
			m_cAffinityModeNone.EnableWindow(TRUE);			

			// setup the processor affinity controls
			DWORD dwProcessorAffinity = IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO;
			pSettings->get_ProcessorAffinity(&dwProcessorAffinity);
			if (dwProcessorAffinity == IISXPRESS_SERVER_PROCESSORAFFINITY_OFF)
			{
				m_cAffinityModeNone.SetCheck(BST_CHECKED);
			}
			else if (dwProcessorAffinity == IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO)
			{
				m_cAffinityModeAutomatic.SetCheck(BST_CHECKED);
			}
			else
			{		
				m_cAffinityModeManual.SetCheck(BST_CHECKED);

				DWORD dwCPUMask = 1;				
				for (size_t i = 0; i < m_chkCPUs.size(); i++)
				{
					if ((dwProcessorAffinity & dwCPUMask) != 0)
						m_chkCPUs[i]->SetCheck(BST_CHECKED);						
					else
						m_chkCPUs[i]->SetCheck(BST_UNCHECKED);
						
					dwCPUMask <<= 1;
				}			
			}

			UpdateAffinityControls();
		}
		else
		{
			// make sure the CPU mode controls are disabled
			m_cAffinityModeAutomatic.EnableWindow(FALSE);
			m_cAffinityModeManual.EnableWindow(FALSE);
			m_cAffinityModeNone.EnableWindow(FALSE);

			// make sure the CPU selection controls are disabled
			for (size_t i = 0; i < m_chkCPUs.size(); i++)
			{
				m_chkCPUs[i]->EnableWindow(FALSE);
			}			
		}

		CComPtr<IComIISxpressRegSettings> pServerRegSettings;
		hr = pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);		

		DWORD dwServerLoggingLevel = -1;
		if (hr == S_OK && pServerRegSettings != NULL)
		{
			pServerRegSettings->get_LoggingLevel(&dwServerLoggingLevel);
		}

		int nItems = m_cServerLoggingLevel.GetCount();
		for (int i = 0; i < nItems; i++)
		{
			DWORD dwData = (DWORD) m_cServerLoggingLevel.GetItemData(i);
			if (dwServerLoggingLevel == dwData)
			{
				m_cServerLoggingLevel.SetCurSel(i);
				break;
			}
		}						
	}	

	// enable/disable the logging button
	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{
		m_cServerLoggingLevel.EnableWindow(TRUE);		
	}
	else
	{
		m_cServerLoggingLevel.EnableWindow(FALSE);		
	}	
}

void CConfigAdvancedDlg::OnBnClickedButtonRepair()
{
	CString sCaption;
	sCaption.LoadString(IDS_IISXPRESS_APPNAME);

	CString sMsg;
	sMsg.LoadString(IDS_MSG_ABOUTTOREPAIR);

	if (MessageBox(sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	CWaitCursor Cursor;

	CPathT<CString> Path;

	// try reading the path from the registry first
	CRegKey Reg;
	if (Reg.Open(HKEY_LOCAL_MACHINE, IISXPRESSFILTER_REGKEY, KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szFilterPath[512] = _T("\0");
		DWORD dwFilterPathSize = sizeof(szFilterPath) / sizeof(szFilterPath[0]);
		Reg.QueryStringValue(IISXPRESS_INSTALLPATH, szFilterPath, &dwFilterPathSize);

		// the registry entry may exist, but it may also be empty
		if (_tcslen(szFilterPath) > 0)
		{
			Path = szFilterPath;

			if (g_VerInfo.dwMajorVersion < 6)
				Path.Append(_T("IISxpress.dll"));
			else
				Path.Append(_T("IISxpressNativeModule.dll"));
		}
	}

	// if the file doesn't exist then use the path to this module as the guess
	if (Path.FileExists() == FALSE)
	{
		TCHAR szFilterPath[512] = _T("\0");
		DWORD dwFilterPathSize = sizeof(szFilterPath) / sizeof(szFilterPath[0]);
		::GetModuleFileName(NULL, szFilterPath, dwFilterPathSize);

		Path = szFilterPath;
		Path.RemoveFileSpec();

		if (g_VerInfo.dwMajorVersion < 6)
				Path.Append(_T("IISxpress.dll"));
			else
				Path.Append(_T("IISxpressNativeModule.dll"));
	}

	if (Path.FileExists() == FALSE)
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_CANTLOCATEFILTERDLL);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);

		return;
	}	

	CRepairingFilterDlg dlg;
	dlg.Init(CStringA(Path));
	INT_PTR nStatus = dlg.DoModal();
	if (nStatus == IDOK)
	{
		sMsg.LoadString(IDS_MSG_FILTERREPAIROK);
		MessageBox(sMsg, sCaption, MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		sMsg.LoadString(IDS_ERROR_FILTERREPAIRFAILED);
		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);
	}
}

void CConfigAdvancedDlg::OnCbnSelchangeComboFilterLogginglevel()
{
	int nSel = m_cFilterLoggingLevel.GetCurSel();
	if (nSel == CB_ERR)
		return;

	DWORD dwLoggingLevel = (DWORD) m_cFilterLoggingLevel.GetItemData(nSel);

	CIISxpressRegSettings RegSettings;
	RegSettings.Init(IISXPRESSFILTER_REGKEY, false);
	if (RegSettings.Load() == false)
		return;

	// if the logging level is the same don't bother changing it
	if (RegSettings.GetLoggingLevel() == dwLoggingLevel)
		return;
	
	RegSettings.SetLoggingLevel(dwLoggingLevel, true);

	// we no longer need to inform the user to restart IIS since the filter
	// code now handles this condition correctly
	/*CString sCaption;
	sCaption.LoadString(IDS_IISXPRESS_APPNAME);

	CString sMsg;
	sMsg.LoadString(IDS_MSG_RESTARTIISFORNEWLOGSETTING);

	MessageBox(sMsg, sCaption, MB_OK | MB_ICONINFORMATION);*/
}

void CConfigAdvancedDlg::OnCbnSelchangeComboIISxpressServerLoggingLevel()
{
	int nSel = m_cServerLoggingLevel.GetCurSel();
	if (nSel == CB_ERR)
		return;

	DWORD dwLoggingLevel = (DWORD) m_cServerLoggingLevel.GetItemData(nSel);

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<IComIISxpressRegSettings> pServerRegSettings;
	pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);

	if (pServerRegSettings != NULL)
	{
		pServerRegSettings->put_LoggingLevel(dwLoggingLevel);
	}
}

void CConfigAdvancedDlg::UpdateAffinityState()
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<IIISxpressHTTPRequest> pRequest;
		HRESULT hr = pSheet->GetHTTPRequest(&pRequest);

	CComQIPtr<IIISxpressSettings> pSettings = pRequest;	
	if (pSettings == NULL)
		return;

	int nAffinityModeNone = m_cAffinityModeNone.GetCheck();
	if (nAffinityModeNone == BST_CHECKED)
	{
		pSettings->put_ProcessorAffinity(IISXPRESS_SERVER_PROCESSORAFFINITY_OFF);
	}
	else
	{
		int nAffinityModeAuto = m_cAffinityModeAutomatic.GetCheck();
		if (nAffinityModeAuto == BST_CHECKED)
		{
			pSettings->put_ProcessorAffinity(IISXPRESS_SERVER_PROCESSORAFFINITY_AUTO);
		}
		else
		{
			DWORD dwCPUMask = 1;
			DWORD dwProcessorAffinity = 0;
			for (size_t i = 0; i < m_chkCPUs.size(); i++)
			{
				if (m_chkCPUs[i]->GetCheck() == BST_CHECKED)				
					dwProcessorAffinity |= dwCPUMask;
					
				dwCPUMask <<= 1;
			}

			pSettings->put_ProcessorAffinity(dwProcessorAffinity);
		}
	}
}

void CConfigAdvancedDlg::UpdateAffinityControls()
{
	int nAffinityModeNone = m_cAffinityModeNone.GetCheck();
	int nAffinityModeAuto = m_cAffinityModeAutomatic.GetCheck();
	if (nAffinityModeNone == BST_CHECKED || nAffinityModeAuto == BST_CHECKED)
	{		
		for (size_t i = 0; i < m_chkCPUs.size(); i++)
			m_chkCPUs[i]->EnableWindow(FALSE);		
	}
	else
	{
		DWORD dwNumberOfCores = (DWORD) min(m_dwNumberOfCores, m_chkCPUs.size());

		size_t i = 0;
		for (; i < dwNumberOfCores; i++)
			m_chkCPUs[i]->EnableWindow(TRUE);

		for (; i < m_chkCPUs.size(); i++)
			m_chkCPUs[i]->EnableWindow(FALSE);
	}
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU1()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU1.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU2()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU2.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU3()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU3.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU4()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU4.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU5()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU5.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU6()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU6.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU7()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU7.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedCheckEnableCPU8()
{
	if (GetNumberOfSelectedCPUs() > 0)
		UpdateAffinityState();
	else
		m_cCPU8.SetCheck(BST_CHECKED);
}

void CConfigAdvancedDlg::OnBnClickedRadioAffinityModeNone()
{
	UpdateAffinityState();
	UpdateAffinityControls();
}

void CConfigAdvancedDlg::OnBnClickedRadioAffinityModeAuto()
{
	UpdateAffinityState();
	UpdateAffinityControls();
}

void CConfigAdvancedDlg::OnBnClickedRadioAffinityModeManual()
{
	UpdateAffinityState();
	UpdateAffinityControls();
}

int CConfigAdvancedDlg::GetNumberOfSelectedCPUs()
{
	int nSelected = 0;

	for (size_t i = 0; i < m_chkCPUs.size(); i++)
	{
		if (m_chkCPUs[i]->GetCheck() == BST_CHECKED)
			nSelected++;
	}

	return nSelected;
}