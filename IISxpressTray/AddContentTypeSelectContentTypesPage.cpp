// AddContentTypeSelectContentTypes.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeSelectContentTypesPage.h"

#include <atlpath.h>
#include <atltrace.h>

// CAddContentTypeSelectContentTypesPage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeSelectContentTypesPage, CPropertyPage)

CAddContentTypeSelectContentTypesPage::CAddContentTypeSelectContentTypesPage()
	: CPropertyPage(CAddContentTypeSelectContentTypesPage::IDD)
{
}

CAddContentTypeSelectContentTypesPage::~CAddContentTypeSelectContentTypesPage()
{	
}

void CAddContentTypeSelectContentTypesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTSELECTCONTENTTYPES, m_cContentTypes);
}


BEGIN_MESSAGE_MAP(CAddContentTypeSelectContentTypesPage, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTSELECTCONTENTTYPES, OnLvnItemChangedListSelectCTs)
END_MESSAGE_MAP()


// CAddContentTypeSelectContentTypesPage message handlers

BOOL CAddContentTypeSelectContentTypesPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// we want full row select and checkboxes
	m_cContentTypes.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);		
	
	// set the column header
	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_CONTENTTYPE);
	m_cContentTypes.InsertColumn(0, sColumn, LVCFMT_LEFT, 420);		

	return TRUE;
}

BOOL CAddContentTypeSelectContentTypesPage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	if (pSheet->GetAddContentTypeWizardMode() != CAddExtnWizardSheet::Auto)
		return FALSE;

	// we want help
	pSheet->EnableHelp(TRUE);

	CWaitCursor Cursor;

	// get the previously selected extensions
	CArray<CString, LPCTSTR> saSelectedContentTypes;
	GetSelectedContentTypes(saSelectedContentTypes);

	// create a fast lookup for previously selected extensions
	CMap<CString, LPCTSTR, bool, bool> PreviouslySelectedContentTypes;
	for (int i = 0; i < saSelectedContentTypes.GetCount(); i++)
	{
		CString sContentType = saSelectedContentTypes[i];
		PreviouslySelectedContentTypes[sContentType] = true;
	}

	// don't allow the control to redraw itself since we are about to
	// do a bunch of updates
	m_cContentTypes.SetRedraw(FALSE);

	// remove all items from the list
	m_cContentTypes.DeleteAllItems();	

	// get the candidates from the sheet
	const CArray<CString, LPCTSTR>& CandidateTypes = pSheet->GetCandidateContentTypes();

	for (int i = 0; i < CandidateTypes.GetCount(); i++)
	{
		LPCTSTR pszCandidateType = CandidateTypes[i];

		int nItem = m_cContentTypes.InsertItem(m_cContentTypes.GetItemCount(), pszCandidateType);

		// if the content type was selected before we re-populated it then
		// re-check the item
		bool bTemp;
		if (PreviouslySelectedContentTypes.Lookup(pszCandidateType, bTemp) == TRUE)
		{
			m_cContentTypes.SetCheck(nItem, TRUE);
		}
	}

	// allow the control to redraw
	m_cContentTypes.SetRedraw(TRUE);

	// we've changed stuff, set the wizard buttons right
	UpdateControls();

	return TRUE;
}

bool CAddContentTypeSelectContentTypesPage::GetSelectedContentTypes(CArray<CString, LPCTSTR>& saContentTypes)
{
	saContentTypes.RemoveAll();

	int nItems = m_cContentTypes.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		if (m_cContentTypes.GetCheck(i) == TRUE)
		{
			CString sContentType = m_cContentTypes.GetItemText(i, 0);
			saContentTypes.Add(sContentType);
		}
	}

	return true;
}

void CAddContentTypeSelectContentTypesPage::OnLvnItemChangedListSelectCTs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateControls();
}

void CAddContentTypeSelectContentTypesPage::UpdateControls()
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	CArray<CString, LPCTSTR> saSelectedContentTypes;
	GetSelectedContentTypes(saSelectedContentTypes);

	if (saSelectedContentTypes.GetCount() == 0)
	{
		pSheet->SetWizardButtons(PSWIZB_BACK);
	}
	else
	{
		pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
}

LRESULT CAddContentTypeSelectContentTypesPage::OnWizardNext(void)
{
	CArray<CString, LPCTSTR> saSelectedContentTypes;
	GetSelectedContentTypes(saSelectedContentTypes);

	if (saSelectedContentTypes.GetCount() <= 0)
		return -1;

	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	pSheet->SetSelectedContentTypes(saSelectedContentTypes);

	return 0;
}