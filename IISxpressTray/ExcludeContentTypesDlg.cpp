// ExcludeContentTypesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ExcludeContentTypesDlg.h"

#include "EnumExclusionRuleHelper.h"
#include "AddContentTypeRuleDlg.h"

// CExcludeContentTypesDlg dialog

IMPLEMENT_DYNAMIC(CExcludeContentTypesDlg, CHelpAwareDialogBase)

CExcludeContentTypesDlg::CExcludeContentTypesDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CExcludeContentTypesDlg::IDD, pParent), m_dwUpdateCookie(0)
{
}

CExcludeContentTypesDlg::~CExcludeContentTypesDlg()
{
}

void CExcludeContentTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTENTTYPES, m_cContentTypes);
	DDX_Control(pDX, IDC_BUTTON_DELETECONTENTTYPERULE, m_cDeleteContentType);
	DDX_Control(pDX, IDC_BUTTON_ADDCONTENTTYPERULE, m_cAddContentType);
}


BEGIN_MESSAGE_MAP(CExcludeContentTypesDlg, CHelpAwareDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_ADDCONTENTTYPERULE, OnBnClickedButtonAddContentTypeRule)
	ON_BN_CLICKED(IDC_BUTTON_DELETECONTENTTYPERULE, OnBnClickedButtonDeleteContentTypeRule)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTENTTYPES, OnLvnItemChangedListContentTypes)
END_MESSAGE_MAP()


// CExcludeContentTypesDlg message handlers

BOOL CExcludeContentTypesDlg::OnInitDialog(void)
{
	BOOL bStatus = CHelpAwareDialogBase::OnInitDialog();
	
	m_cContentTypes.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sContentType;
	sContentType.LoadString(IDS_COLUMN_CONTENTTYPE);
	m_cContentTypes.InsertColumn(0, sContentType, LVCFMT_LEFT, 300);

	// load the keyboard accelerators
	m_hAccelerator = ::LoadAccelerators(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_ACCELERATOR_EXCLUDECONTEXTTYPES));

	UpdateControls();

	return bStatus;
}

BOOL CExcludeContentTypesDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::TranslateAccelerator(GetSafeHwnd(), m_hAccelerator, pMsg) != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CExcludeContentTypesDlg::UpdateControls(DWORD dwUpdateCookie)
{
	if (dwUpdateCookie == 0 || m_dwUpdateCookie != dwUpdateCookie)
	{
		UpdateControls();
	}
}

void CExcludeContentTypesDlg::UpdateControls(void)
{
	if (::IsWindow(this->m_hWnd) == FALSE)
		return;

	UpdateContentTypesListControl();	

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{		
		m_cContentTypes.EnableWindow(TRUE);
		m_cAddContentType.EnableWindow(TRUE);		

		m_dwUpdateCookie = pSheet->GetUpdateCookie();
	}
	else
	{		
		m_cContentTypes.EnableWindow(FALSE);
		m_cAddContentType.EnableWindow(FALSE);
	}
}

void CExcludeContentTypesDlg::OnBnClickedButtonAddContentTypeRule()
{
	CIISxpressTrayPropertySheet* pTraySheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
	if (pTraySheet->GetIISxpressInterfaces() != S_OK)
		return;

#if 0
	CStringArray saExcludedContentTypes;

	int nItems = m_cContentTypes.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		CString sContentType = m_cContentTypes.GetItemText(i, 0);
		saExcludedContentTypes.Add(sContentType);
	}

	CAddContentTypeRuleDlg dlg;
	dlg.Init(saExcludedContentTypes);
	if (dlg.DoModal() == IDOK)
	{
		CComPtr<ICompressionRuleManager> pCompressionRuleManager;
		pSheet->GetCompressionManager(&pCompressionRuleManager);
		if (pCompressionRuleManager != NULL)
		{
			CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
			if (pNeverCompressRules != NULL)
			{
				if (pNeverCompressRules->AddContentTypeRule(dlg.m_sNewContentType, "", "") == S_OK)
				{
					int nItem = m_cContentTypes.InsertItem(m_cContentTypes.GetItemCount(), dlg.m_sNewContentType);
					m_cContentTypes.EnsureVisible(nItem, FALSE);
				}
			}
		}
	}
#endif

	CBitmap bmpCompass;
	bmpCompass.LoadBitmap(IDB_BITMAP_WIZARDCOMPASS);

	CBitmap bmpCompassSegment;
	bmpCompassSegment.LoadBitmap(IDB_BITMAP_COMPASSSEGMENT64);

	CAddContentTypeWizardSheet AddContentTypeWizardSheet(IDS_EXTNEXWIZARD, this, 0, bmpCompass, NULL, bmpCompassSegment);
	AddContentTypeWizardSheet.m_psh.dwFlags |= PSH_WIZARD97;	
	AddContentTypeWizardSheet.Create(this);

	CAddContentTypeWelcomePage WelcomePage;
	WelcomePage.m_psp.dwFlags |= PSP_HIDEHEADER | PSP_USETITLE;
	WelcomePage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);
	AddContentTypeWizardSheet.AddPage(&WelcomePage);

	CAddContentTypeManualAddPage ManualAddPage;
	ManualAddPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	ManualAddPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);
	ManualAddPage.m_psp.dwFlags |= PSP_USEHEADERTITLE;
	ManualAddPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_MANUALCTADD);
	ManualAddPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_MANUALCTADD);
	AddContentTypeWizardSheet.AddPage(&ManualAddPage);

	CAddContentTypeSelectWebSitesPage AutoAddSelectWebSitesPage;
	AutoAddSelectWebSitesPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	AutoAddSelectWebSitesPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);
	AutoAddSelectWebSitesPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_SELECTWEBSITE_CT);	
	AutoAddSelectWebSitesPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_SELECTWEBSITE_CT);
	AddContentTypeWizardSheet.AddPage(&AutoAddSelectWebSitesPage);

	CAddContentTypeAutoAddSearchPage SearchPage;
	SearchPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	SearchPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);
	SearchPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_SEARCH_CT);	
	SearchPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_SEARCH_CT);
	AddContentTypeWizardSheet.AddPage(&SearchPage);

	CAddContentTypeSelectContentTypesPage SelectContentTypesPage;
	SelectContentTypesPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	SelectContentTypesPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);
	SelectContentTypesPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_SELECT_CT);	
	SelectContentTypesPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_SELECT_CT);
	AddContentTypeWizardSheet.AddPage(&SelectContentTypesPage);

	CAddContentTypeConfirmPage ConfirmPage;
	ConfirmPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	ConfirmPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);
	ConfirmPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_CONFIRM_CT);	
	ConfirmPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_CONFIRM_CT);
	AddContentTypeWizardSheet.AddPage(&ConfirmPage);

	CAddContentTypeFinishPage FinishPage;
	FinishPage.m_psp.dwFlags |= PSP_USETITLE | PSP_HIDEHEADER;
	FinishPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_CONTENTTYPEEXWIZARD);	
	AddContentTypeWizardSheet.AddPage(&FinishPage);

	// get the rule manager from the main sheet
	CComPtr<ICompressionRuleManager> pRuleManager;
	pTraySheet->GetCompressionManager(&pRuleManager);
	
	if (pRuleManager != NULL)
	{
		// we need to convert the rule manager into a never rules interface
		CComQIPtr<INeverCompressRules> pNeverRules = pRuleManager;

		// put the never rules interface into the sheet
		AddContentTypeWizardSheet.Init(pNeverRules, m_dwUpdateCookie);
	}	

	INT_PTR nStatus = AddContentTypeWizardSheet.DoModal();
	if (nStatus == ID_WIZFINISH)
	{		
		UpdateContentTypesListControl();
	}

}

void CExcludeContentTypesDlg::OnBnClickedButtonDeleteContentTypeRule()
{
	int nSelectedItems = m_cContentTypes.GetSelectedCount();
	if (nSelectedItems <= 0)
		return;

	CString sAppName;
	sAppName.LoadString(IDS_IISXPRESS_APPNAME);

	int nPrompt = -1;
	if (nSelectedItems == 1)
	{
		CString sMsg;
		sMsg.LoadString(IDS_REALLYDELETE_CONTENTTYPERULE);

		nPrompt = MessageBox(sMsg, sAppName, MB_YESNO | MB_ICONQUESTION);
	}
	else
	{		
		CString sMsg;
		sMsg.LoadString(IDS_REALLYDELETE_CONTENTTYPERULES);

		nPrompt = MessageBox(sMsg, sAppName, MB_YESNO | MB_ICONQUESTION);
	}

	if (nPrompt != IDYES)
		return;

	CDWordArray dwaItems;
	POSITION pos = m_cContentTypes.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = m_cContentTypes.GetNextSelectedItem(pos);

		dwaItems.InsertAt(0, nItem);
	}

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{
		CComPtr<ICompressionRuleManager> pCompressionRuleManager;
		pSheet->GetCompressionManager(&pCompressionRuleManager);
		if (pCompressionRuleManager != NULL)
		{
			CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
			if (pNeverCompressRules != NULL)
			{
				int nItems = (int) dwaItems.GetCount();
				for (int i = 0; i < nItems; i++)
				{
					int nItem = dwaItems[i];

					CString sContentType = m_cContentTypes.GetItemText(nItem, 0);

					if (pNeverCompressRules->RemoveContentTypeRule(CStringA(sContentType), "", m_dwUpdateCookie) == S_OK)
					{
						m_cContentTypes.DeleteItem(nItem);
					}
				}
			}
		}
	}
}


void CExcludeContentTypesDlg::OnLvnItemChangedListContentTypes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_cContentTypes.GetSelectedCount() == 0)
	{
		m_cDeleteContentType.EnableWindow(FALSE);
	}
	else
	{
		m_cDeleteContentType.EnableWindow(TRUE);
	}
	
	*pResult = 0;
}

void CExcludeContentTypesDlg::UpdateContentTypesListControl(void)
{
	m_cContentTypes.DeleteAllItems();

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{
		CComPtr<ICompressionRuleManager> pCompressionRuleManager;
		pSheet->GetCompressionManager(&pCompressionRuleManager);
		if (pCompressionRuleManager != NULL)
		{
			CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
			if (pNeverCompressRules != NULL)
			{				
				CComPtr<IEnumExclusionRule> pEnumContentTypes;
				if (pNeverCompressRules->EnumContentTypes(&pEnumContentTypes) == S_OK)
				{
					ExclusionRuleInfo Info;					
					ULONG nFetched = 0;

					while (pEnumContentTypes->Next(1, &Info, &nFetched) == S_OK)
					{
						int nItem = m_cContentTypes.InsertItem(m_cContentTypes.GetItemCount(), CString(Info.bsRule));

						CEnumExclusionRuleHelper::DestroyExclusionRuleInfo(Info);						
					}
				}				
			}
		}
	}
}