// WebSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "WebSelectFormView.h"

#include "BrowserHelper.h"

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
}

// CWebSelectFormView dialog

IMPLEMENT_DYNCREATE(CWebSelectFormView, CFormView)

CWebSelectFormView::CWebSelectFormView() : CFormView(IDD)
{
	// we don't want directories with these names
	m_IgnoreDirNames.insert(L"_vti_cnf");
	m_IgnoreDirNames.insert(L"_vti_bin");
	m_IgnoreDirNames.insert(L"_vti_script");
	m_IgnoreDirNames.insert(L"_vti_log");
	m_IgnoreDirNames.insert(L"_vti_pvt");
	m_IgnoreDirNames.insert(L"_vti_txt");
}

CWebSelectFormView::~CWebSelectFormView()
{
}

void CWebSelectFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WEBSITES, m_cWebSites);
	DDX_Control(pDX, IDC_TREE_WEBSITEDIRECTORIES, m_cWebDirectories);
	DDX_Control(pDX, IDC_BUTTONREFRESH, m_cRefresh);
	DDX_Control(pDX, IDC_STATIC_WEBSITESELECTDLGMARGINS, m_cDlgMargins);
}


BEGIN_MESSAGE_MAP(CWebSelectFormView, CFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_WEBSITES, &CWebSelectFormView::OnCbnSelChangeComboWebSites)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_WEBSITEDIRECTORIES, &CWebSelectFormView::OnTvnItemExpandingTreeWebSiteDirectories)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WEBSITEDIRECTORIES, &CWebSelectFormView::OnTvnSelChangedTreeWebSiteDirectories)
	ON_BN_CLICKED(IDC_BUTTONREFRESH, &CWebSelectFormView::OnBnClickedButtonRefresh)
	ON_WM_CONTEXTMENU()
	ON_MENUXP_MESSAGES()
	ON_COMMAND(ID_WEBTREECONTEXT_EXPLOREFOLDER, OnExploreFolder)
	ON_COMMAND(ID_WEBTREECONTEXT_BROWSEWITHINTERNETEXPLORER, OnBrowseInternetExplorer)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_IE, OnBrowseInternetExplorer)
	ON_COMMAND(ID_WEBTREECONTEXT_BROWSEWITHFIREFOX, OnBrowseFirefox)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_FIREFOX, OnBrowseFirefox)
	ON_COMMAND(ID_WEBTREECONTEXT_BROWSEWITHOPERA, OnBrowseOpera)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_OPERA, OnBrowseOpera)
	ON_COMMAND(ID_WEBTREECONTEXT_BROWSEWITHSAFARI, OnBrowseSafari)	
	ON_COMMAND(IDC_BUTTON_TOOLBAR_SAFARI, OnBrowseSafari)	
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, OnCmdUIViewRefresh)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_IE, OnCmdUIToolbarIE)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_FIREFOX, OnCmdUIToolbarFirefox)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_OPERA, OnCmdUIToolbarOpera)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_SAFARI, OnCmdUIToolbarSafari)
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CWebSelectFormView, CFormView);

IMPLEMENT_HANDLECONTEXTMENUMESSAGES(CWebSelectFormView)

// CWebSelectFormView message handlers

void CWebSelectFormView::OnInitialUpdate()
{	
	CFormView::OnInitialUpdate();
	
	m_WebSitesMargins.GetMargins(&m_cDlgMargins, &m_cWebSites);
	m_nWebDirectoriesMargins.GetMargins(&m_cDlgMargins, &m_cWebDirectories);
	m_nRefreshMargins.GetMargins(&m_cDlgMargins, &m_cRefresh);

	PositionChildControls();

	m_WebTreeContext.LoadMenu(IDR_MENUWEBTREECONTEXT);

	if (CPNGHelper::LoadPNG(_T("FOLDEROPEN16.PNG"), m_bmpFolderOpen) == true &&
		CPNGHelper::LoadPNG(_T("IEXPLORE16.PNG"), m_bmpIE) == true)
	{
		CMenuXP::SetMenuItemImage(ID_WEBTREECONTEXT_EXPLOREFOLDER, &m_bmpFolderOpen);		
		CMenuXP::SetMenuItemImage(ID_WEBTREECONTEXT_BROWSEWITHINTERNETEXPLORER, &m_bmpIE);		
	}

	CMenuXP::SetXPLookNFeel(this);	

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
	m_bmpVirtualDirectory.LoadBitmap(IDB_BITMAP_WEBDIR16);	
	m_bmpWebServer.LoadBitmap(IDB_BITMAP_WEBSERVER16);		

	// create the tree imagelist
	m_ImageList.Create(20, 16, ILC_COLOR24 | ILC_MASK, 6, 0);
	m_nFolderIndex = m_ImageList.Add(&m_bmpFolder, RGB(0, 0, 255));		
	m_nVirtualDirectoryIndex = m_ImageList.Add(&m_bmpVirtualDirectory, RGB(0, 0, 255));	
	m_nWebServerIndex = m_ImageList.Add(&m_bmpWebServer, RGB(0, 0, 255));	
	
	// assign the imagelist to the tree
	m_cWebDirectories.SetImageList(&m_ImageList, TVSIL_NORMAL);			

	PopulateWebSiteCombo();				
}

void CWebSelectFormView::OnDestroy()
{
	FreeComboItems();

	FreeTreeItems();

	CFormView::OnDestroy();
}

void CWebSelectFormView::OnSize(UINT nType, int cx, int cy)
{
	PositionChildControls();
}

void CWebSelectFormView::PositionChildControls()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_cWebSites.m_hWnd != NULL)
	{
		int x = m_WebSitesMargins.GetLeftMargin();
		int y = m_WebSitesMargins.GetTopMargin();
		int nWidth = rcClient.right - m_WebSitesMargins.GetRightMargin();
		int nHeight = m_WebSitesMargins.GetHeight();
		m_cWebSites.MoveWindow(x, y, nWidth, nHeight);
	}

	if (m_cWebDirectories.m_hWnd != NULL)
	{
		int x = m_nWebDirectoriesMargins.GetLeftMargin();
		int y = m_nWebDirectoriesMargins.GetTopMargin();
		int nWidth = rcClient.right - m_nWebDirectoriesMargins.GetRightMargin();
		int nHeight = rcClient.Height() - m_nWebDirectoriesMargins.GetTopMargin() - m_nWebDirectoriesMargins.GetBottomMargin();
		m_cWebDirectories.MoveWindow(x, y, nWidth, nHeight);
	}

	if (m_cRefresh.m_hWnd != NULL)
	{			
		int x = rcClient.Width() - m_nRefreshMargins.GetRightMargin() - m_nRefreshMargins.GetWidth();
		int y = m_nRefreshMargins.GetTopMargin();
		int nWidth = m_nRefreshMargins.GetWidth();
		int nHeight = m_nRefreshMargins.GetHeight();
		m_cRefresh.MoveWindow(x, y, nWidth, nHeight);
	}	
}

void CWebSelectFormView::OnCbnSelChangeComboWebSites()
{
	PopulateTree();	

	SelectWebSiteRoot();
}

void CWebSelectFormView::SelectWebSiteRoot()
{
	// select the root item
	HTREEITEM htWebRoot = m_cWebDirectories.GetChildItem(TVI_ROOT);
	if (htWebRoot != NULL)
	{
		m_cWebDirectories.Select(htWebRoot, TVGN_CARET);
	}
}

CView* CWebSelectFormView::GetParentView(CWnd* pWnd)
{
	if (pWnd != NULL)
	{
		pWnd = pWnd->GetParent();
		while (pWnd != NULL)
		{
			if (pWnd->IsKindOf(RUNTIME_CLASS(CView)) == TRUE)
			{
				return (CView*) pWnd;
			}

			pWnd = pWnd->GetParent();
		}
	}

	return NULL;
}

void CWebSelectFormView::PopulateTree(void)
{	
	CWaitCursor cursor;

	FreeTreeItems();
	m_cWebDirectories.DeleteAllItems();

	IISWebSite* pSelectedComboItem = GetSelectedComboInstanceData();
	if (pSelectedComboItem == NULL)
		return;

	// don't allow the window to update
	m_cWebDirectories.LockWindowUpdate();	

	// insert the root item
	HTREEITEM htRoot = m_cWebDirectories.InsertItem(_T("/"), m_nWebServerIndex, m_nWebServerIndex, TVI_ROOT);

	// we need an item data for this tree item
	IISURIItem* pItem = new IISURIItem;
	pItem->Type = IISURIItem::WebServer;
	pItem->sMetaBasePath = pSelectedComboItem->sMetaBasePath;
	pItem->sFileSystemPath = pSelectedComboItem->sFileSystemPath;
	pItem->sURI = L"/";

	m_cWebDirectories.SetItemData(htRoot, (DWORD_PTR) pItem);

	// stage 1: populate the tree with the virtual directories ...
	if (g_OSVersion.dwMajorVersion < 6)
	{
		// ... from the metabase (<= Windows 2003)
		CIISConfigHelper::PopulateTreeFromMetaBase(m_cWebDirectories, htRoot, pSelectedComboItem, m_IgnoreDirNames, m_nVirtualDirectoryIndex);
	}
	else
	{
		// ... from the XML config file (>= Vista)
		CIISConfigHelper::PopulateTreeFromXML(m_cWebDirectories, htRoot, m_nVirtualDirectoryIndex, pSelectedComboItem);
	}

	// stage 2: recurse the tree, populating the existing nodes with filesystem dirs
	CIISConfigHelper::RecurseTreeAndPopulateFromFileSystem(m_cWebDirectories, htRoot, m_IgnoreDirNames, 1); 

	// stage 3: sort all the tree items
	CIISConfigHelper::RecurseTreeAndSortItems(m_cWebDirectories, htRoot);

	// stage 4: populate with the excluded folders
	//PopulateExcludedFolders();

	m_cWebDirectories.Expand(htRoot, TVE_EXPAND);

	// allow the window to update again
	m_cWebDirectories.UnlockWindowUpdate();
}

void CWebSelectFormView::FreeTreeItems(void)
{
	HTREEITEM htRoot = m_cWebDirectories.GetChildItem(TVI_ROOT);
	FreeTreeItems(htRoot);
}

void CWebSelectFormView::FreeTreeItems(HTREEITEM htItem)
{
	if (htItem == NULL)
		return;

	HTREEITEM htChild = m_cWebDirectories.GetChildItem(htItem);
	while (htChild != NULL)
	{
		FreeTreeItems(htChild);

		htChild = m_cWebDirectories.GetNextSiblingItem(htChild);
	}

	IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(htItem);
	if (pItem != NULL)
	{
		delete pItem;
		m_cWebDirectories.SetItemData(htItem, NULL);
	}	
}

IISWebSite* CWebSelectFormView::GetSelectedComboInstanceData(void)
{
	int nSel = m_cWebSites.GetCurSel();
	if (nSel == CB_ERR)
		return NULL;

	IISWebSite* pItem = (IISWebSite*) m_cWebSites.GetItemData(nSel);
	return pItem;
}

void CWebSelectFormView::OnTvnItemExpandingTreeWebSiteDirectories(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (pNMTreeView->action == TVE_EXPAND) 
	{
		HTREEITEM hChild = m_cWebDirectories.GetChildItem(pNMTreeView->itemNew.hItem);
		if (hChild != NULL)
		{
			if (m_cWebDirectories.GetItemText(hChild) == CIISConfigHelper::EmptyNodeString)
			{
				m_cWebDirectories.DeleteItem(hChild);
				hChild = NULL;

				CIISConfigHelper::PopulateTreeFromFileSystem(m_cWebDirectories, pNMTreeView->itemNew.hItem, m_IgnoreDirNames, 1);
			}
		}
	}
	
	*pResult = 0;
}

void CWebSelectFormView::OnTvnSelChangedTreeWebSiteDirectories(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);	
	*pResult = 0;

	CView* pView = GetParentView(this);
	if (pView != NULL && pNMTreeView->itemNew.hItem != NULL)
	{
		IISWebSite* pWebSite = GetSelectedComboInstanceData();
		IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(pNMTreeView->itemNew.hItem);
		if (pWebSite != NULL && pItem != NULL)
		{
			CNotifyWebDirectorySelectionChanged notify;	
			notify.sPhysicalPath = pItem->sFileSystemPath;			
			notify.sURI = pItem->sURI;
			notify.Ports.Copy(pWebSite->Ports);
			notify.SecurePorts.Copy(pWebSite->SecurePorts);

			pView->GetDocument()->UpdateAllViews(pView, 0, &notify);
		}
	}	
}

void CWebSelectFormView::OnBnClickedButtonRefresh()
{
	OnViewRefresh();
}

void CWebSelectFormView::PopulateWebSiteCombo()
{
	FreeComboItems();

	int nItems = m_cWebSites.GetCount();
	for (int i = 0; i < nItems; i++)
	{
		m_cWebSites.DeleteString(0);
	}

	std::vector<IISWebSite> WebSites;
	if (g_OSVersion.dwMajorVersion < 6)
	{
		CIISConfigHelper::EnumWebSitesFromMetaBase(WebSites);		
	}
	else
	{
		CIISConfigHelper::EnumWebSitesFromXML(WebSites);
	}

	int nSites = (int) WebSites.size();
	for (int i = 0; i < nSites ; i++)
	{
		IISWebSite* pItem = new IISWebSite(WebSites[i]);

		CString sDescription;
		sDescription.Format(_T("%ls (%ls)"), pItem->sDescription, pItem->sInstance);

		int nItem = m_cWebSites.AddString(sDescription);				
		m_cWebSites.SetItemData(nItem, (DWORD_PTR) pItem);
	}	

	if (nSites > 0)
	{
		m_cWebSites.SetCurSel(0);
		PopulateTree();
		SelectWebSiteRoot();
	}	
}

void CWebSelectFormView::FreeComboItems()
{
	int nItems = m_cWebSites.GetCount();
	for (int i = 0; i < nItems; i++)
	{
		IISWebSite* pItem = (IISWebSite*) m_cWebSites.GetItemData(i);		
		m_cWebSites.SetItemData(i, NULL);

		delete pItem;
	}
}

void CWebSelectFormView::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	if (pWnd != NULL && pWnd->GetSafeHwnd() == m_cWebDirectories.GetSafeHwnd())
	{
		OnWebTreeContextMenu(pWnd, pos);
	}
}

void CWebSelectFormView::OnWebTreeContextMenu(CWnd* pWnd, CPoint pos)
{
	// make sure the user has clicked on one of the tree items
	CPoint ptClient(pos);
	m_cWebDirectories.ScreenToClient(&ptClient);

	UINT nFlags = 0;
	HTREEITEM htClicked = m_cWebDirectories.HitTest(ptClient, &nFlags);
	if (htClicked == NULL || nFlags != TVHT_ONITEMLABEL)
	{
		return;
	}

	CMenu* pSubMenu = m_WebTreeContext.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;	

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 		
			pos.x, pos.y, ::AfxGetMainWnd());
}

void CWebSelectFormView::OnExploreFolder()
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL)
	{
		IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(htSelected);
		if (pItem != NULL)
		{
			::ShellExecute(NULL, _T("open"), pItem->sFileSystemPath, NULL, pItem->sFileSystemPath, SW_SHOWNORMAL);			
		}
	}
}

void CWebSelectFormView::OnBrowseInternetExplorer()
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL)
	{
		IISWebSite* pWebSite = GetSelectedComboInstanceData();
		IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(htSelected);		

		if (pWebSite != NULL && pItem != NULL)
		{
			CString sURL;
			FormatURL(pWebSite->Ports, pWebSite->SecurePorts, pItem->sURI, _T(""), sURL);

			if (CBrowserHelper::LaunchIE(sURL) == false)
			{
				::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
			}
		}
	}
}

void CWebSelectFormView::OnBrowseFirefox()
{	
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL)
	{
		IISWebSite* pWebSite = GetSelectedComboInstanceData();
		IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(htSelected);		

		if (pWebSite != NULL && pItem != NULL)
		{
			CString sURL;
			FormatURL(pWebSite->Ports, pWebSite->SecurePorts, pItem->sURI, _T(""), sURL);

			if (CBrowserHelper::LaunchFirefox(sURL) == false)
			{
				::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
			}
		}
	}
}

void CWebSelectFormView::OnBrowseOpera()
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL)
	{
		IISWebSite* pWebSite = GetSelectedComboInstanceData();
		IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(htSelected);		

		if (pWebSite != NULL && pItem != NULL)
		{
			CString sURL;
			FormatURL(pWebSite->Ports, pWebSite->SecurePorts, pItem->sURI, _T(""), sURL);

			if (CBrowserHelper::LaunchOpera(sURL) == false)
			{
				::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
			}
		}
	}
}

void CWebSelectFormView::OnBrowseSafari()
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL)
	{
		IISWebSite* pWebSite = GetSelectedComboInstanceData();
		IISURIItem* pItem = (IISURIItem*) m_cWebDirectories.GetItemData(htSelected);		

		if (pWebSite != NULL && pItem != NULL)
		{
			CString sURL;
			FormatURL(pWebSite->Ports, pWebSite->SecurePorts, pItem->sURI, _T(""), sURL);			

			if (CBrowserHelper::LaunchSafari(sURL) == false)
			{
				::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
			}
		}
	}
}

bool CWebSelectFormView::FormatURL(const CAtlArray<IISWebSiteBindings>& Ports, const CAtlArray<IISWebSiteBindings>& SecurePorts, LPCTSTR pszURI, LPCTSTR pszFilename, CString& sURL)
{
	TCHAR* pszProtocol = _T("http");
	CAtlArray<IISWebSiteBindings> const* pPorts = &Ports;
	if (pPorts->GetCount() == 0)
	{
		pszProtocol = _T("https");
		pPorts = &SecurePorts;
	}

	if (pPorts->GetCount() == 0)
	{
		return false;
	}

	CString sHostname(_T("localhost"));
	if (pPorts->GetAt(0).sHostname.GetLength() > 0)
	{
		sHostname = pPorts->GetAt(0).sHostname;
	}
	else if (pPorts->GetAt(0).sIP.GetLength() > 0)
	{
		sHostname = pPorts->GetAt(0).sIP;
	}

	CString sURI(pszURI);
	if (sURI.GetLength() == 0 || sURI.Right(1) != _T("/"))
	{
		sURI += '/';
	}
	
	sURL.Format(_T("%s://%s:%u%s%s"), pszProtocol, sHostname, pPorts->GetAt(0).dwPort, sURI, pszFilename);	
	sURL.Replace(_T(" "), _T("%20"));

	return true;
}

void CWebSelectFormView::OnViewRefresh()
{
	PopulateWebSiteCombo();
}

void CWebSelectFormView::OnCmdUIViewRefresh(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CWebSelectFormView::OnCmdUIExploreFolder(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CWebSelectFormView::OnCmdUIToolbarIE(CCmdUI* pCmdUI)
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CWebSelectFormView::OnCmdUIToolbarFirefox(CCmdUI* pCmdUI)
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL && CBrowserHelper::IsFirefoxInstalled() == true)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CWebSelectFormView::OnCmdUIToolbarOpera(CCmdUI* pCmdUI)
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL && CBrowserHelper::IsOperaInstalled() == true)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CWebSelectFormView::OnCmdUIToolbarSafari(CCmdUI* pCmdUI)
{
	HTREEITEM htSelected = m_cWebDirectories.GetSelectedItem();
	if (htSelected != NULL && CBrowserHelper::IsSafariInstalled() == true)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}