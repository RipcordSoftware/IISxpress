// ExcludeExtensionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ExcludeExtensionsDlg.h"

#include "EnumExclusionRuleHelper.h"
#include "AddExtensionRuleDlg.h"

// CExcludeExtensionsDlg dialog

IMPLEMENT_DYNAMIC(CExcludeExtensionsDlg, CHelpAwareDialogBase)

CExcludeExtensionsDlg::CExcludeExtensionsDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CExcludeExtensionsDlg::IDD, pParent), m_dwUpdateCookie(0)
{
}

CExcludeExtensionsDlg::~CExcludeExtensionsDlg()
{
}

void CExcludeExtensionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EXTENSIONS, m_cExtensions);
	DDX_Control(pDX, IDC_BUTTON_ADDEXTENSIONRULE, m_cAddExtension);
	DDX_Control(pDX, IDC_BUTTON_DELETEEXTENSIONRULE, m_cDeleteExtension);
}


BEGIN_MESSAGE_MAP(CExcludeExtensionsDlg, CHelpAwareDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_ADDEXTENSIONRULE, OnBnClickedButtonAddExtensionRule)
	ON_BN_CLICKED(IDC_BUTTON_DELETEEXTENSIONRULE, OnBnClickedButtonDeleteExtensionRule)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EXTENSIONS, OnLvnItemChangedListExtensions)
END_MESSAGE_MAP()


// CExcludeExtensionsDlg message handlers

BOOL CExcludeExtensionsDlg::OnInitDialog(void)
{
	BOOL bStatus = CDialog::OnInitDialog();

	m_cExtensions.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sExtension;
	sExtension.LoadString(IDS_COLUMN_EXTENSION);
	m_cExtensions.InsertColumn(0, sExtension, LVCFMT_LEFT, 75);

	CString sDescription;
	sDescription.LoadString(IDS_COLUMN_EXTNDESC);
	m_cExtensions.InsertColumn(1, sDescription, LVCFMT_LEFT, 225);

	// use the shell image list
	m_cExtensions.SetImageList(g_ShellImageList, LVSIL_SMALL);
	
	// load the keyboard accelerators
	m_hAccelerator = ::LoadAccelerators(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_ACCELERATOR_EXCLUDEEXTENSIONS));

	UpdateControls();

	return bStatus;
}

BOOL CExcludeExtensionsDlg::PreTranslateMessage(MSG* pMsg)
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

void CExcludeExtensionsDlg::OnBnClickedButtonAddExtensionRule()
{
	CIISxpressTrayPropertySheet* pTraySheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
	if (pTraySheet->GetIISxpressInterfaces() != S_OK)
		return;

	CBitmap bmpCompass;
	bmpCompass.LoadBitmap(IDB_BITMAP_WIZARDCOMPASS);

	CBitmap bmpCompassSegment;
	bmpCompassSegment.LoadBitmap(IDB_BITMAP_COMPASSSEGMENT64);	

	CAddExtnWizardSheet AddExtnWizardSheet(IDS_EXTNEXWIZARD, this, 0, bmpCompass, NULL, bmpCompassSegment);
	AddExtnWizardSheet.m_psh.dwFlags |= PSH_WIZARD97;	
	AddExtnWizardSheet.Create(this);

	CAddExtnWelcomePage WelcomePage;
	WelcomePage.m_psp.dwFlags |= PSP_HIDEHEADER | PSP_USETITLE;
	WelcomePage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_EXTNEXWIZARD);
	AddExtnWizardSheet.AddPage(&WelcomePage);

	CAddExtnManualAddPage ManualAddPage;
	ManualAddPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	ManualAddPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_EXTNEXWIZARD);
	ManualAddPage.m_psp.dwFlags |= PSP_USEHEADERTITLE;
	ManualAddPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_MANUALEXTNADD);
	ManualAddPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_MANUALEXTNADD);
	AddExtnWizardSheet.AddPage(&ManualAddPage);

	CAddExtnAutoAddSelectWebSitesPage AutoAddSelectWebSitesPage;
	AutoAddSelectWebSitesPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	AutoAddSelectWebSitesPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_EXTNEXWIZARD);
	AutoAddSelectWebSitesPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_SELECTWEBSITE_EXTN);	
	AutoAddSelectWebSitesPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_SELECTWEBSITE_EXTN);
	AddExtnWizardSheet.AddPage(&AutoAddSelectWebSitesPage);

	CAddExtnAutoAddSelectExtnsPage AutoAddExtnsPage;
	AutoAddExtnsPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	AutoAddExtnsPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_EXTNEXWIZARD);
	AutoAddExtnsPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_SELECTEXTNS);	
	AutoAddExtnsPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_SELECTEXTNS);
	AddExtnWizardSheet.AddPage(&AutoAddExtnsPage);

	CAddExtnConfirmPage ConfirmPage;
	ConfirmPage.m_psp.dwFlags |= PSP_USETITLE | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	ConfirmPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_EXTNEXWIZARD);
	ConfirmPage.m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZHEADER_CONFIRMEXTNS);	
	ConfirmPage.m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZSUBHEADER_CONFIRMEXTNS);
	AddExtnWizardSheet.AddPage(&ConfirmPage);

	CAddExtnFinishPage AddExtnFinishPage;
	AddExtnFinishPage.m_psp.dwFlags |= PSP_USETITLE | PSP_HIDEHEADER;
	AddExtnFinishPage.m_psp.pszTitle = MAKEINTRESOURCE(IDS_EXTNEXWIZARD);	
	AddExtnWizardSheet.AddPage(&AddExtnFinishPage);

	// get the rule manager from the main sheet
	CComPtr<ICompressionRuleManager> pRuleManager;
	pTraySheet->GetCompressionManager(&pRuleManager);
	
	if (pRuleManager != NULL)
	{
		// we need to convert the rule manager into a never rules interface
		CComQIPtr<INeverCompressRules> pNeverRules = pRuleManager;

		// put the never rules interface into the sheet
		AddExtnWizardSheet.Init(pNeverRules, m_dwUpdateCookie);
	}

	INT_PTR nStatus = AddExtnWizardSheet.DoModal();
	if (nStatus == ID_WIZFINISH)
	{
		UpdateExtensionsListControl();
	}
}

void CExcludeExtensionsDlg::OnBnClickedButtonDeleteExtensionRule()
{
	int nSelectedItems = m_cExtensions.GetSelectedCount();
	if (nSelectedItems <= 0)
		return;

	CString sAppName;
	sAppName.LoadString(IDS_IISXPRESS_APPNAME);

	int nPrompt = -1;
	if (nSelectedItems == 1)
	{
		CString sMsg;
		sMsg.LoadString(IDS_REALLYDELETE_EXTNRULE);

		nPrompt = MessageBox(sMsg, sAppName, MB_YESNO | MB_ICONQUESTION);
	}
	else
	{		
		CString sMsg;
		sMsg.LoadString(IDS_REALLYDELETE_EXTNRULES);

		nPrompt = MessageBox(sMsg, sAppName, MB_YESNO | MB_ICONQUESTION);
	}

	if (nPrompt != IDYES)
		return;

	CDWordArray dwaItems;
	POSITION pos = m_cExtensions.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = m_cExtensions.GetNextSelectedItem(pos);

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

					CString sExtension = m_cExtensions.GetItemText(nItem, 0);

					if (pNeverCompressRules->RemoveExtensionRule(CStringA(sExtension), "", m_dwUpdateCookie) == S_OK)
					{
						m_cExtensions.DeleteItem(nItem);
					}
				}
			}
		}
	}
}

void CExcludeExtensionsDlg::OnLvnItemChangedListExtensions(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_cExtensions.GetSelectedCount() == 0)
	{
		m_cDeleteExtension.EnableWindow(FALSE);
	}
	else
	{
		m_cDeleteExtension.EnableWindow(TRUE);
	}

	*pResult = 0;
}

void CExcludeExtensionsDlg::UpdateControls(DWORD dwUpdateCookie)
{
	if (dwUpdateCookie == 0 || m_dwUpdateCookie != dwUpdateCookie)
	{
		UpdateControls();
	}
}

void CExcludeExtensionsDlg::UpdateControls(void)
{
	if (::IsWindow(this->m_hWnd) == FALSE)
		return;

	UpdateExtensionsListControl();

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();	
	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{		
		m_cExtensions.EnableWindow(TRUE);		
		m_cAddExtension.EnableWindow(TRUE);				

		// get a new update cookie		
		m_dwUpdateCookie = pSheet->GetUpdateCookie();
	}
	else
	{
		m_cExtensions.EnableWindow(FALSE);		

		m_cAddExtension.EnableWindow(FALSE);
		m_cDeleteExtension.EnableWindow(FALSE);		
	}	
}

void CExcludeExtensionsDlg::UpdateExtensionsListControl(void)
{
	m_cExtensions.DeleteAllItems();

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	if (pSheet->GetIISxpressInterfaces() != S_OK)
		return;
	
	CComPtr<ICompressionRuleManager> pCompressionRuleManager;
	pSheet->GetCompressionManager(&pCompressionRuleManager);
	if (pCompressionRuleManager == NULL)
		return;
	
	CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
	if (pNeverCompressRules == NULL)
		return;
	
	CComPtr<IEnumExclusionRule> pEnumExtensions;
	if (pNeverCompressRules->EnumExtensions(&pEnumExtensions) != S_OK)
		return;

	m_cExtensions.SetRedraw(FALSE);
	
	ExclusionRuleInfo Info;					
	ULONG nFetched = 0;

	while (pEnumExtensions->Next(1, &Info, &nFetched) == S_OK)
	{
		CString sExtn(Info.bsRule);

		int nImage = g_ShellImageList.GetExtensionOffset(sExtn);

		int nItem = m_cExtensions.InsertItem(m_cExtensions.GetItemCount(), sExtn, nImage);

		CString sDesc;
		g_ShellImageList.GetExtensionDescription(sExtn, sDesc);
		m_cExtensions.SetItemText(nItem, 1, sDesc);						

		CEnumExclusionRuleHelper::DestroyExclusionRuleInfo(Info);						
	}

	m_cExtensions.SetRedraw(TRUE);
}