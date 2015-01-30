// AddExtnConfirmPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddExtnConfirmPage.h"


// CAddExtnConfirmPage dialog

IMPLEMENT_DYNAMIC(CAddExtnConfirmPage, CPropertyPage)

CAddExtnConfirmPage::CAddExtnConfirmPage()
	: CPropertyPage(CAddExtnConfirmPage::IDD)
{
}

CAddExtnConfirmPage::~CAddExtnConfirmPage()
{
}

void CAddExtnConfirmPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTEXTENSIONS, m_cExtns);
}


BEGIN_MESSAGE_MAP(CAddExtnConfirmPage, CPropertyPage)
END_MESSAGE_MAP()


// CAddExtnConfirmPage message handlers

BOOL CAddExtnConfirmPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	m_cExtns.SetImageList(g_ShellImageList, LVSIL_SMALL);

	m_cExtns.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_EXTENSION);
	m_cExtns.InsertColumn(0, sColumn, LVCFMT_LEFT, 120);

	sColumn.LoadString(IDS_COLUMN_EXTNDESC);
	m_cExtns.InsertColumn(1, sColumn, LVCFMT_LEFT, 300);	

	return TRUE;
}

BOOL CAddExtnConfirmPage::OnSetActive(void)
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	const CArray<CString, LPCTSTR>& saSelectedExtns = pSheet->GetSelectedExtensions();

	m_cExtns.SetRedraw(FALSE);

	m_cExtns.DeleteAllItems();

	for (int i = 0; i < saSelectedExtns.GetCount(); i++)
	{
		LPCTSTR pszExtn = saSelectedExtns[i];

		int nImage = g_ShellImageList.GetExtensionOffset(pszExtn);

		int nItem = m_cExtns.InsertItem(m_cExtns.GetItemCount(), pszExtn, nImage);

		CString sDesc;
		g_ShellImageList.GetExtensionDescription(pszExtn, sDesc);
		m_cExtns.SetItemText(nItem, 1, sDesc);
	}

	m_cExtns.SetRedraw(TRUE);

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	// we want the help button
	pSheet->EnableHelp(TRUE);

	return TRUE;
}

LRESULT CAddExtnConfirmPage::OnWizardNext(void)
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	const CArray<CString, LPCTSTR>& saSelectedExtns = pSheet->GetSelectedExtensions();

	// keep track of the number of rules added
	int nRulesAdded = 0;

	CComPtr<INeverCompressRules> pNeverRules;
	if (pSheet->GetNeverRules(&pNeverRules) == true && pNeverRules != NULL)
	{				
		for (int i = 0; i < saSelectedExtns.GetCount(); i++)
		{
			CStringA sExtn(saSelectedExtns[i]);

			HRESULT hr = pNeverRules->AddExtensionRule(sExtn, "", "", pSheet->GetUpdateCookie());
			if (hr == S_OK)
				nRulesAdded++;
		}
	}

	if (nRulesAdded > 0)	
	{
		// we need to reset the compression cache since the config has changed		
		CIISxpressTrayPropertySheet::ResetResponseCache();
	}

	return 0;
}