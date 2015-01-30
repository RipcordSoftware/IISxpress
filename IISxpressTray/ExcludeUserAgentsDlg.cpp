// ExcludeUserAgentsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"

#include "ExcludeUserAgentsDlg.h"
#include "AddUserAgentRuleDlg.h"

// CExcludeUserAgentsDlg dialog

struct ExcludedUserAgentData
{
	ExcludedUserAgentData(LPCTSTR pszUserAgent) 
	{ 	
		SetUserAgentFromRaw(pszUserAgent);
	}

	void GetRawAgentString(CString& rawUserAgent)
	{
		rawUserAgent.Empty();

		if (!excluded)
		{
			rawUserAgent = _T("!");
		}

		rawUserAgent += userAgent;
	}

	void SetUserAgentFromRaw(LPCTSTR pszUserAgent)
	{
		if (pszUserAgent != NULL)
		{
			if (pszUserAgent[0] == '!')
			{
				userAgent = pszUserAgent + 1;
				excluded = false; 
			}
			else
			{
				userAgent = pszUserAgent;
				excluded = true; 
			}
		}	
	}

	CString userAgent;
	bool excluded;
};

IMPLEMENT_DYNAMIC(CExcludeUserAgentsDlg, CHelpAwareDialogBase)

CExcludeUserAgentsDlg::CExcludeUserAgentsDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CExcludeUserAgentsDlg::IDD, pParent), m_dwUpdateCookie(0)
{

}

CExcludeUserAgentsDlg::~CExcludeUserAgentsDlg()
{
}

void CExcludeUserAgentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USERAGENTS, m_cExcludedUserAgents);
	DDX_Control(pDX, IDC_BUTTON_ADDUSERAGENTRULE, m_cAddUserAgentRule);
	DDX_Control(pDX, IDC_BUTTON_DELETEUSERAGENTRULE, m_cDeleteUserAgentRule);
	DDX_Control(pDX, IDC_CHECK_ENABLE_USERAGENTRULES, m_cUserAgentExclusionEnabled);
	DDX_Control(pDX, IDC_BUTTON_EDITUSERAGENTRULE, m_cEditUserAgentRule);
}


BEGIN_MESSAGE_MAP(CExcludeUserAgentsDlg, CHelpAwareDialogBase)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_ENABLE_USERAGENTRULES, &CExcludeUserAgentsDlg::OnBnClickedCheckEnableUserAgentRules)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USERAGENTS, &CExcludeUserAgentsDlg::OnLvnItemChangedListUserAgents)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_USERAGENTS, &CExcludeUserAgentsDlg::OnLvnEndLabelEditListUserAgents)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_USERAGENTS, &CExcludeUserAgentsDlg::OnLvnKeyDownListUserAgents)
	ON_BN_CLICKED(IDC_BUTTON_EDITUSERAGENTRULE, &CExcludeUserAgentsDlg::OnBnClickedButtonEditUserAgentRule)
	ON_BN_CLICKED(IDC_BUTTON_DELETEUSERAGENTRULE, &CExcludeUserAgentsDlg::OnBnClickedButtonDeleteUserAgentRule)
	ON_BN_CLICKED(IDC_BUTTON_ADDUSERAGENTRULE, &CExcludeUserAgentsDlg::OnBnClickedButtonAddUserAgentRule)
	ON_NOTIFY(LVN_INSERTITEM, IDC_LIST_USERAGENTS, &CExcludeUserAgentsDlg::OnLvnInsertItemListUserAgents)
END_MESSAGE_MAP()


// CExcludeUserAgentsDlg message handlers

BOOL CExcludeUserAgentsDlg::OnInitDialog()
{
	BOOL status = CHelpAwareDialogBase::OnInitDialog();

	m_cExcludedUserAgents.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_cExcludedUserAgents.RegisterCallback(&CExcludeUserAgentsDlg::OnCustomDrawCallback);

	CString sUAColHeader;
	sUAColHeader.LoadString(IDS_COLHEADER_USERAGENTSTRING);
	m_cExcludedUserAgents.InsertColumn(0, sUAColHeader, LVCFMT_LEFT, 200);	

	CString sHintColHeader;
	sHintColHeader.LoadString(IDS_COLHEADER_HINT);
	m_cExcludedUserAgents.InsertColumn(1, sHintColHeader, LVCFMT_LEFT, 100);	

	if (::LoadPNG(_T("REDFLAG16.png"), m_imgRedFlag) == true &&
		::LoadPNG(_T("GREENFLAG16.png"), m_imgGreenFlag) == true)
	{
		m_ImageList.Create(16, 16, ILC_COLOR32, 2, 2);
						
		m_ImageList.Add(CBitmap::FromHandle(m_imgRedFlag), (CBitmap*) NULL);
		m_ImageList.Add(CBitmap::FromHandle(m_imgGreenFlag), (CBitmap*) NULL);
	
		m_cExcludedUserAgents.SetImageList(&m_ImageList, LVSIL_SMALL);
	}
	
	m_Config.Init(IISXPRESSFILTER_REGKEY, true, false);

	UpdateControls();

	return status;
}

void CExcludeUserAgentsDlg::OnDestroy()
{
	for (int i = 0; i < m_cExcludedUserAgents.GetItemCount(); i++)
	{
		DeleteItemData(i);
	}
}

void CExcludeUserAgentsDlg::UpdateControls(DWORD dwUpdateCookie)
{
	if (dwUpdateCookie == 0 || m_dwUpdateCookie != dwUpdateCookie)
	{
		UpdateControls();
	}
}

void CExcludeUserAgentsDlg::UpdateControls(void)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	SetUserAgentRuleEnabledState();
	UpdateControlEnableStates();
	UpdateUserAgentControl();
}

void CExcludeUserAgentsDlg::OnBnClickedCheckEnableUserAgentRules()
{	
	// update the setting if it has changed
	if (m_cUserAgentExclusionEnabled.GetCheck() == BST_CHECKED && !m_Config.GetUserAgentExclusionEnabled())
	{
		m_Config.SetUserAgentExclusionEnabled(true);
	}
	else if (m_Config.GetUserAgentExclusionEnabled())
	{
		m_Config.SetUserAgentExclusionEnabled(false);
	}

	UpdateControlEnableStates();
}

void CExcludeUserAgentsDlg::OnLvnItemChangedListUserAgents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	UpdateEditUserAgentControl();
	UpdateDeleteUserAgentControl();	

	if ((pNMLV->uChanged & LVIF_TEXT) == LVIF_TEXT)
	{
		// the user agent string changed, change the Hint text
		UpdateUserAgentItemName(pNMLV->iItem);		
	}

	*pResult = 0;
}

void CExcludeUserAgentsDlg::UpdateControlEnableStates()
{
	if (m_cUserAgentExclusionEnabled.GetCheck() == BST_CHECKED)
	{
		m_cAddUserAgentRule.EnableWindow(TRUE);		
		UpdateEditUserAgentControl();
		UpdateDeleteUserAgentControl();
		m_cExcludedUserAgents.EnableWindow(TRUE);
	}
	else
	{
		m_cAddUserAgentRule.EnableWindow(FALSE);
		m_cEditUserAgentRule.EnableWindow(FALSE);
		m_cDeleteUserAgentRule.EnableWindow(FALSE);
		m_cExcludedUserAgents.EnableWindow(FALSE);
	}
}

void CExcludeUserAgentsDlg::UpdateUserAgentControl()
{
	DeleteAllItems();	

	CAtlArray<CAtlStringA> userAgents;
	m_Config.GetExcludedUserAgents(userAgents);

	for (int i = 0; i < (int) userAgents.GetCount(); i++)
	{		
		ExcludedUserAgentData* data = new ExcludedUserAgentData(CString(userAgents[i]));		

		int nItem = m_cExcludedUserAgents.InsertItem(i, data->userAgent);
		m_cExcludedUserAgents.SetItemData(nItem, (DWORD_PTR) data);
		SetItemImage(nItem);
		SortItems();
	}
}

void CExcludeUserAgentsDlg::UpdateUserAgentItemName(int nItem)
{
	CString sName;

	CStringA sUserAgent = CAtlStringA(m_cExcludedUserAgents.GetItemText(nItem, 0));
	if (sUserAgent.GetLength() > 0)
	{	
		m_UserAgentLookup.GetUserAgentName(sUserAgent, sName);		
	}

	m_cExcludedUserAgents.SetItemText(nItem, 1, sName);
}

void CExcludeUserAgentsDlg::SetUserAgentRuleEnabledState()
{
	if (m_Config.GetUserAgentExclusionEnabled())
	{
		m_cUserAgentExclusionEnabled.SetCheck(BST_CHECKED);
	}
	else
	{
		m_cUserAgentExclusionEnabled.SetCheck(BST_UNCHECKED);
	}
}

void CExcludeUserAgentsDlg::OnLvnEndLabelEditListUserAgents(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = 0;
		
	if (pDispInfo != NULL && pDispInfo->item.pszText != NULL && pDispInfo->item.pszText[0] != '!')
	{		
		HttpUserAgent::UserAgentProducts<Ripcord::Types::tstring> userAgent;

		if (userAgent.ParseUserAgentString(pDispInfo->item.pszText) == S_OK)
		{
			CAtlArray<CAtlStringA> agents;
			int nItems = m_cExcludedUserAgents.GetItemCount();			
			for (int i = 0; i < nItems; i++)
			{
				ExcludedUserAgentData* data = (ExcludedUserAgentData*) m_cExcludedUserAgents.GetItemData(i);
				if (data != NULL)
				{
					// TODO: consider simplifying
					if (i != pDispInfo->item.iItem)
					{
						CString userAgent;
						data->GetRawAgentString(userAgent);

						agents.Add(CAtlStringA(userAgent));
					}
					else
					{
						data->userAgent = pDispInfo->item.pszText;

						CString userAgent;
						data->GetRawAgentString(userAgent);

						agents.Add(CAtlStringA(userAgent));
					}				
				}
			}			

			SetItemImage(pDispInfo->item.iItem);

			if (m_Config.SetExcludedUserAgents(agents))
			{
				*pResult = 1;			
			}
		}			
	}	
}

void CExcludeUserAgentsDlg::OnLvnKeyDownListUserAgents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	if (pLVKeyDown->wVKey == VK_F2)
	{
		EditSelectedUserAgentString();
	}
	else if (pLVKeyDown->wVKey == VK_DELETE)
	{
		DeleteSelectedItems();
	}

	*pResult = 0;
}

void CExcludeUserAgentsDlg::DeleteAllItems()
{
	for (int i = 0; i < m_cExcludedUserAgents.GetItemCount(); i++)
	{
		DeleteItemData(i);		
	}

	m_cExcludedUserAgents.DeleteAllItems();
}

void CExcludeUserAgentsDlg::DeleteItemData(int nItem)
{
	if (nItem >= 0)
	{
		ExcludedUserAgentData* data = (ExcludedUserAgentData*) m_cExcludedUserAgents.GetItemData(nItem);
		if (data != NULL)
		{
			m_cExcludedUserAgents.SetItemData(nItem, NULL);
			delete data;
		}
	}
}

void CExcludeUserAgentsDlg::OnBnClickedButtonEditUserAgentRule()
{
	EditSelectedUserAgentString();
}

void CExcludeUserAgentsDlg::EditSelectedUserAgentString()
{
	if (m_cExcludedUserAgents.GetSelectedCount() == 1)
	{
		m_cExcludedUserAgents.SetFocus();
		int nSel = m_cExcludedUserAgents.GetSelectionMark();		
		m_cExcludedUserAgents.EditLabel(nSel);		
	}
}

void CExcludeUserAgentsDlg::OnBnClickedButtonDeleteUserAgentRule()
{	
	DeleteSelectedItems();
}

void CExcludeUserAgentsDlg::DeleteSelectedItems()
{	
	int nSelected = m_cExcludedUserAgents.GetSelectedCount();
	if (nSelected <= 0)
	{
		return;
	}

	CString sMsg;
	if (nSelected == 1)
	{
		sMsg.LoadString(IDS_MSG_REALLYDELETEUSERAGENT);
	}
	else
	{
		sMsg.LoadString(IDS_MSG_REALLYDELETEUSERAGENTS);
	}

	CString sCaption;
	sCaption.LoadString(IDS_IISXPRESS_APPNAME);

	if (MessageBox(sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		int nItemsDeleted = 0;
		int nItems = m_cExcludedUserAgents.GetItemCount();
		for (int i = nItems - 1; i >= 0; i--)
		{
			if (m_cExcludedUserAgents.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				DeleteItemData(i);
				m_cExcludedUserAgents.DeleteItem(i);
				nItemsDeleted++;
			}
		}

		if (nItemsDeleted > 0)
		{
			UpdateUserAgentExclusionConfiguration();
		}
	}	
}

void CExcludeUserAgentsDlg::UpdateEditUserAgentControl()
{
	int nSelectedCount = m_cExcludedUserAgents.GetSelectedCount();
	if (nSelectedCount == 1)
	{
		m_cEditUserAgentRule.EnableWindow(TRUE);
	}
	else
	{
		m_cEditUserAgentRule.EnableWindow(FALSE);
	}
}

void CExcludeUserAgentsDlg::UpdateDeleteUserAgentControl()
{
	int nSelectedCount = m_cExcludedUserAgents.GetSelectedCount();
	if (nSelectedCount > 0)
	{			
		m_cDeleteUserAgentRule.EnableWindow(TRUE);
	}
	else
	{
		m_cDeleteUserAgentRule.EnableWindow(FALSE);
	}
}

void CExcludeUserAgentsDlg::UpdateUserAgentExclusionConfiguration()
{
	CAtlArray<CAtlStringA> agents;

	int nAgents = m_cExcludedUserAgents.GetItemCount();
	for (int i = 0; i < nAgents; i++)
	{
		ExcludedUserAgentData* data = (ExcludedUserAgentData*) m_cExcludedUserAgents.GetItemData(i);
		if (data != NULL)
		{
			CString agent;
			data->GetRawAgentString(agent);

			agents.Add(CAtlStringA(agent));
		}
	}

	m_Config.SetExcludedUserAgents(agents);
}

void CExcludeUserAgentsDlg::OnBnClickedButtonAddUserAgentRule()
{
	CAddUserAgentRuleDlg dlg;
	if (dlg.DoModal() == IDOK)
	{		
		ExcludedUserAgentData* data = new ExcludedUserAgentData(dlg.selectedUserAgent);

		int nItem = m_cExcludedUserAgents.InsertItem(m_cExcludedUserAgents.GetItemCount(), data->userAgent);
		m_cExcludedUserAgents.SetItemData(nItem, (DWORD_PTR) data);
		SetItemImage(nItem);
		SortItems();

		UpdateUserAgentExclusionConfiguration();
	}
}

void CExcludeUserAgentsDlg::OnLvnInsertItemListUserAgents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateUserAgentItemName(pNMLV->iItem);
	*pResult = 0;
}

void CExcludeUserAgentsDlg::OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*) pNmHdr;		

	DWORD dwDrawStage = pDraw->nmcd.dwDrawStage;
	if ((dwDrawStage & CDDS_ITEM) != 0)
	{
		CListCtrl* pCtrl = (CListCtrl*) pWnd;
		
		ExcludedUserAgentData* data = (ExcludedUserAgentData*) pCtrl->GetItemData((int) pDraw->nmcd.dwItemSpec);

		if (data != NULL && !data->excluded)
		{
			static DWORD dwTextColor = ::GetSysColor(COLOR_HOTLIGHT);			
			pDraw->clrText = dwTextColor;
		}
	}
}

void CExcludeUserAgentsDlg::SetItemImage(int nItem)
{
	if (nItem >= 0)
	{
		ExcludedUserAgentData* data = (ExcludedUserAgentData*) m_cExcludedUserAgents.GetItemData(nItem);
		if (data != NULL)
		{
			int image = 0;
			if (!data->excluded)
			{
				image = 1;
			}

			LVITEM item;
			item.iItem = nItem;
			item.iSubItem = 0;
			item.mask = LVIF_IMAGE;
			item.iImage = image;

			m_cExcludedUserAgents.SetItem(&item);
		}
	}	
}

void CExcludeUserAgentsDlg::SortItems()
{
	m_cExcludedUserAgents.SortItems(MyCompareProc, NULL);
}

int CExcludeUserAgentsDlg::MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	ExcludedUserAgentData* data1 = (ExcludedUserAgentData*) lParam1;
	ExcludedUserAgentData* data2 = (ExcludedUserAgentData*) lParam2;

	if (data1->excluded && !data2->excluded)
	{
		return 1;
	}
	else if (!data1->excluded && data2->excluded)
	{
		return -1;
	}
	else
	{
		return data1->userAgent.CompareNoCase(data2->userAgent);	
	}
}