// AddContentTypeConfirmPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeConfirmPage.h"


// CAddContentTypeConfirmPage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeConfirmPage, CPropertyPage)
CAddContentTypeConfirmPage::CAddContentTypeConfirmPage()
	: CPropertyPage(CAddContentTypeConfirmPage::IDD)
{
}

CAddContentTypeConfirmPage::~CAddContentTypeConfirmPage()
{
}

void CAddContentTypeConfirmPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCONTENTTYPES, m_cContentTypes);
}


BEGIN_MESSAGE_MAP(CAddContentTypeConfirmPage, CPropertyPage)
END_MESSAGE_MAP()


// CAddContentTypeConfirmPage message handlers

BOOL CAddContentTypeConfirmPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();	

	m_cContentTypes.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_CONTENTTYPE);
	m_cContentTypes.InsertColumn(0, sColumn, LVCFMT_LEFT, 430);	

	return TRUE;
}

BOOL CAddContentTypeConfirmPage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();	

	const CArray<CString, LPCTSTR>& saSelectedContentTypes = pSheet->GetSelectedContentTypes();

	m_cContentTypes.SetRedraw(FALSE);

	m_cContentTypes.DeleteAllItems();

	for (int i = 0; i < saSelectedContentTypes.GetCount(); i++)
	{
		LPCTSTR pszContentType = saSelectedContentTypes[i];		

		int nItem = m_cContentTypes.InsertItem(m_cContentTypes.GetItemCount(), pszContentType);		
	}

	m_cContentTypes.SetRedraw(TRUE);

	// we want help
	pSheet->EnableHelp(TRUE);

	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);	

	return TRUE;
}

LRESULT CAddContentTypeConfirmPage::OnWizardNext(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	const CArray<CString, LPCTSTR>& saSelectedContentTypes = pSheet->GetSelectedContentTypes();

	// keep track of the number of rules added
	int nRulesAdded = 0;

	CComPtr<INeverCompressRules> pNeverRules;
	if (pSheet->GetNeverRules(&pNeverRules) == true && pNeverRules != NULL)
	{				
		for (int i = 0; i < saSelectedContentTypes.GetCount(); i++)
		{
			CStringA sContentType(saSelectedContentTypes[i]);

			HRESULT hr = pNeverRules->AddContentTypeRule(sContentType, "", "", pSheet->GetUpdateCookie());
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