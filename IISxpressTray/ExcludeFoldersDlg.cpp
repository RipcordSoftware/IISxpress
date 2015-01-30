// ExcludeFoldersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ExcludeFoldersDlg.h"
#include "AddFolderRuleDlg.h"

#include <vector>
#include <string>
using namespace std;

#include "resource.h"
#include ".\excludefoldersdlg.h"

BEGIN_MESSAGE_MAP(CMyRClickTreeCtrl, CTreeCtrl)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CMyRClickTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{	
	// make sure the control has the input focus (this won't happen unless we do it here)
	SetFocus();

	// determine whether the user has clicked on a tree item or not
	UINT nHitTestFlags = 0;
	HTREEITEM htItem = HitTest(point, &nHitTestFlags);
	if (htItem != NULL && (nHitTestFlags & TVHT_ONITEM) != 0)
	{	
		// select the item
		SelectItem(htItem);		
	}
	else
	{
		// the user didn't click anywhere useful, deselect
		SelectItem(NULL);
	}
}

// *************************************************************************************************** //

// CExcludeFoldersDlg dialog

IMPLEMENT_DYNAMIC(CExcludeFoldersDlg, CHelpAwareDialogBase)

CExcludeFoldersDlg::CExcludeFoldersDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CExcludeFoldersDlg::IDD, pParent), m_dwUpdateCookie(0)
{
	m_nLastSelectedInstanceItem = CB_ERR;

	// we don't want directories with these names
	m_IgnoreDirNames.insert(L"_vti_cnf");
	m_IgnoreDirNames.insert(L"_vti_bin");
	m_IgnoreDirNames.insert(L"_vti_script");
	m_IgnoreDirNames.insert(L"_vti_log");
	m_IgnoreDirNames.insert(L"_vti_pvt");
	m_IgnoreDirNames.insert(L"_vti_txt");
}

CExcludeFoldersDlg::~CExcludeFoldersDlg()
{
}

void CExcludeFoldersDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_INSTANCES, m_cInstances);
	DDX_Control(pDX, IDC_BUTTONREFRESH, m_cRefresh);
	DDX_Control(pDX, IDC_TREE_EXCLUDEDURIS, m_cURITree);
}


BEGIN_MESSAGE_MAP(CExcludeFoldersDlg, CHelpAwareDialogBase)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_INSTANCES, OnCbnSelChangeComboInstances)			
	ON_BN_CLICKED(IDC_BUTTONREFRESH, OnBnClickedButtonRefresh)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_URIMENU_EXPLORE, OnExplore)
	ON_COMMAND(ID_URIMENU_BROWSE, OnBrowse)
	ON_COMMAND(ID_URIMENU_EXCLUDE_BRANCH, OnExcludeBranch)
	ON_COMMAND(ID_URIMENU_EXCLUDE_FOLDER, OnExcludeFolder)
	ON_COMMAND(ID_URIMENU_INCLUDE, OnInclude)
	ON_NOTIFY(TVN_GETINFOTIP, IDC_TREE_EXCLUDEDURIS, OnTvnGetInfoTipTreeExcludedURIs)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TREE_EXCLUDEDURIS, OnCustomDraw)
	ON_MENUXP_MESSAGES()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_EXCLUDEDURIS, OnTvnItemExpandingURITree)
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CExcludeFoldersDlg, CHelpAwareDialogBase);

// CExcludeFoldersDlg message handlers
BOOL CExcludeFoldersDlg::OnInitDialog(void)
{
	BOOL bStatus = CHelpAwareDialogBase::OnInitDialog();		

	if (::LoadPNG(_T("folder16.png"), m_bmpMenuFolder) == true &&
		::LoadPNG(_T("browse16.png"), m_bmpMenuBrowse) == true)
	{
		CMenuXP::SetXPLookNFeel(this);
		CMenuXP::SetMenuItemImage(ID_URIMENU_EXPLORE, &m_bmpMenuFolder);
		CMenuXP::SetMenuItemImage(ID_URIMENU_BROWSE, &m_bmpMenuBrowse);
	}

	// load the 16x16 refresh icon
	HICON hRefreshIcon = (HICON) ::LoadImage(::AfxGetApp()->m_hInstance, 
		MAKEINTRESOURCE(IDI_ICONREFRESH), IMAGE_ICON, 16, 16, 0);

	// inform developer of problems
	ASSERT(hRefreshIcon != NULL);

	// set the icon into the refresh button
	if (hRefreshIcon != NULL)
		m_cRefresh.SetIcon(hRefreshIcon);

	// load the bitmaps we are going to use in the tree
	m_bmpFolder.LoadBitmap(IDB_BITMAP_FOLDER16);	
	m_bmpFolderExcluded.LoadBitmap(IDB_BITMAP_FOLDEREX16);
	m_bmpVirtualDirectory.LoadBitmap(IDB_BITMAP_WEBDIR16);
	m_bmpVirtualDirectoryExcluded.LoadBitmap(IDB_BITMAP_WEBDIREX16);
	m_bmpWebServer.LoadBitmap(IDB_BITMAP_WEBSERVER16);	
	m_bmpWebServerExcluded.LoadBitmap(IDB_BITMAP_WEBSERVEREX16);	

	// create the tree imagelist
	m_ImageList.Create(20, 16, ILC_COLOR24 | ILC_MASK, 6, 0);
	m_nFolderIndex = m_ImageList.Add(&m_bmpFolder, RGB(0, 0, 255));	
	m_nFolderExcludedIndex = m_ImageList.Add(&m_bmpFolderExcluded, RGB(0, 0, 255));	
	m_nVirtualDirectoryIndex = m_ImageList.Add(&m_bmpVirtualDirectory, RGB(0, 0, 255));
	m_nVirtualDirectoryExcludedIndex = m_ImageList.Add(&m_bmpVirtualDirectoryExcluded, RGB(0, 0, 255));
	m_nWebServerIndex = m_ImageList.Add(&m_bmpWebServer, RGB(0, 0, 255));
	m_nWebServerExcludedIndex = m_ImageList.Add(&m_bmpWebServerExcluded, RGB(0, 0, 255));
	
	// assign the imagelist to the tree
	m_cURITree.SetImageList(&m_ImageList, TVSIL_NORMAL);			

	// ask for a V5 tree control on W2K and earlier
	if (g_VerInfo.dwMajorVersion < 5 || (g_VerInfo.dwMajorVersion == 5 && g_VerInfo.dwMinorVersion < 1))
	{
		m_cURITree.SendMessage(CCM_SETVERSION, 5, 0);
	}

	m_PopupMenu.LoadMenu(IDR_MENU_URICONTEXT);

	// load the tooltip caption
	CString sCaption;
	sCaption.LoadString(IDS_TOOLTIP_URIEXCLUDED_CAPTION);

	// associate a tooltip control with the URI tree
	m_cTips.Create(this, TTS_BALLOON);
	m_cTips.AddTool(&m_cURITree);
	m_cTips.SetDelayTime(TTDT_INITIAL, 2000);
	m_cTips.SetDelayTime(TTDT_AUTOPOP, 8000);
	m_cTips.SetDelayTime(TTDT_RESHOW, 2000);
	m_cTips.SetTitle(TTI_INFO, sCaption);	

	// force the controls into a disabled state
	UpdateControls(TRUE);

	return bStatus;
}

BOOL CExcludeFoldersDlg::PreTranslateMessage(MSG* pMsg)
{
	// intercept mouse messages since the tooltip control needs to know about them
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	{
		// if the source message window is the tree then we need to pass this on to the tooltip control
		if (pMsg->hwnd == m_cURITree.m_hWnd)
		{
			m_cTips.RelayEvent(pMsg);
		}

		// if the mouse has moved and the source was the tree then we need to handle it
		if (pMsg->message == WM_MOUSEMOVE && pMsg->hwnd == m_cURITree.m_hWnd)
		{
			static HTREEITEM htLast = NULL;

			// map the cursor pos to the tree
			CPoint ptCursor(pMsg->pt);
			m_cURITree.ScreenToClient(&ptCursor);

			// if the mouse is not over an item then kill the tooltip
			UINT nFlags = 0;
			HTREEITEM htItem = m_cURITree.HitTest(ptCursor, &nFlags);
			if (htItem == NULL || htItem != htLast || (nFlags & TVHT_ONITEM) == 0)
			{
				m_cTips.Pop();
				ATLTRACE("PreTranslateMessage() - Pop()\n");
			}
			
			// store the last tooltip item, this allows us to go from one to another
			htLast = htItem;		
		}	
	}

	// allow the dialog to do its work
	return CHelpAwareDialogBase::PreTranslateMessage(pMsg);
}

BOOL CExcludeFoldersDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// intercept the tooltip's notification
	NMHDR* pNmHdr = (NMHDR*) lParam;
	if (pNmHdr->code == TTN_NEEDTEXT && pNmHdr->hwndFrom == m_cTips)
	{
		NMTTDISPINFO* pDispInfo = (NMTTDISPINFO*) lParam;		

		// map the cursor position to the tree control
		CPoint ptCursor;
		::GetCursorPos(&ptCursor);
		m_cURITree.ScreenToClient(&ptCursor);

		// determine whether the cursor is over an item
		UINT nFlags = 0;
		HTREEITEM htItem = m_cURITree.HitTest(ptCursor, &nFlags);
		if (htItem != NULL && (nFlags & TVHT_ONITEM) != 0)
		{		
			// get the item data
			IISURIItem* pItem = (IISURIItem*) m_cURITree.GetItemData(htItem);
			if (pItem != NULL && pItem->bExcluded == TRUE)
			{
				// we need a static buffer (very lame)
				static TCHAR szTipBuffer[512] = _T("\0");

				// load the string if we haven't already
				if (szTipBuffer[0] == '\0')
				{
					CString sMsg;
					sMsg.LoadString(IDS_TOOLTIP_URIEXCLUDED_MSG);

					_tcsncpy_s(szTipBuffer, _countof(szTipBuffer), sMsg, sizeof(szTipBuffer) / sizeof(szTipBuffer[0]));
				}

				// tell the control about the text
				pDispInfo->lpszText = szTipBuffer;
			}
		}

		// nothing else should handle this, so return
		return TRUE;
	}

	return CHelpAwareDialogBase::OnNotify(wParam, lParam, pResult);
}

void CExcludeFoldersDlg::UpdateControls(DWORD dwUpdateCookie)
{
	if (dwUpdateCookie == 0 || m_dwUpdateCookie != dwUpdateCookie)
	{
		UpdateControls();
	}
}

void CExcludeFoldersDlg::UpdateControls(BOOL bForceDisabled)
{	
	vector<IISWebSite> sites;

	if (bForceDisabled == FALSE)
	{
		CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
		if (pSheet->GetIISxpressInterfaces() != S_OK)
		{
			// we failed to get IISxpress interfaces, assume the server isn't running and force a disable
			bForceDisabled = TRUE;
		}
		{
			if (g_VerInfo.dwMajorVersion < 6)
			{						
				// if we are running on Windows 2003 and earlier then we have a metabase, so enumerate
				// the web sites from it
				bForceDisabled = CIISConfigHelper::EnumWebSitesFromMetaBase(sites) == TRUE ? FALSE : TRUE;
			}
			else
			{			
				// if we are running on Vista and later then we don't have a metabase, so enumerate
				// the web sites from the XML config file
				bForceDisabled = CIISConfigHelper::EnumWebSitesFromXML(sites) == TRUE ? FALSE : TRUE;
			}

			m_dwUpdateCookie = pSheet->GetUpdateCookie();
		}
	}

	// either the caller asked for disable or the controls couldn't be populated, so disable them
	if (bForceDisabled == TRUE)
	{
		// clear out the instance combo
		FreeComboInstanceStrings();
		m_cInstances.ResetContent();
		m_cInstances.EnableWindow(FALSE);
		m_nLastSelectedInstanceItem = CB_ERR;

		// clear out the URI tree
		FreeTreeItems();
		m_cURITree.DeleteAllItems();
		m_cURITree.EnableWindow(FALSE);

		m_cRefresh.EnableWindow(FALSE);

		return;
	}	

	// the matched instance index
	int nNewInstanceSel = CB_ERR;

	// get the old selected instance string
	CAtlStringW sOldSelectedInstance;
	int nOldInstanceSel = m_cInstances.GetCurSel();	
	if (nOldInstanceSel != CB_ERR)
	{
		IISWebSite* pItem = (IISWebSite*) m_cInstances.GetItemData(nOldInstanceSel);
		if (pItem != NULL)			
			sOldSelectedInstance = pItem->sInstance;
	}

	m_cInstances.EnableWindow(TRUE);			
	FreeComboInstanceStrings();
	m_cInstances.ResetContent();

	m_cURITree.EnableWindow(TRUE);

	m_cRefresh.EnableWindow(TRUE);

	int nSites = (int) sites.size();
	for (int i = 0; i < nSites ; i++)
	{
		IISWebSite* pItem = new IISWebSite(sites[i]);

		CString sDescription;
		sDescription.Format(_T("%ls (%ls)"), pItem->sDescription, pItem->sInstance);

		int nItem = m_cInstances.AddString(sDescription);				
		m_cInstances.SetItemData(nItem, (DWORD_PTR) pItem);

		// match the previously selected instance string
		if (sOldSelectedInstance == pItem->sInstance)
		{
			nNewInstanceSel = nItem;
		}
	}

	// select the previous instance if we have match
	if (nNewInstanceSel != CB_ERR)
	{
		m_cInstances.SetCurSel(nNewInstanceSel);
	}			
	else
	{
		m_cInstances.SetCurSel(0);
	}			
	
	// populate the URI tree
	PopulateTree();
}

void CExcludeFoldersDlg::FreeComboInstanceStrings(void)
{
	int nInstances = m_cInstances.GetCount();
	for (int i = 0; i < nInstances; i++)
	{
		IISWebSite* pItem = (IISWebSite*) m_cInstances.GetItemData(i);
		delete pItem;

		m_cInstances.SetItemData(i, NULL);
	}
}

void CExcludeFoldersDlg::FreeTreeItems(void)
{
	HTREEITEM htRoot = m_cURITree.GetChildItem(TVI_ROOT);
	FreeTreeItems(htRoot);
}

void CExcludeFoldersDlg::FreeTreeItems(HTREEITEM htItem)
{
	if (htItem == NULL)
		return;

	HTREEITEM htChild = m_cURITree.GetChildItem(htItem);
	while (htChild != NULL)
	{
		FreeTreeItems(htChild);

		htChild = m_cURITree.GetNextSiblingItem(htChild);
	}

	IISURIItem* pItem = (IISURIItem*) m_cURITree.GetItemData(htItem);
	if (pItem != NULL)
	{
		delete pItem;
		m_cURITree.SetItemData(htItem, NULL);
	}	
}

void CExcludeFoldersDlg::OnDestroy(void)
{
	FreeComboInstanceStrings();
	FreeTreeItems();
}

void CExcludeFoldersDlg::OnCbnSelChangeComboInstances()
{
	int nSel = m_cInstances.GetCurSel();
	if (nSel == CB_ERR)
		return;

	// prevent the user causing an update by preventing them selecting the same item they have already selected
	if (nSel == m_nLastSelectedInstanceItem)
		return;

	m_nLastSelectedInstanceItem = nSel;

	PopulateTree();
}

bool CExcludeFoldersDlg::GetInstanceExclusionFolders(
								LPCTSTR pszInstance, 
								map<CStringW, DWORD, CStringWCompareNoCase>& Folders)								
{	
	Folders.clear();

	if (pszInstance == NULL)
		return false;

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();
	if (pSheet->GetIISxpressInterfaces() != S_OK)
		return false;

	CComPtr<ICompressionRuleManager> pCompressionRuleManager;
	pSheet->GetCompressionManager(&pCompressionRuleManager);
	if (pCompressionRuleManager == NULL)
		return false;
	
	CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
	if (pNeverCompressRules == NULL)
		return false;
	
	CComPtr<IEnumExclusionRule> pExcludeFolders;
	if (pNeverCompressRules->EnumFolders(&pExcludeFolders) != S_OK)
		return false;

	CComBSTR bsInstance(pszInstance);

	ExclusionRuleInfo RuleInfo;
	ULONG nFetched = 0;
	while (pExcludeFolders->Next(1, &RuleInfo, &nFetched) == S_OK)
	{
		if (RuleInfo.bsInstance == NULL)
			continue;

		if (bsInstance != RuleInfo.bsInstance)
			continue;

		Folders[RuleInfo.bsRule] = RuleInfo.dwFlags;		
	}

	return true;
}

IISWebSite* CExcludeFoldersDlg::GetSelectedComboInstanceData(void)
{
	int nInstanceSel = m_cInstances.GetCurSel();
	if (nInstanceSel == CB_ERR)
		return NULL;

	IISWebSite* pItem = (IISWebSite*) m_cInstances.GetItemData(nInstanceSel);
	return pItem;
}

void CExcludeFoldersDlg::OnBnClickedButtonRefresh()
{
	CWaitCursor Cursor;	
	UpdateControls();	
}

void CExcludeFoldersDlg::PopulateTree(void)
{	
	FreeTreeItems();
	m_cURITree.DeleteAllItems();

	IISWebSite* pSelectedComboItem = GetSelectedComboInstanceData();
	if (pSelectedComboItem == NULL)
		return;

	// don't allow the window to update
	m_cURITree.LockWindowUpdate();

	// insert the root item
	HTREEITEM htRoot = m_cURITree.InsertItem(_T("/"), m_nWebServerIndex, m_nWebServerIndex, TVI_ROOT);

	// we need an item data for this tree item
	IISURIItem* pItem = new IISURIItem;
	pItem->Type = IISURIItem::WebServer;
	pItem->sMetaBasePath = pSelectedComboItem->sMetaBasePath;
	pItem->sFileSystemPath = pSelectedComboItem->sFileSystemPath;
	pItem->sURI = L"/";

	m_cURITree.SetItemData(htRoot, (DWORD_PTR) pItem);

	// stage 1: populate the tree with the virtual directories ...
	if (g_VerInfo.dwMajorVersion < 6)
	{
		// ... from the metabase (<= Windows 2003)
		CIISConfigHelper::PopulateTreeFromMetaBase(m_cURITree, htRoot, pSelectedComboItem, m_IgnoreDirNames, m_nVirtualDirectoryIndex);
	}
	else
	{
		// ... from the XML config file (>= Vista)
		CIISConfigHelper::PopulateTreeFromXML(m_cURITree, htRoot, m_nVirtualDirectoryIndex, pSelectedComboItem);
	}

	// stage 2: recurse the tree, populating the existing nodes with filesystem dirs
	CIISConfigHelper::RecurseTreeAndPopulateFromFileSystem(m_cURITree, htRoot, m_IgnoreDirNames, 1);

	// stage 3: sort all the tree items
	CIISConfigHelper::RecurseTreeAndSortItems(m_cURITree, htRoot);

	// stage 4: populate with the excluded folders
	PopulateExcludedFolders();

	m_cURITree.Expand(htRoot, TVE_EXPAND);

	// allow the window to update again
	m_cURITree.UnlockWindowUpdate();
}

void CExcludeFoldersDlg::OnContextMenu(CWnd* pWnd, CPoint pos)
{	
	// only handle the URI tree context menu 
	if (pWnd->GetSafeHwnd() != m_cURITree.GetSafeHwnd())
		return;

	CRect rcTree;
	m_cURITree.GetWindowRect(rcTree);	

	CMenu* pSubMenu = m_PopupMenu.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;

	// assume these menu items are disabled
	EnableMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_FOLDER, MF_GRAYED);
	EnableMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_BRANCH, MF_GRAYED);
	EnableMenuItem(pSubMenu, ID_URIMENU_INCLUDE, MF_GRAYED);
	EnableMenuItem(pSubMenu, ID_URIMENU_BROWSE, MF_GRAYED);
	EnableMenuItem(pSubMenu, ID_URIMENU_EXPLORE, MF_GRAYED);

	// assume these menu items are unchecked
	CheckMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_BRANCH, MF_UNCHECKED);
	CheckMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_FOLDER, MF_UNCHECKED);
	CheckMenuItem(pSubMenu, ID_URIMENU_INCLUDE, MF_UNCHECKED);

	// get the currently selected item if there is one
	HTREEITEM htSel = m_cURITree.GetSelectedItem();	

	// detect an invalid position - possibly a key press activation
	if (pos.x < 0 || pos.y < 0)
	{		
		if (htSel != NULL)
		{
			// get the selected item's rectangle
			CRect rcItem;
			m_cURITree.GetItemRect(htSel, &rcItem, TRUE);

			// calculate a suitable position for the menu
			pos.x = rcItem.left + 5;
			pos.y = rcItem.top + (rcItem.Height() / 2);

			// map to screen co-ords for display
			m_cURITree.ClientToScreen(&pos);
		}
		else
		{
			// nothing is selected, assume top left of the control
			pos.x = 0;
			pos.y = 0;

			// map to screen co-ords for display
			m_cURITree.ClientToScreen(&pos);
		}		
	}	

	// get the URI info if there is a selected item
	IISURIItem* pURIItem = NULL;
	if (htSel != NULL)
	{
		pURIItem = (IISURIItem*) m_cURITree.GetItemData(htSel);
		ASSERT(pURIItem != NULL);
	}	

	if (pSubMenu != NULL && pURIItem != NULL)
	{		
		// enable the exclusion menu items
		if (pURIItem->bExcluded == FALSE)		
		{
			EnableMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_FOLDER, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_BRANCH, MF_ENABLED);			
		}

		// enable/disable the include menu item
		if (pURIItem->bExcluded == TRUE && pURIItem->dwExclusionType != RULEFLAGS_FOLDER_NOMATCH)
		{
			EnableMenuItem(pSubMenu, ID_URIMENU_INCLUDE, MF_ENABLED);
		}		
		
		if (pURIItem->dwExclusionType == RULEFLAGS_FOLDER_EXACTMATCH)
		{			
			CheckMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_FOLDER, MF_CHECKED);			
		}
		else if (pURIItem->dwExclusionType == RULEFLAGS_FOLDER_WILDCARDMATCH)
		{						
			CheckMenuItem(pSubMenu, ID_URIMENU_EXCLUDE_BRANCH, MF_CHECKED);			
		}
		else if (pURIItem->bExcluded == FALSE)
		{			
			CheckMenuItem(pSubMenu, ID_URIMENU_INCLUDE, MF_CHECKED);
		}	

		// we have a selection, these are always enabled
		EnableMenuItem(pSubMenu, ID_URIMENU_BROWSE, MF_ENABLED);
		EnableMenuItem(pSubMenu, ID_URIMENU_EXPLORE, MF_ENABLED);
	}	

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 		
			pos.x, pos.y, this);
}

void CExcludeFoldersDlg::OnExplore(void)
{
	// get the selected item
	HTREEITEM htSel = m_cURITree.GetSelectedItem();
	if (htSel == NULL)
		return;

	// get the item data
	IISURIItem* pItem = (IISURIItem*) m_cURITree.GetItemData(htSel);
	if (pItem == NULL)
		return;

	// test to see if the directory exists
	if (::GetFileAttributesW(pItem->sFileSystemPath) == INVALID_FILE_ATTRIBUTES)
	{		
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_ERROR_MISSINGFOLDER, pItem->sFileSystemPath);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONWARNING);

		return;
	}

	// ask the shell to 'explore' it
	::ShellExecuteW(GetSafeHwnd(), L"explore", pItem->sFileSystemPath, NULL, NULL, SW_SHOW);
}

void CExcludeFoldersDlg::OnBrowse(void)
{
	// make sure there is something selected in the tree
	HTREEITEM htSel = m_cURITree.GetSelectedItem();
	if (htSel == NULL)
		return;

	// make sure something is selected in the combo
	int nInstanceSel = m_cInstances.GetCurSel();
	if (nInstanceSel == CB_ERR)
		return;	

	// get the URI info
	IISURIItem* pURIItem = (IISURIItem*) m_cURITree.GetItemData(htSel);
	if (pURIItem == NULL)
		return;

	// get the instance info
	IISWebSite* pInstanceItem = (IISWebSite*) m_cInstances.GetItemData(nInstanceSel);
	if (pInstanceItem == NULL)
		return;	
	
	// get the hostname and port
	CString sHostName;
	DWORD dwPort = 80;
	if (g_VerInfo.dwMajorVersion < 6)
	{
		// get a metabase connection
		CIISMetaBase MetaBase;
		CComPtr<IMSAdminBase> pAdminBase;
		if (MetaBase.Init() != true ||
			MetaBase.GetAdminBase(&pAdminBase) != true)
		{
			return;
		}

		if (CIIS6ConfigHelper::GetWebServerHostnameAndPort(pAdminBase, 
			CString(pInstanceItem->sMetaBasePath), sHostName, dwPort) == false)
		{
			return;
		}
	}
	else
	{
		CComPtr<IXMLDOMDocument> pDoc;
		CComPtr<IXMLDOMNode> pSite;

		if (CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDoc) != S_OK ||
			CIIS7XMLConfigHelper::GetSiteNode(pDoc, pInstanceItem->sInstance, &pSite) != S_OK ||
			CIIS7XMLConfigHelper::GetWebServerHostnameAndPort(pSite, sHostName, dwPort) != S_OK)
		{
			return;
		}
	}

	// if the hostname is zero length then assume it's localhost
	if (sHostName.GetLength() <= 0)
	{
		sHostName = _T("localhost");
	}
	
	// create a URL object
	CUrl Url;
	Url.SetScheme(ATL_URL_SCHEME_HTTP);
	Url.SetHostName(sHostName);
	Url.SetPortNumber((ATL_URL_PORT) dwPort);
	Url.SetUrlPath(CString(pURIItem->sURI));

	// get a useful URL string
	TCHAR szURLBuffer[2048];
	DWORD dwURLBufferSize = sizeof(szURLBuffer) / sizeof(szURLBuffer[0]);
	Url.CreateUrl(szURLBuffer, &dwURLBufferSize, ATL_URL_ESCAPE);

	// ask the shell to open the browser window
	::ShellExecute(GetSafeHwnd(), _T("open"), szURLBuffer, NULL, NULL, SW_SHOW);
}

void CExcludeFoldersDlg::OnTvnGetInfoTipTreeExcludedURIs(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);	

#ifdef _DEBUG
	IISURIItem* pItem = (IISURIItem*) pGetInfoTip->lParam;
	if (pItem != NULL)
	{
		CString sTip;
		sTip.Format(
			_T("URI: %ls\n")
			_T("File System: %ls\n")
			_T("Metabase path: %ls\n")
			_T("Exclusion type: %u"), 
			pItem->sURI, pItem->sFileSystemPath, pItem->sMetaBasePath, 
			pItem->dwExclusionType);

		_tcsncpy(pGetInfoTip->pszText, sTip, pGetInfoTip->cchTextMax);
	}
#endif

	*pResult = 0;
}

void CExcludeFoldersDlg::PopulateExcludedFolders(HTREEITEM htParent)
{
	// make sure something is selected in the combo
	int nInstanceSel = m_cInstances.GetCurSel();
	if (nInstanceSel == CB_ERR)
		return;

	// get the instance info
	IISWebSite* pInstanceItem = (IISWebSite*) m_cInstances.GetItemData(nInstanceSel);
	if (pInstanceItem == NULL)
		return;	

	map<CStringW, DWORD, CStringWCompareNoCase> Folders;
	if (GetInstanceExclusionFolders(CString(pInstanceItem->sInstance), Folders) == false)
		return;

	if (htParent == NULL)
	{
		HTREEITEM htRoot = m_cURITree.GetRootItem();
		htParent = htRoot;
	}

	PopulateExcludedFolders(htParent, Folders);
}

void CExcludeFoldersDlg::PopulateExcludedFolders(HTREEITEM htParent, const map<CStringW, DWORD, CStringWCompareNoCase>& ExcludedFolders, bool bForceExclude)
{
	if (htParent == NULL)
		return;

	IISURIItem* pParentItem = (IISURIItem*) m_cURITree.GetItemData(htParent);
	if (pParentItem == NULL)
		return;

	// store the old excluded state (we need to know when to force a text update)
	BOOL bOldExcludedState = pParentItem->bExcluded;

	// assume it's not excluded
	pParentItem->bExcluded = FALSE;
	pParentItem->dwExclusionType = RULEFLAGS_FOLDER_NOMATCH;

	// if we are not forcing then determine whether this node is excluded or not
	bool bExcludeParent = false;
	if (bForceExclude == false)
	{
		map<CStringW, DWORD, CStringWCompareNoCase>::const_iterator iter = ExcludedFolders.find(pParentItem->sURI);
		if (iter != ExcludedFolders.end())
		{
			bExcludeParent = true;			

			if (iter->second == RULEFLAGS_FOLDER_WILDCARDMATCH)
			{
				// force all children to be excluded
				bForceExclude = true;				
			}

			// store the exclusion type
			pParentItem->dwExclusionType = iter->second;
		}
	}	

	// get the item image
	int nImageIndex = -1;
	if (bExcludeParent == true || bForceExclude == true)
	{
		switch (pParentItem->Type)
		{
		case IISURIItem::VirtualRoot:		nImageIndex = m_nVirtualDirectoryExcludedIndex; break;
		case IISURIItem::FileSystem:		nImageIndex = m_nFolderExcludedIndex; break;
		case IISURIItem::WebServer:			nImageIndex = m_nWebServerExcludedIndex; break;
		}
	}
	else
	{
		switch (pParentItem->Type)
		{
		case IISURIItem::VirtualRoot:		nImageIndex = m_nVirtualDirectoryIndex; break;
		case IISURIItem::FileSystem:		nImageIndex = m_nFolderIndex; break;
		case IISURIItem::WebServer:			nImageIndex = m_nWebServerIndex; break;
		}
	}

	if (nImageIndex >= 0)
	{
		m_cURITree.SetItemImage(htParent, nImageIndex, nImageIndex);
	}	

	// mark the item as excluded or not
	pParentItem->bExcluded = (bExcludeParent == true || bForceExclude == true);

	// we can get around the font size problem by forcing a text update
	if (bOldExcludedState != pParentItem->bExcluded)
	{
		CString sText = m_cURITree.GetItemText(htParent);
		m_cURITree.SetItemText(htParent, sText);
	}	

	HTREEITEM htChild = m_cURITree.GetChildItem(htParent);
	while (htChild != NULL)
	{		
		PopulateExcludedFolders(htChild, ExcludedFolders, bForceExclude);

		htChild = m_cURITree.GetNextSiblingItem(htChild);		
	}			
}

void CExcludeFoldersDlg::OnCustomDraw(NMHDR* pNmHdr, LRESULT* pResult)
{
	NMTVCUSTOMDRAW* pDraw = (NMTVCUSTOMDRAW*) pNmHdr;

	*pResult = 0;

	DWORD dwDrawStage = pDraw->nmcd.dwDrawStage;
	switch (dwDrawStage)
	{

	case CDDS_PREPAINT:

		*pResult = CDRF_NOTIFYITEMDRAW;
		return;	

	case CDDS_ITEMPREPAINT:

		{
			IISURIItem* pItem = (IISURIItem*) pDraw->nmcd.lItemlParam;
			if (pItem != NULL && pItem->bExcluded == TRUE && ::IsWindowEnabled(this->m_hWnd) == TRUE)
			{				
				// create a BOLD font based on the current font
				HFONT ftCurrent = (HFONT) ::GetCurrentObject(pDraw->nmcd.hdc, OBJ_FONT);
				if (ftCurrent != NULL)
				{
					LOGFONT LogFont;
					if (::GetObject(ftCurrent, sizeof(LogFont), &LogFont) != 0)
					{
						LogFont.lfWeight = FW_NORMAL + 200;

						HFONT ftExcluded = ::CreateFontIndirect(&LogFont);
						if (ftExcluded != NULL)
						{
							::SelectObject(pDraw->nmcd.hdc, ftExcluded);
							::DeleteObject(ftExcluded);														

							*pResult = CDRF_NEWFONT;
						}
					}
				}

				if ((pDraw->nmcd.uItemState & CDIS_SELECTED) == 0)
				{
					pDraw->clrText = RGB(255, 0, 0);
				}						
			}

			return;
		}
	}
}

bool CExcludeFoldersDlg::EnableMenuItem(CMenu* pMenu, UINT nID, UINT nEnable)
{
	if (pMenu == NULL)
		return false;	

	int nItems = pMenu->GetMenuItemCount();
	for (int i = 0; i < nItems; i++)
	{
		UINT nItemID = pMenu->GetMenuItemID(i);
		if (nItemID == nID)
		{
			pMenu->EnableMenuItem(nID, MF_BYCOMMAND | nEnable);
			return true;
		}
		else if (nItemID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);
			if (pSubMenu != NULL)
			{
				if (EnableMenuItem(pSubMenu, nID, nEnable) == true)
					return true;
			}
		}
	}

	return false;
}

bool CExcludeFoldersDlg::CheckMenuItem(CMenu* pMenu, UINT nID, UINT nCheck)
{
	if (pMenu == NULL)
		return false;	

	int nItems = pMenu->GetMenuItemCount();
	for (int i = 0; i < nItems; i++)
	{
		UINT nItemID = pMenu->GetMenuItemID(i);
		if (nItemID == nID)
		{
			pMenu->CheckMenuItem(nID, MF_BYCOMMAND | nCheck);
			return true;
		}
		else if (nItemID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);
			if (pSubMenu != NULL)
			{
				if (CheckMenuItem(pSubMenu, nID, nCheck) == true)
					return true;
			}
		}
	}

	return false;
}

void CExcludeFoldersDlg::OnExcludeBranch(void)
{
	// get the currently selected item if there is one
	HTREEITEM htSel = m_cURITree.GetSelectedItem();
	if (htSel == NULL)
		return;

	IISURIItem* pItem = (IISURIItem*) m_cURITree.GetItemData(htSel);
	if (pItem == NULL)
		return;

	if (pItem->bExcluded == TRUE)
		return;	

	int nInstanceSel = m_cInstances.GetCurSel();
	if (nInstanceSel == CB_ERR)
		return;

	// get the instance info
	IISWebSite* pInstanceItem = (IISWebSite*) m_cInstances.GetItemData(nInstanceSel);
	if (pInstanceItem == NULL)
		return;	

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<ICompressionRuleManager> pRuleManager;	
	if (pSheet->GetIISxpressInterfaces() != S_OK || pSheet->GetCompressionManager(&pRuleManager) != S_OK)
		return;	

	CComQIPtr<INeverCompressRules> pNeverRules = pRuleManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(CStringA(pInstanceItem->sInstance), CStringA(pItem->sURI), RULEFLAGS_FOLDER_WILDCARDMATCH, m_dwUpdateCookie);	

	if (hr == S_OK)
	{
		// we need to reset the compression cache since the config has changed		
		CIISxpressTrayPropertySheet::ResetResponseCache();
	}

	map<CStringW, DWORD, CStringWCompareNoCase> Folders;
	if (GetInstanceExclusionFolders(CString(pInstanceItem->sInstance), Folders) == false)
		return;

	PopulateExcludedFolders(htSel, Folders, false);
}

void CExcludeFoldersDlg::OnExcludeFolder(void)
{
	// get the currently selected item if there is one
	HTREEITEM htSel = m_cURITree.GetSelectedItem();
	if (htSel == NULL)
		return;

	IISURIItem* pItem = (IISURIItem*) m_cURITree.GetItemData(htSel);
	if (pItem == NULL)
		return;

	if (pItem->bExcluded == TRUE)
		return;	

	int nInstanceSel = m_cInstances.GetCurSel();
	if (nInstanceSel == CB_ERR)
		return;

	// get the instance info
	IISWebSite* pInstanceItem = (IISWebSite*) m_cInstances.GetItemData(nInstanceSel);
	if (pInstanceItem == NULL)
		return;		

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<ICompressionRuleManager> pRuleManager;	
	if (pSheet->GetIISxpressInterfaces() != S_OK || pSheet->GetCompressionManager(&pRuleManager) != S_OK)
		return;	

	CComQIPtr<INeverCompressRules> pNeverRules = pRuleManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(CStringA(pInstanceItem->sInstance), CStringA(pItem->sURI), RULEFLAGS_FOLDER_EXACTMATCH, m_dwUpdateCookie);	

	if (hr == S_OK)
	{
		// we need to reset the compression cache since the config has changed		
		CIISxpressTrayPropertySheet::ResetResponseCache();
	}

	map<CStringW, DWORD, CStringWCompareNoCase> Folders;
	if (GetInstanceExclusionFolders(CString(pInstanceItem->sInstance), Folders) == false)
		return;

	PopulateExcludedFolders(htSel, Folders, false);
}

void CExcludeFoldersDlg::OnInclude(void)
{
	// get the currently selected item if there is one
	HTREEITEM htSel = m_cURITree.GetSelectedItem();
	if (htSel == NULL)
		return;

	IISURIItem* pItem = (IISURIItem*) m_cURITree.GetItemData(htSel);
	if (pItem == NULL)
		return;

	if (pItem->bExcluded == FALSE)
		return;	

	if (pItem->dwExclusionType == RULEFLAGS_FOLDER_NOMATCH)
		return;

	int nInstanceSel = m_cInstances.GetCurSel();
	if (nInstanceSel == CB_ERR)
		return;

	// get the instance info
	IISWebSite* pInstanceItem = (IISWebSite*) m_cInstances.GetItemData(nInstanceSel);
	if (pInstanceItem == NULL)
		return;		

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	CComPtr<ICompressionRuleManager> pRuleManager;	
	if (pSheet->GetIISxpressInterfaces() != S_OK || pSheet->GetCompressionManager(&pRuleManager) != S_OK)
		return;	

	CComQIPtr<INeverCompressRules> pNeverRules = pRuleManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->RemoveFolderRule(CStringA(pInstanceItem->sInstance), CStringA(pItem->sURI), m_dwUpdateCookie);	
	if (hr != S_OK)
		return;

	// we need to reset the compression cache since the config has changed		
	CIISxpressTrayPropertySheet::ResetResponseCache();

	// get the rules from the server
	map<CStringW, DWORD, CStringWCompareNoCase> Folders;
	if (GetInstanceExclusionFolders(CString(pInstanceItem->sInstance), Folders) == false)
		return;

	PopulateExcludedFolders(htSel, Folders, false);
}

void CExcludeFoldersDlg::OnTvnItemExpandingURITree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (pNMTreeView->action == TVE_EXPAND) 
	{
		HTREEITEM hChild = m_cURITree.GetChildItem(pNMTreeView->itemNew.hItem);
		if (hChild != NULL)
		{
			if (m_cURITree.GetItemText(hChild) == CIISConfigHelper::EmptyNodeString)
			{
				m_cURITree.DeleteItem(hChild);
				hChild = NULL;

				CIISConfigHelper::PopulateTreeFromFileSystem(m_cURITree, pNMTreeView->itemNew.hItem, m_IgnoreDirNames, 1);

				PopulateExcludedFolders(pNMTreeView->itemNew.hItem);
			}
		}
	}
	
	*pResult = 0;
}