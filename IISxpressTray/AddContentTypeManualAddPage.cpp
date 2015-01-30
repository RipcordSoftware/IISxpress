// AddContentTypeManualAddPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeManualAddPage.h"

BEGIN_MESSAGE_MAP(CAddContentTypeEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

void CAddContentTypeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

UINT CAddContentTypeEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

// CAddContentTypeManualAddPage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeManualAddPage, CPropertyPage)

CAddContentTypeManualAddPage::CAddContentTypeManualAddPage()
	: CPropertyPage(CAddContentTypeManualAddPage::IDD)
{
}

CAddContentTypeManualAddPage::~CAddContentTypeManualAddPage()
{
}

void CAddContentTypeManualAddPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTMANUALADDCONTENTTYPES, m_cContentTypes);
	DDX_Control(pDX, IDC_BUTTONMANUALADD_REMOVE, m_cRemove);
	DDX_Control(pDX, IDC_BUTTONMANUALADD_CLEAR, m_cClear);
	DDX_Control(pDX, IDC_COMBOBOX_CONTENTTYPES, m_cContentTypesCombo);
}


BEGIN_MESSAGE_MAP(CAddContentTypeManualAddPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTONMANUALADD, OnBnClickedButtonManualAdd)
	ON_BN_CLICKED(IDC_BUTTONMANUALADD_REMOVE, OnBnClickedButtonManualAddRemove)
	ON_BN_CLICKED(IDC_BUTTONMANUALADD_CLEAR, OnBnClickedButtonManualAddClear)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTMANUALADDCONTENTTYPES, OnLvnItemChangedListManualAddContentTypes)
END_MESSAGE_MAP()


// CAddContentTypeManualAddPage message handlers

BOOL CAddContentTypeManualAddPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// set the partner of the edit control which intercepts the VK_RETURN
	m_cAddContentTypeEdit.SetPartner(GetDlgItem(IDC_BUTTONMANUALADD)->GetSafeHwnd());

	// subclass the edit control in the combo
	HWND hwndEdit = m_cContentTypesCombo.GetWindow(GW_CHILD)->GetSafeHwnd();
	m_cAddContentTypeEdit.SubclassWindow(hwndEdit);	

	// the content types list should be full row select
	m_cContentTypes.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_CONTENTTYPE);
	m_cContentTypes.InsertColumn(0, sColumn, LVCFMT_LEFT, 340);	

	return TRUE;
}

BOOL CAddContentTypeManualAddPage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	// we only activate when in manual mode
	if (pSheet->GetAddContentTypeWizardMode() != CAddContentTypeWizardSheet::Manual)
		return FALSE;

	// we want help
	pSheet->EnableHelp(TRUE);

	// if there are no items in the control yet then we need to get them
	if (m_cContentTypesCombo.GetCount() == 0)
	{
		CWaitCursor Cursor;		

		CRegKey reg;
		if (reg.Open(HKEY_CLASSES_ROOT, _T(""), KEY_READ) == ERROR_SUCCESS)
		{
			// keep track of the content types we have found
			CMap<CString, LPCTSTR, bool, bool> KnownContentTypes;

			TCHAR szName[265];
			DWORD dwNameLength = 0;

			int nRegItem = 0;
			while ((dwNameLength = sizeof(szName) / sizeof(szName[0])) > 0 &&
				reg.EnumKey(nRegItem, szName, &dwNameLength) == ERROR_SUCCESS)
			{
				if (szName[0] == '.')
				{
					TCHAR szContentType[256];
					DWORD dwContentTypeLength = sizeof(szContentType) / sizeof(szContentType[0]);

					CRegKey ContentTypeReg;
					bool bTemp;
					if (ContentTypeReg.Open(reg, szName, KEY_READ) == ERROR_SUCCESS &&
						ContentTypeReg.QueryStringValue(_T("Content Type"), szContentType, &dwContentTypeLength) == ERROR_SUCCESS)
					{
						// force to lower case
						_tcslwr_s(szContentType, _countof(szContentType));

						if (KnownContentTypes.Lookup(szContentType, bTemp) == FALSE)
						{
							// add the content type to the sorted combo
							m_cContentTypesCombo.AddString(szContentType);

							// add the content type to the known map
							KnownContentTypes[szContentType] = true;
						}
					}
				}

				nRegItem++;
			}
		}			
	}

	UpdateControls();

	return TRUE;
}

void CAddContentTypeManualAddPage::OnBnClickedButtonManualAdd()
{
	CString sContentType;
	m_cContentTypesCombo.GetWindowText(sContentType);
	
	if (sContentType.GetLength() <= 0)
		return;

	sContentType.TrimLeft(_T("\t "));
	sContentType.TrimRight(_T("\t "));
	sContentType.MakeLower();	
	AddContentType(sContentType);
	
	m_cContentTypesCombo.SetCurSel(-1);
}

bool CAddContentTypeManualAddPage::AddContentType(LPCTSTR pszContentType)
{
	if (pszContentType == NULL)
		return false;

	const TCHAR* pszRegEx = 
		_T("\\a+\\/")						// match the first part of the content type
		_T("\\*|(\\a.*)")					// match * or any text starting with an alphanum
		_T("$");							// we must now find the end of the string

	// this reg exp should allow entry of an IP address of up to 4 octets
	CAtlRegExp<> re;
	re.Parse(pszRegEx);		

	CAtlREMatchContext<> mc;	
	const TCHAR* pszEnd = NULL;

	if (re.Match(pszContentType, &mc, &pszEnd) == FALSE)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERROR_BADCONTEXTTYPEFORMAT);		

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);

		return false;
	}

	bool bTemp = true;
	if (m_PendingContentTypes.Lookup(pszContentType, bTemp) == TRUE && bTemp == true)
		return false;	

	int nItem = m_cContentTypes.InsertItem(m_cContentTypes.GetItemCount(), pszContentType);	

	m_cContentTypes.EnsureVisible(nItem, FALSE);

	// add the content type to the map to prevent the user adding it twice
	m_PendingContentTypes[pszContentType] = true;

	UpdateControls();

	return true;
}

void CAddContentTypeManualAddPage::OnBnClickedButtonManualAddRemove()
{
	POSITION pos = m_cContentTypes.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	CDWordArray dwaItems;

	while (pos != NULL)
	{
		int nItem = m_cContentTypes.GetNextSelectedItem(pos);

		dwaItems.Add(nItem);
	}

	m_cContentTypes.SetRedraw(FALSE);

	for (int i = (int) dwaItems.GetCount() - 1; i >= 0; i--)
	{
		int nItem = dwaItems[i];

		CString sContentType = m_cContentTypes.GetItemText(nItem, 0);

		m_PendingContentTypes[sContentType] = false;

		m_cContentTypes.DeleteItem(nItem);
	}

	m_cContentTypes.SetRedraw(TRUE);

	UpdateControls();
}

void CAddContentTypeManualAddPage::OnBnClickedButtonManualAddClear()
{
	m_PendingContentTypes.RemoveAll();

	m_cContentTypes.DeleteAllItems();

	UpdateControls();
}

void CAddContentTypeManualAddPage::OnLvnItemChangedListManualAddContentTypes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateControls();
}

void CAddContentTypeManualAddPage::UpdateControls(void)
{
	int nItems = m_cContentTypes.GetItemCount();
	int nSelected = m_cContentTypes.GetSelectedCount();

	if (nItems > 0)
		m_cClear.EnableWindow(TRUE);
	else
		m_cClear.EnableWindow(FALSE);

	if (nSelected > 0)
		m_cRemove.EnableWindow(TRUE);
	else
		m_cRemove.EnableWindow(FALSE);

	// we always have back, sometimes next
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();
	if (nItems > 0)
		pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	else
		pSheet->SetWizardButtons(PSWIZB_BACK);
}

LRESULT CAddContentTypeManualAddPage::OnWizardNext(void)
{	
	// the user can only advance if there are content types in the control
	int nItems = m_cContentTypes.GetItemCount();
	if (nItems <= 0)
		return -1;	

	// get all the content types the user has entered
	CArray<CString, LPCTSTR> saSelectedContentTypes;
	for (int i = 0; i < nItems; i++)
	{
		CString sCT = m_cContentTypes.GetItemText(i, 0);		
		saSelectedContentTypes.Add(sCT);
	}

	// save the selected content types in the sheet
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();
	pSheet->SetSelectedContentTypes(saSelectedContentTypes);

	// allow the user to proceed to the next page
	return 0;
}