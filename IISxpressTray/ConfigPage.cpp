// ServicesPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ConfigPage.h"

#include "resource.h"

#define CONFIGITEM_GENERAL		1
#define CONFIGITEM_CACHE		2
#define CONFIGITEM_ADVANCED		3
#define CONFIGITEM_AHA			4

// CConfigPage dialog

IMPLEMENT_DYNAMIC(CConfigPage, CIISxpressPageBase)

CConfigPage::CConfigPage()
	: CIISxpressPageBase(CConfigPage::IDD)
{
	m_pGeneralDlg = NULL;
	m_pCacheDlg = NULL;
	m_pAdvancedDlg = NULL;
	m_pAHADlg = NULL;
}

CConfigPage::~CConfigPage()
{
	if (m_pGeneralDlg != NULL)
		delete m_pGeneralDlg;

	if (m_pCacheDlg != NULL)
		delete m_pCacheDlg;

	if (m_pAdvancedDlg != NULL)
		delete m_pAdvancedDlg;

	if (m_pAHADlg != NULL)
		delete m_pAHADlg;
}

void CConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONFIGTYPES, m_cConfigTypes);
	DDX_Control(pDX, IDC_CHILDFRAME, m_cChildFrame);
}


BEGIN_MESSAGE_MAP(CConfigPage, CIISxpressPageBase)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONFIGTYPES, OnLvnItemChangedListConfigTypes)
END_MESSAGE_MAP()


// CConfigPage message handlers

BOOL CConfigPage::OnInitDialog(void)
{
	BOOL bStatus = CIISxpressPageBase::OnInitDialog();

	// ***********************************************************

	CRect rcChildFrame;
	m_cChildFrame.GetWindowRect(rcChildFrame);
	ScreenToClient(rcChildFrame);

	m_pGeneralDlg = new CConfigGeneralDlg();
	m_pGeneralDlg->Create(CConfigGeneralDlg::IDD, this);
	m_pGeneralDlg->MoveWindow(rcChildFrame);	
	m_pGeneralDlg->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pGeneralDlg->ShowWindow(SW_HIDE);

	m_pCacheDlg = new CConfigCacheDlg();
	m_pCacheDlg->Create(CConfigCacheDlg::IDD, this);
	m_pCacheDlg->MoveWindow(rcChildFrame);	
	m_pCacheDlg->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pCacheDlg->ShowWindow(SW_HIDE);

	m_pAdvancedDlg = new CConfigAdvancedDlg();
	m_pAdvancedDlg->Create(CConfigAdvancedDlg::IDD, this);
	m_pAdvancedDlg->MoveWindow(rcChildFrame);	
	m_pAdvancedDlg->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pAdvancedDlg->ShowWindow(SW_HIDE);

	// ***********************************************************

	m_Icons.Create(32, 32, ILC_COLOR24 | ILC_MASK, 1, 8);	

	m_GeneralBmp.LoadBitmap(IDB_BITMAP_CONFIGGENERAL);
	m_Icons.Add(&m_GeneralBmp, RGB(255, 0, 255));

	m_CacheBmp.LoadBitmap(IDB_BITMAP_CONFIGCACHE);
	m_Icons.Add(&m_CacheBmp, RGB(255, 0, 255));

	m_AdvancedBmp.LoadBitmap(IDB_BITMAP_CONFIGADVANCED);
	m_Icons.Add(&m_AdvancedBmp, RGB(255, 0, 255));

	m_HardwareCompressionBmp.LoadBitmap(IDB_BITMAP_HWCOMPRESSION);
	m_Icons.Add(&m_HardwareCompressionBmp, RGB(255, 0, 255));

	m_cConfigTypes.SetImageList(&m_Icons, LVSIL_NORMAL);

	// override the default icon spacing
	//m_cExclusionTypes.SetIconSpacing(80, 42);

	CString sGeneral;
	sGeneral.LoadString(IDS_CONFIGTYPE_GENERAL);
	int nExtensionItem = m_cConfigTypes.InsertItem(0, sGeneral, 0);	
	m_cConfigTypes.SetItemData(nExtensionItem, CONFIGITEM_GENERAL);	

	CString sCache;
	sCache.LoadString(IDS_CONFIGTYPE_CACHE);
	int nContentTypeItem = m_cConfigTypes.InsertItem(1, sCache, 1);
	m_cConfigTypes.SetItemData(nContentTypeItem, CONFIGITEM_CACHE);

	CString sAdvanced;
	sAdvanced.LoadString(IDS_CONFIGTYPE_ADVANCED);
	int nAdvancedItem = m_cConfigTypes.InsertItem(2, sAdvanced, 2);
	m_cConfigTypes.SetItemData(nAdvancedItem, CONFIGITEM_ADVANCED);

	if (IsAHALicenseMode())
	{
		m_pAHADlg = new CConfigAHADlg();
		m_pAHADlg->Create(CConfigAHADlg::IDD, this);
		m_pAHADlg->MoveWindow(rcChildFrame);	
		m_pAHADlg->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		m_pAHADlg->ShowWindow(SW_HIDE);

		CString sHWComp;
		sHWComp.LoadString(IDS_CONFIGTYPE_AHA);
		int nHWCompItem = m_cConfigTypes.InsertItem(3, sHWComp, 3);
		m_cConfigTypes.SetItemData(nHWCompItem, CONFIGITEM_AHA);
	}
	
	m_cConfigTypes.SetItemState(nExtensionItem, LVIS_SELECTED, LVIS_SELECTED);

	// make sure the background colour is white
	m_cConfigTypes.SetBkColor(RGB(255, 255, 255));

	return bStatus;
}

void CConfigPage::ServerOnline(void)
{
	UpdateControls();

	if (m_pAHADlg != NULL)
	{
		m_pAHADlg->ServerOnline();
	}
}

void CConfigPage::ServerOffline(void)
{
	UpdateControls();

	if (m_pAHADlg != NULL)
	{
		m_pAHADlg->ServerOffline();
	}
}

void CConfigPage::Heartbeat(void)
{		
	if (m_pAHADlg != NULL)
	{
		m_pAHADlg->Heartbeat();
	}
}

void CConfigPage::UpdateControls(void)
{
	if (m_pGeneralDlg != NULL)
	{
		m_pGeneralDlg->UpdateControls();
	}
	
	if (m_pCacheDlg != NULL)
	{
		m_pCacheDlg->UpdateControls();
	}

	if (m_pAdvancedDlg != NULL)
	{
		m_pAdvancedDlg->UpdateControls();
	}
}

void CConfigPage::OnLvnItemChangedListConfigTypes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ((pNMLV->uChanged & LVIF_STATE) != 0)
	{
		POSITION pos = m_cConfigTypes.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nSel = m_cConfigTypes.GetNextSelectedItem(pos);
			DWORD dwExclusionType = (DWORD) m_cConfigTypes.GetItemData(nSel);

			if (dwExclusionType == CONFIGITEM_GENERAL)
			{
				m_pGeneralDlg->ShowWindow(SW_SHOW);
				m_pCacheDlg->ShowWindow(SW_HIDE);
				m_pAdvancedDlg->ShowWindow(SW_HIDE);

				if (m_pAHADlg != NULL)
				{
					m_pAHADlg->ShowWindow(SW_HIDE);
				}
			}			
			else if (dwExclusionType == CONFIGITEM_CACHE)
			{
				m_pGeneralDlg->ShowWindow(SW_HIDE);	
				m_pCacheDlg->ShowWindow(SW_SHOW);
				m_pAdvancedDlg->ShowWindow(SW_HIDE);

				if (m_pAHADlg != NULL)
				{
					m_pAHADlg->ShowWindow(SW_HIDE);
				}
			}			
			else if (dwExclusionType == CONFIGITEM_ADVANCED)
			{
				m_pGeneralDlg->ShowWindow(SW_HIDE);	
				m_pCacheDlg->ShowWindow(SW_HIDE);
				m_pAdvancedDlg->ShowWindow(SW_SHOW);

				if (m_pAHADlg != NULL)
				{
					m_pAHADlg->ShowWindow(SW_HIDE);
				}
			}		
			else if (dwExclusionType == CONFIGITEM_AHA)
			{
				m_pGeneralDlg->ShowWindow(SW_HIDE);	
				m_pCacheDlg->ShowWindow(SW_HIDE);
				m_pAdvancedDlg->ShowWindow(SW_HIDE);

				if (m_pAHADlg != NULL)
				{
					m_pAHADlg->ShowWindow(SW_SHOW);
				}
			}	
			else
			{			
				m_pGeneralDlg->ShowWindow(SW_HIDE);	
				m_pCacheDlg->ShowWindow(SW_HIDE);
				m_pAdvancedDlg->ShowWindow(SW_HIDE);

				if (m_pAHADlg != NULL)
				{
					m_pAHADlg->ShowWindow(SW_HIDE);
				}
			}
		}
	}

	*pResult = 0;
}

bool CConfigPage::IsAHALicenseMode()
{
	bool ahaLicenseMode = false;

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<IIISxpressHTTPRequest> pRequest;
	HRESULT hr = pSheet->GetHTTPRequest(&pRequest);
	if (hr == S_OK)
	{
		CComPtr<IComIISxpressRegSettings> pRegSettings;
		hr = pSheet->GetIISxpressServerRegSettings(&pRegSettings);
		if (hr == S_OK)
		{
			LicenseMode licenseMode = LicenseModeNone;
			hr = pRegSettings->get_LicenseMode(&licenseMode);
			if (hr == S_OK && licenseMode == LicenseModeAHA)
			{
				ahaLicenseMode = true;
			}
		}
	}
	else
	{
		CRegKey Config;
		if (Config.Open(HKEY_LOCAL_MACHINE, IISXPRESSTRAY_REGKEY, KEY_READ) == ERROR_SUCCESS)
		{				
			DWORD dwShowAHAConfig = 0;
			if (Config.QueryDWORDValue(IISXPRESSTRAY_SHOWAHACONFIG, dwShowAHAConfig) == ERROR_SUCCESS && dwShowAHAConfig != 0)
			{
				ahaLicenseMode = true;
			}
		}		
	}

	return ahaLicenseMode;
}