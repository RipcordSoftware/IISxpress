// AddExtnManualAddPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddExtnManualAddPage.h"
#include ".\addextnmanualaddpage.h"

BEGIN_MESSAGE_MAP(CAddExtnEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

void CAddExtnEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		CString sText;
		GetWindowText(sText);

		if (sText.GetLength() > 0 && m_hwndPartner != NULL)
		{			
			::SendMessage(m_hwndPartner, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(8, 8));
			::SendMessage(m_hwndPartner, WM_LBUTTONUP, 0, MAKELONG(8, 8));

			// make sure the edit control has the focus
			SetFocus();
		}
	}
	else
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

UINT CAddExtnEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

// CAddExtnManualAddPage dialog

IMPLEMENT_DYNAMIC(CAddExtnManualAddPage, CPropertyPage)

CAddExtnManualAddPage::CAddExtnManualAddPage()
	: CPropertyPage(CAddExtnManualAddPage::IDD)	
{
}

CAddExtnManualAddPage::~CAddExtnManualAddPage()
{
}

void CAddExtnManualAddPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_LISTMANUALADDEXTENSIONS, m_cExtensions);
	DDX_Control(pDX, IDC_BUTTONMANUALADD_REMOVE, m_cRemove);
	DDX_Control(pDX, IDC_BUTTONMANUALADD_CLEAR, m_cClear);
	DDX_Control(pDX, IDC_COMBOBOXEX_EXTENSIONS, m_cExtensionsCombo);
}


BEGIN_MESSAGE_MAP(CAddExtnManualAddPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTONMANUALADD, OnBnClickedButtonManualAdd)
	ON_BN_CLICKED(IDC_BUTTONMANUALADD_REMOVE, OnBnClickedButtonManualAddRemove)
	ON_BN_CLICKED(IDC_BUTTONMANUALADD_CLEAR, OnBnClickedButtonManualAddClear)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTMANUALADDEXTENSIONS, OnLvnItemChangedListManualAddExtensions)
END_MESSAGE_MAP()


// CAddExtnManualAddPage message handlers

BOOL CAddExtnManualAddPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// set the partner of the edit control which intercepts the VK_RETURN
	m_cAddExtnEdit.SetPartner(GetDlgItem(IDC_BUTTONMANUALADD)->GetSafeHwnd());

	// subclass the edit control in the combo
	HWND hwndEdit = m_cExtensionsCombo.GetEditCtrl()->GetSafeHwnd();
	m_cAddExtnEdit.SubclassWindow(hwndEdit);	

	// the extensions list should be full row select
	m_cExtensions.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_EXTENSION);
	m_cExtensions.InsertColumn(0, sColumn, LVCFMT_LEFT, 100);

	sColumn.LoadString(IDS_COLUMN_EXTNDESC);
	m_cExtensions.InsertColumn(1, sColumn, LVCFMT_LEFT, 220);	

	/*******************************************/	

	m_cExtensions.SetImageList(g_ShellImageList, LVSIL_SMALL);

	/*******************************************/	

	m_cExtensionsCombo.SetImageList(g_ShellImageList);

	return TRUE;
}

BOOL CAddExtnManualAddPage::OnSetActive(void)
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	// we only activate when in manual mode
	if (pSheet->GetAddExtnWizardMode() != CAddExtnWizardSheet::Manual)
		return FALSE;

	// we want the help button
	pSheet->EnableHelp(TRUE);

	// if there are no items in the control yet then we need to get them
	if (m_cExtensionsCombo.GetCount() == 0)
	{
		CWaitCursor Cursor;

		CRegKey reg;
		if (reg.Open(HKEY_CLASSES_ROOT, _T(""), KEY_READ) == ERROR_SUCCESS)
		{
			TCHAR szName[265];
			DWORD dwNameLength = 0;

			int nComboItem = 0;
			int nRegItem = 0;
			while ((dwNameLength = sizeof(szName) / sizeof(szName[0])) > 0 &&
				reg.EnumKey(nRegItem, szName, &dwNameLength) == ERROR_SUCCESS)
			{
				if (szName[0] == '.')
				{
					TCHAR szContentType[256];
					DWORD dwContentTypeLength = sizeof(szContentType) / sizeof(szContentType[0]);

					CRegKey ExtnReg;
					if (ExtnReg.Open(reg, szName, KEY_READ) == ERROR_SUCCESS &&
						ExtnReg.QueryStringValue(_T("Content Type"), szContentType, &dwContentTypeLength) == ERROR_SUCCESS)
					{
						int nImageOffset = g_ShellImageList.GetExtensionOffset(szName);					

						COMBOBOXEXITEM ComboBoxItem;
						ComboBoxItem.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;					
						ComboBoxItem.iItem = nComboItem++;
						ComboBoxItem.pszText = &szName[1];
						ComboBoxItem.iImage = nImageOffset;					
						ComboBoxItem.iSelectedImage = nImageOffset;

						m_cExtensionsCombo.InsertItem(&ComboBoxItem);
					}
				}

				nRegItem++;
			}
		}	
	}

	UpdateControls();

	return TRUE;
}

void CAddExtnManualAddPage::OnBnClickedButtonManualAdd()
{
	CString sExtension;
	m_cExtensionsCombo.GetWindowText(sExtension);
	
	if (sExtension.GetLength() <= 0)
		return;

	sExtension.TrimLeft(_T("\t "));
	sExtension.TrimRight(_T("\t "));
	sExtension.MakeLower();	
	AddExtension(sExtension);
	
	m_cExtensionsCombo.SetCurSel(-1);
}

void CAddExtnManualAddPage::AddExtension(LPCTSTR pszExtension)
{
	if (pszExtension == NULL)
		return;

	if (pszExtension[0] == '.')
		pszExtension++;	

	bool bTemp = true;
	if (m_PendingExtensions.Lookup(pszExtension, bTemp) == TRUE && bTemp == true)
		return;

	int nImage = g_ShellImageList.GetExtensionOffset(pszExtension);	

	int nItem = m_cExtensions.InsertItem(m_cExtensions.GetItemCount(), pszExtension, nImage);

	// set the extension desc
	CString sDesc;
	g_ShellImageList.GetExtensionDescription(pszExtension, sDesc);
	m_cExtensions.SetItemText(nItem, 1, sDesc);	

	m_cExtensions.EnsureVisible(nItem, FALSE);

	// add the extension to the map to prevent the user adding it twice
	m_PendingExtensions[pszExtension] = true;

	UpdateControls();
}

void CAddExtnManualAddPage::OnBnClickedButtonManualAddRemove()
{
	POSITION pos = m_cExtensions.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	CDWordArray dwaItems;

	while (pos != NULL)
	{
		int nItem = m_cExtensions.GetNextSelectedItem(pos);

		dwaItems.Add(nItem);
	}

	m_cExtensions.SetRedraw(FALSE);

	for (int i = (int) dwaItems.GetCount() - 1; i >= 0; i--)
	{
		int nItem = dwaItems[i];

		CString sExtension = m_cExtensions.GetItemText(nItem, 0);

		m_PendingExtensions[sExtension] = false;

		m_cExtensions.DeleteItem(nItem);
	}

	m_cExtensions.SetRedraw(TRUE);

	UpdateControls();
}

void CAddExtnManualAddPage::OnBnClickedButtonManualAddClear()
{
	m_PendingExtensions.RemoveAll();

	m_cExtensions.DeleteAllItems();

	UpdateControls();
}

void CAddExtnManualAddPage::OnLvnItemChangedListManualAddExtensions(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateControls();
}

void CAddExtnManualAddPage::UpdateControls(void)
{
	int nItems = m_cExtensions.GetItemCount();
	int nSelected = m_cExtensions.GetSelectedCount();

	if (nItems > 0)
		m_cClear.EnableWindow(TRUE);
	else
		m_cClear.EnableWindow(FALSE);

	if (nSelected > 0)
		m_cRemove.EnableWindow(TRUE);
	else
		m_cRemove.EnableWindow(FALSE);

	// we always have back, sometimes next
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();
	if (nItems > 0)
		pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	else
		pSheet->SetWizardButtons(PSWIZB_BACK);
}

LRESULT CAddExtnManualAddPage::OnWizardNext(void)
{	
	// the user can only advance if there are extensions in the control
	int nItems = m_cExtensions.GetItemCount();
	if (nItems <= 0)
		return -1;	

	// get all the extensions the user has entered
	CArray<CString, LPCTSTR> saSelectedExtns;
	for (int i = 0; i < nItems; i++)
	{
		CString sExtn = m_cExtensions.GetItemText(i, 0);		
		saSelectedExtns.Add(sExtn);
	}

	// save the selected extensions in the sheet
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();
	pSheet->SetSelectedExtensions(saSelectedExtns);

	// allow the user to proceed to the next page
	return 0;
}