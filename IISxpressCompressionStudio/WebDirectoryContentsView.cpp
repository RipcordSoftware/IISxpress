// WebDirectoryContentsView.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "WebDirectoryContentsView.h"

#include <atlpath.h>
#include <atlutil.h>

#include <vector>
#include <algorithm>

#include "CStringCompareNoCase.h"

#include "BrowserHelper.h"

#include <msxml2.h>

// CWebDirectoryContentsView

IMPLEMENT_DYNCREATE(CWebDirectoryContentsView, CListView)

CWebDirectoryContentsView::CWebDirectoryContentsView()
{

}

CWebDirectoryContentsView::~CWebDirectoryContentsView()
{
}

BEGIN_MESSAGE_MAP(CWebDirectoryContentsView, CAutoSortableHeaderListView<CColouredListView>)	
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)	
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_MENUXP_MESSAGES()
	ON_COMMAND(ID_WEBDIRCONTEXT_WRITETOCSV, OnWriteToCSV)
	ON_COMMAND(ID_WEBDIRCONTEXT_EXPLOREFOLDER, OnExploreFolder)
	ON_COMMAND(ID_WEBDIRCONTEXT_ANALYZE, OnAnalyze)
	ON_COMMAND(ID_WEBDIRCONTEXT_ZLIB, OnAnalyzeZLIB)
	ON_COMMAND(ID_WEBDIRCONTEXT_CSS, OnAnalyzeCSS)
	ON_COMMAND(ID_WEBDIRCONTEXT_CLEAR, OnClearHistory)
	ON_COMMAND(ID_WEBDIRCONTEXT_OPENWITHDEFAULTAPPLICATION, OnOpenWithDefaultApplication)
	ON_COMMAND(ID_WEBDIRCONTEXT_OPENWITHNOTEPAD, OnOpenWithNotepad)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_FILESIZE, OnShowHideColumnFileSize)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_ORIGINALSIZE, OnShowHideColumnOrigSize)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_COMPRESSEDSIZE, OnShowHideColumnCompSize)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_RATIO, OnShowHideColumnRatio)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_RESPONSES, OnShowHideColumnResponses)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_LASTDOWNLOAD, OnShowHideColumnLastDown)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_CONTENTTYPE, OnShowHideColumnContentType)
	ON_COMMAND(ID_WEBDIRECTORYHEADERCONTEXT_COMPRESS, OnShowHideColumnCompress)
	ON_COMMAND(ID_WEBDIRCONTEXT_COMPRESSAUTOMATIC, OnCompressAutomatic)
	ON_COMMAND(ID_WEBDIRCONTEXT_COMPRESSALWAYS, OnCompressAlways)
	ON_COMMAND(ID_WEBDIRCONTEXT_COMPRESSNEVER, OnCompressNever)
	ON_COMMAND(ID_WEBDIRCONTEXT_EXCLUDETHISEXTENSION, OnExcludeExtension)	
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDEEXTN, OnExcludeExtension)	
	ON_COMMAND(ID_WEBDIRCONTEXT_EXCLUDETHISCONTENTTYPE, OnExcludeContentType)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDECT, OnExcludeContentType)	
	ON_COMMAND(ID_WEBDIRCONTEXT_EXCLUDEMATCHINGCONTENTTYPES, OnExcludeMatchingContentTypes)		
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDECT_MATCH, OnExcludeMatchingContentTypes)	
	ON_COMMAND(ID_WEBDIRCONTEXT_BROWSEWITHINTERNETEXPLORER, OnBrowseInternetExplorer)
	ON_COMMAND(ID_WEBDIRCONTEXT_BROWSEWITHFIREFOX, OnBrowseFirefox)
	ON_COMMAND(ID_WEBDIRCONTEXT_BROWSEWITHOPERA, OnBrowseOpera)
	ON_COMMAND(ID_WEBDIRCONTEXT_BROWSEWITHSAFARI, OnBrowseSafari)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectAll)	
	ON_COMMAND(IDC_BUTTON_TOOLBAR_IE, OnBrowseInternetExplorer)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_FIREFOX, OnBrowseFirefox)	
	ON_COMMAND(IDC_BUTTON_TOOLBAR_OPERA, OnBrowseOpera)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_SAFARI, OnBrowseSafari)	

	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, OnCmdUIAlwaysEnabled)	
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnCmdUIListViewHasItems)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_IE, OnCmdUIToolbarIE)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_BROWSEWITHINTERNETEXPLORER, OnCmdUIToolbarIE)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_FIREFOX, OnCmdUIToolbarFirefox)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_BROWSEWITHFIREFOX, OnCmdUIToolbarFirefox)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_OPERA, OnCmdUIToolbarOpera)	
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_BROWSEWITHSAFARI, OnCmdUIToolbarSafari)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_SAFARI, OnCmdUIToolbarSafari)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_BROWSEWITHOPERA, OnCmdUIToolbarOpera)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_WRITETOCSV, OnCmdUIListViewHasItems)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_COMPRESSAUTOMATIC, OnCmdUICompressionMode)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_COMPRESSALWAYS, OnCmdUICompressionMode)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_COMPRESSNEVER, OnCmdUICompressionMode)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_EXCLUDETHISEXTENSION, OnCmdUIListViewHasSelectedItem)	
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDEEXTN, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_EXCLUDETHISCONTENTTYPE, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDECT, OnCmdUIListViewHasSelectedItem)	
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_EXCLUDEMATCHINGCONTENTTYPES, OnCmdUIListViewHasSelectedItem)		
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDECT_MATCH, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_OPENWITHDEFAULTAPPLICATION, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_OPENWITHNOTEPAD, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_ANALYZE, OnCmdUIListViewHasSelectedItems)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_ZLIB, OnCmdUIListViewHasSelectedItems)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_CLEAR, OnCmdUIListViewHasSelectedItems)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_EXPLOREFOLDER, OnCmdUIAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_WEBDIRCONTEXT_CSS, OnCmdUIAnalyzeCSS)
	
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CWebDirectoryContentsView, CAutoSortableHeaderListView<CColouredListView>);

IMPLEMENT_HANDLECONTEXTMENUMESSAGES(CWebDirectoryContentsView)

// CWebDirectoryContentsView diagnostics

#ifdef _DEBUG
void CWebDirectoryContentsView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CWebDirectoryContentsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWebDirectoryContentsView message handlers

BOOL CWebDirectoryContentsView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}

void CWebDirectoryContentsView::OnDestroy()
{
	FreeItemData();

	CAutoSortableHeaderListView<CColouredListView>::OnDestroy();	
}

int CWebDirectoryContentsView::OnCreate(LPCREATESTRUCT lpcs)
{
	CAutoSortableHeaderListView<CColouredListView>::OnCreate(lpcs);

	CListCtrl& cListCtrl = GetListCtrl();	

	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();

	cListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP | LVS_EX_HEADERDRAGDROP);

	CreateColumns();

	cListCtrl.SetImageList(g_ShellImageList, LVSIL_SMALL);

	CMenuXP::SetXPLookNFeel(this);

	m_HeaderPopupMenu.LoadMenu(IDR_MENU_WEBDIRHEADERCONTEXT);
	m_ListPopupMenu.LoadMenu(IDR_MENU_WEBDIRCONTEXT);

	if (CPNGHelper::LoadPNG(_T("GENMIXEDMEDIADOC16.PNG"), m_bmpMixedMedia) == true &&
		CPNGHelper::LoadPNG(_T("IDRDLL16.PNG"), m_bmpIDRDLL) == true &&
		CPNGHelper::LoadPNG(_T("IEXPLORE16.PNG"), m_bmpIE) == true &&
		CPNGHelper::LoadPNG(_T("FOLDEROPEN16.PNG"), m_bmpFolderOpen) == true)
	{				
		CMenuXP::SetMenuItemImage(ID_WEBDIRCONTEXT_EXCLUDETHISCONTENTTYPE, &m_bmpMixedMedia);		
		CMenuXP::SetMenuItemImage(ID_WEBDIRCONTEXT_EXCLUDETHISEXTENSION, &m_bmpIDRDLL);		
		CMenuXP::SetMenuItemImage(ID_WEBDIRCONTEXT_BROWSEWITHINTERNETEXPLORER, &m_bmpIE);	
		CMenuXP::SetMenuItemImage(ID_WEBDIRCONTEXT_EXPLOREFOLDER, &m_bmpFolderOpen);
	}	

	m_sCompressStateAuto.LoadString(IDS_COMPRESSSTATE_AUTO);
	m_sCompressStateAlways.LoadString(IDS_COMPRESSSTATE_ALWAYS);
	m_sCompressStateNever.LoadString(IDS_COMPRESSSTATE_NEVER);

	return 0;
}

void CWebDirectoryContentsView::InsertColumn(int nColumn, int nId, int nFormat, int nWidth, WebDirectoryContentsViewRowData::SortMethod fnSort)
{
	CListCtrl& cListCtrl = GetListCtrl();	

	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();	

	CString sName;
	sName.LoadString(nId);					

	nColumn = cListCtrl.InsertColumn(nColumn, sName, nFormat, nWidth);	
	if (pcHeader != NULL && nColumn >= 0)
	{
		m_ColumnSortMethodMap[nId] = fnSort;
		HDITEM item;
		item.mask = HDI_LPARAM;
		item.lParam = nId;
		pcHeader->SetItem(nColumn, &item);
	}
}

int CWebDirectoryContentsView::GetColumnId(int nColumn)
{
	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader == NULL)
	{
		return -1;
	}

	HDITEM item;
	item.mask = HDI_LPARAM;
	if (pcHeader->GetItem(nColumn, &item) == FALSE)
	{
		return -1;
	}

	return (int) item.lParam;
}

void CWebDirectoryContentsView::GetColumnIndexMap(std::map<int, int>& indexes)
{
	indexes.clear();

	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();	
	if (pcHeader == NULL)
	{
		return;
	}

	int nColumns = pcHeader->GetItemCount();
	for (int i = 0; i < nColumns; i++)
	{
		HDITEM item;
		item.mask = HDI_LPARAM;
		pcHeader->GetItem(i, &item);

		indexes[(int)item.lParam] = i;
	}
}

void CWebDirectoryContentsView::OnInitialUpdate()
{
}

void CWebDirectoryContentsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pHint == NULL)
	{
		return;
	}

	CNotifyWebDirectorySelectionChanged* pNotify = dynamic_cast<CNotifyWebDirectorySelectionChanged*>(pHint);
	if (pNotify != NULL)
	{
		m_Ports.Copy(pNotify->Ports);
		m_SecurePorts.Copy(pNotify->SecurePorts);
		m_sPhysicalPath = pNotify->sPhysicalPath;
		m_sURI = pNotify->sURI;
		PopulateFromDirectory(pNotify->sPhysicalPath);			
		return;
	}	

	CNotifyServerState* pServerState = dynamic_cast<CNotifyServerState*>(pHint);
	if (pServerState != NULL)
	{
		if (pServerState->ServerOnline() == true)
		{
			OnServerOnline();
		}
		else
		{
			OnServerOffline();
		}
	}
}

void CWebDirectoryContentsView::PopulateFromDirectory(LPCWSTR pszDir)
{
	CWaitCursor cursor;

	FreeItemData();

	CListCtrl& cListCtrl = GetListCtrl();	
	cListCtrl.DeleteAllItems();

	if (pszDir == NULL || pszDir[0] == '\0')
	{
		return;
	}
	
	CComPtr<ICompressionRuleManager> pCompressionManager;
	CComQIPtr<ICompressionHistory> pHistory;
	CComQIPtr<IStaticFileRules> pStaticRules;
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc != NULL)
	{		
		if (pDoc->GetCompressionManager(&pCompressionManager) == S_OK && pCompressionManager != NULL)
		{
			pHistory = pCompressionManager;
			pStaticRules = pCompressionManager;
		}		
	}

	// create a search string
	CPathT<CStringW> SearchPath(pszDir);
	SearchPath.Append(L"*.*");
	CStringW sSearchPath = (LPCWSTR) SearchPath;

	// start the search
	WIN32_FIND_DATAW FindData;	
	HANDLE hFind = ::FindFirstFileW(sSearchPath, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	std::set<CString, CStringCompareNoCase> foundFilenames;
	int nItem = 0;
	do
	{
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		__int64 nFileSize = FindData.nFileSizeHigh;
		nFileSize <<= 32;
		nFileSize |= FindData.nFileSizeLow;

		nItem = InsertItem(nItem, pszDir, FindData.cFileName, nFileSize, pHistory, pStaticRules);
		nItem++;

		foundFilenames.insert(FindData.cFileName);
	}
	while (::FindNextFileW(hFind, &FindData) == TRUE);	

	// get rid of the find handle
	::FindClose(hFind);
	hFind = NULL;	

	if (pHistory != NULL)
	{
		CComPtr<IEnumString> pHistoryEntries;
		HRESULT hr = pHistory->EnumEntriesByPath(CStringA(pszDir), &pHistoryEntries);
		if (hr == S_OK && pHistoryEntries != NULL)
		{
			LPOLESTR pszEntry = NULL;		
			while (pHistoryEntries->Next(1, &pszEntry, NULL) == S_OK)
			{
				if (foundFilenames.find(CString(pszEntry)) == foundFilenames.end())
				{
					nItem = InsertItem(nItem, pszDir, pszEntry, -1, pHistory, pStaticRules);				
					nItem++;
				}

				::CoTaskMemFree(pszEntry);
				pszEntry = NULL;
			}
		}
	}

	// now apply the sort if there is one
	ApplySort();
}

int CWebDirectoryContentsView::InsertItem(int nPosition, LPCWSTR pszDir, LPCWSTR pszFilename, __int64 nFileSize, ICompressionHistory* pHistory, IStaticFileRules* pStaticRules)
{
	CListCtrl& cListCtrl = GetListCtrl();	

	int nImageOffset = g_ShellImageList.GetExtensionOffset(pszFilename);		

	int nItem = cListCtrl.InsertItem(nPosition, pszFilename, nImageOffset);
	if (nItem >= 0)
	{
		WebDirectoryContentsViewRowData* pData = new WebDirectoryContentsViewRowData();
		pData->m_sPhysicalPath = CString(pszDir);
		pData->m_sFileName = CString(pszFilename);
		pData->m_nFileSize = nFileSize;		
		ResetHistoryData(*pData);

		CPathT<CStringW> FullFilePath(pszDir);
		FullFilePath.Append(pszFilename);	

		CString sExtension(FullFilePath.GetExtension());		
		m_ContentTypes.GetContentTypeFromExtension(sExtension, pData->m_sContentType);

		if (pStaticRules != NULL)
		{
			int nUserCompress = -1;
			int nSystemCompress = -1;
			HRESULT hr = pStaticRules->GetFileRule(CStringA(FullFilePath), &nUserCompress, &nSystemCompress);

			if (hr == S_OK)
			{
				if (nUserCompress < 0)
				{
					pData->m_CompressState = WebDirectoryContentsViewRowData::Automatic;
				}
				else if (nUserCompress == 0)
				{
					pData->m_CompressState = WebDirectoryContentsViewRowData::Never;
				}
				else if (nUserCompress > 0)
				{
					pData->m_CompressState = WebDirectoryContentsViewRowData::Always;
				}			
			}					

			if (pHistory != NULL)
			{			
				GetFileHistory(pHistory, FullFilePath, pData->m_nRawSize, pData->m_nCompressedSize, pData->m_nDownloads, pData->m_fRatio, pData->m_stLastDownload);
			}		
		}

		cListCtrl.SetItemData(nItem, (DWORD_PTR) pData);

		UpdateItem(nItem, *pData);		
	}

	return nItem;
}

void CWebDirectoryContentsView::OnSortColumn(int nColumn, BOOL bAscending)
{
	ApplySort(nColumn, bAscending);
}

void CWebDirectoryContentsView::ApplySort()
{
	int nSelectedColumn = GetSelectedColumn();
	if (nSelectedColumn >= 0)
	{
		ColumnSortOrder order = GetColumnSort(nSelectedColumn);
		if (order == Ascending || order == Descending)
		{
			ApplySort(nSelectedColumn, order == Ascending);
		}
	}
}

void CWebDirectoryContentsView::ApplySort(int nColumn, BOOL bAscending)
{
	int nId = GetColumnId(nColumn);

	WebDirectoryContentsViewRowData::SortMethod* pfnSortMethod = m_ColumnSortMethodMap[nId];
	if (pfnSortMethod == NULL)
	{
		return;
	}

	DWORD dwSelectionCookie = ::GetTickCount();

	CListCtrl& cListCtrl = GetListCtrl();
	int nItems = cListCtrl.GetItemCount();

	std::vector<WebDirectoryContentsViewRowData*> items(nItems);

	for (int i = 0; i < nItems; i++)
	{
		items[i] = (WebDirectoryContentsViewRowData*) cListCtrl.GetItemData(i);

		// mark the selected items with the cookie and de-select the item in the control
		if (cListCtrl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			items[i]->m_dwSelectionCookie = dwSelectionCookie;
			cListCtrl.SetItemState(i, 0, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}		
	
	pfnSortMethod(items);	
	
	if (bAscending == FALSE)
	{
		std::reverse(items.begin(), items.end());
	}

	for (int i = 0; i < nItems; i++)
	{
		UpdateItem(i, *items[i]);
		cListCtrl.SetItemData(i, (DWORD_PTR) items[i]);

		// select the previously selected items again
		if (items[i]->m_dwSelectionCookie == dwSelectionCookie)
		{
			cListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
	}	
}

bool CWebDirectoryContentsView::GetFileHistory(ICompressionHistory* pHistory, LPCTSTR pszFullPath, __int64& nRawSize, __int64& nCompressedSize, __int64& nDownloads, float& fRatio, SYSTEMTIME& stLastDownload)
{
	if (pHistory == NULL || pszFullPath == NULL)
	{
		return false;
	}

	FileCompressionHistory history;
	HRESULT hr = pHistory->QueryEntry(CStringA(pszFullPath), &history);
	if (hr != S_OK)
	{
		return false;
	}	

	nRawSize = history.dwRawSize;				

	nCompressedSize = history.dwCompressedSize;						

	fRatio = 0.0;
	if (history.dwRawSize > 0)
	{
		fRatio = (float) history.dwCompressedSize;
		fRatio /= (float) history.dwRawSize;
		fRatio = 1.0f - fRatio;
		fRatio *= 100.0f;

		// if the compression ratio is somehow less than zero set it to zero!
		if (fRatio < 0.0f)
			fRatio = 0.0f;					
	}				
	
	nDownloads = history.dwDownloads;				

	COleDateTime lastDownloadTime(history.vLastDownload.date);
	lastDownloadTime.GetAsSystemTime(stLastDownload);

	return true;
}

void CWebDirectoryContentsView::UpdateItem(int nItem, const WebDirectoryContentsViewRowData& data)
{
	std::map<int, int> ColumnMap;
	GetColumnIndexMap(ColumnMap);

	CListCtrl& cListCtrl = GetListCtrl();

	// update the item image
	{
		int nImageOffset = g_ShellImageList.GetExtensionOffset(data.m_sFileName);		

		LVITEM item;
		item.iItem = nItem;
		item.iSubItem = 0;
		item.mask = LVIF_IMAGE;
		item.iImage = nImageOffset;
		cListCtrl.SetItem(&item);
	}
	
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_FILENAME) != ColumnMap.end())
	{
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_FILENAME], data.m_sFileName);
	}

	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPRESS) != ColumnMap.end())
	{
		CString sCompressState(m_sCompressStateAuto);

		switch (data.m_CompressState)
		{
		case WebDirectoryContentsViewRowData::Automatic: sCompressState = m_sCompressStateAuto; break;
		case WebDirectoryContentsViewRowData::Always: sCompressState = m_sCompressStateAlways; break;
		case WebDirectoryContentsViewRowData::Never: sCompressState = m_sCompressStateNever; break;
		}

		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_COMPRESS], sCompressState);
	}

	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_CONTENTTYPE) != ColumnMap.end())	
	{		
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_CONTENTTYPE], data.m_sContentType);
	}
	
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_FILESIZE) != ColumnMap.end())	
	{
		CString sFileSize;
		if (data.m_nFileSize >= 0)
		{
			sFileSize.Format(_T("%I64u"), data.m_nFileSize);
			CInternationalHelper::FormatIntegerNumber(sFileSize);
		}
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_FILESIZE], sFileSize);
	}
	
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_ORIGSIZE) != ColumnMap.end())
	{
		CString sRawSize;		
		if (data.m_nRawSize >= 0)
		{
			sRawSize.Format(_T("%I64d"), data.m_nRawSize);
			CInternationalHelper::FormatIntegerNumber(sRawSize);
		}
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_ORIGSIZE], sRawSize);
	}
		
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPSIZE) != ColumnMap.end())	
	{
		CString sCompressedSize;
		if (data.m_nCompressedSize >= 0)
		{
			sCompressedSize.Format(_T("%I64d"), data.m_nCompressedSize);
			CInternationalHelper::FormatIntegerNumber(sCompressedSize);
		}
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_COMPSIZE], sCompressedSize);
	}
		
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPRATIO) != ColumnMap.end())	
	{
		CString sRatio;
		if (data.m_fRatio >= 0.0)
		{
			sRatio.Format(_T("%0.1f%%"), data.m_fRatio);	
			CInternationalHelper::FormatFloatNumber(sRatio, 1);	
		}
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_COMPRATIO], sRatio);
	}
	
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPRESPONSES) != ColumnMap.end())
	{
		CString sDownloads;
		if (data.m_nDownloads >= 0)
		{
			sDownloads.Format(_T("%I64d"), data.m_nDownloads);
			CInternationalHelper::FormatIntegerNumber(sDownloads);
		}
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_COMPRESPONSES], sDownloads);
	}
	
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_LASTDOWNLOAD) != ColumnMap.end())
	{
		CString sLastDownload;			
		CInternationalHelper::FormatDateTime(data.m_stLastDownload, FALSE, sLastDownload);
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_FILEVIEW_COLUMN_LASTDOWNLOAD], sLastDownload);	
	}
}

void CWebDirectoryContentsView::FreeItemData()
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nItems = cListCtrl.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		WebDirectoryContentsViewRowData* pData = (WebDirectoryContentsViewRowData*) cListCtrl.GetItemData(i);
		cListCtrl.SetItemData(i, NULL);
		delete pData;		
	}
}

void CWebDirectoryContentsView::OnServerOnline()
{
	PopulateFromDirectory(m_sPhysicalPath);
}

void CWebDirectoryContentsView::OnServerOffline()
{
	PopulateFromDirectory(m_sPhysicalPath);
}

void CWebDirectoryContentsView::ResetHistoryData(WebDirectoryContentsViewRowData& data)
{
	data.m_nRawSize = -1;
	data.m_nCompressedSize = -1;
	data.m_nDownloads = -1;
	data.m_fRatio = -1.0;
	memset(&data.m_stLastDownload, 0, sizeof(data.m_stLastDownload));
}

void CWebDirectoryContentsView::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CListCtrl& cListCtrl = GetListCtrl();

	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader != NULL)
	{
		CRect rcHeader;
		pcHeader->GetWindowRect(rcHeader);

		if (rcHeader.PtInRect(pos) == TRUE)
		{
			OnHeaderContextMenu(pWnd, pos);
			return;
		}
	}

	OnListContextMenu(pWnd, pos);
}

void CWebDirectoryContentsView::OnWriteToCSV()
{
	CListCtrl& cListCtrl = GetListCtrl();
	CDumpListCtrlToCSV::Dump(cListCtrl, GetParent());
}

void CWebDirectoryContentsView::OnExploreFolder()
{
	if (m_sPhysicalPath.GetLength() <= 0)
	{
		return;
	}

	::ShellExecute(::GetDesktopWindow(), _T("open"), m_sPhysicalPath, NULL, m_sPhysicalPath, SW_SHOWNORMAL);
}

void CWebDirectoryContentsView::OnAnalyze()
{
	CWaitCursor cursor;

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}

	CComPtr<IIISxpressHTTPRequest> pRequest;
	HRESULT hr = pDoc->GetHTTPRequest(&pRequest);
	if (hr != S_OK || pRequest == NULL)
	{
		return;
	}
	
	CComPtr<ICompressionRuleManager> pCompressionManager;
	if (pDoc->GetCompressionManager(&pCompressionManager) != S_OK || pCompressionManager == NULL)
	{
		return;
	}	

	CComQIPtr<ICompressionHistory> pHistory = pCompressionManager;

	CListCtrl& cListCtrl = GetListCtrl();		

	std::vector<int> selectedItems;
	CListCtrlHelper::GetSelectedItems(cListCtrl, selectedItems);

	for (size_t i = 0; i < selectedItems.size(); i++)
	{
		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, selectedItems[i]);
		if (pData == NULL)
		{
			// TODO: assert here
			continue;
		}

		CPath FullPhysicalPath(pData->m_sPhysicalPath);	
		FullPhysicalPath.Append(pData->m_sFileName);

		CFile file;
		if (file.Open(FullPhysicalPath, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan) == FALSE)
		{
			// TODO: handle this better
			continue;
		}

		CString sExtn = FullPhysicalPath.GetExtension();
		CStringA sContentType("application-x/unknown");
		m_ContentTypes.GetContentTypeFromExtension(sExtn, sContentType);

		DWORD dwFileSize = (DWORD) file.GetLength();

		CStringA sPhysicalPath(pData->m_sPhysicalPath);
		CStringA sFullPhysicalPath(FullPhysicalPath);

		CStringA sURI(m_sURI);
		if (sURI.GetLength() > 0 && sURI[sURI.GetLength() - 1] != '/')
		{
			sURI += '/';
		}
		sURI += pData->m_sFileName;

		IISInfo iisInfo;
		memset(&iisInfo, 0, sizeof(iisInfo));
		iisInfo.pszInstanceId = "1";
		iisInfo.pszServerName = "127.0.0.1";
		iisInfo.pszServerPort = "80";
		iisInfo.pszURLMapPath = sFullPhysicalPath;

		// TODO: probably handle deflate/gzip switching here
		RequestInfo requestInfo;
		memset(&requestInfo, 0, sizeof(requestInfo));
		requestInfo.pszAcceptEncoding = "deflate, gzip";
		requestInfo.pszHostname = "127.0.0.1";
		requestInfo.pszQueryString = "";
		requestInfo.pszRemoteAddress = "127.0.0.1";
		requestInfo.pszURI = sURI;
		requestInfo.pszUserAgent = "IISxpressCompressionStudio/1.0";	

		ResponseInfo responseInfo;
		memset(&responseInfo, 0, sizeof(responseInfo));
		responseInfo.dwResponseCode = 200;
		responseInfo.pszContentType = sContentType;
		responseInfo.dwContentLength = dwFileSize;

		// TODO: need to add error handling to this section
		DWORD dwContext = 0;
		hr = pRequest->OnSendResponse(&iisInfo, &requestInfo, &responseInfo, &dwContext);
		if (hr == S_OK)
		{
			CComPtr<IStream> pStream;
			hr = ::CreateStreamOnHGlobal(NULL, TRUE, &pStream);		
			
			CStringA sHeader;
			sHeader.Format(
				"HTTP/1.1 200 OK\r\n"
				"Content-Length: %u\r\n"
				"Content-Type: %s\r\n"
				"\r\n", dwFileSize, sContentType);

			ULONG nWritten = 0;
			hr = pStream->Write(sHeader, sHeader.GetLength(), &nWritten);

			STATSTG statstg;
			hr = pStream->Stat(&statstg, STATFLAG_NONAME);

			LARGE_INTEGER nPos = { 0 , 0 };
			hr = pStream->Seek(nPos, STREAM_SEEK_SET, NULL);

			char szContentEncoding[256] = "";
			hr = pRequest->OnSendRawData(dwContext, pStream, statstg.cbSize.LowPart, TRUE, (signed char*) szContentEncoding, _countof(szContentEncoding));

			const int nDefaultSize = 32768;
			ULARGE_INTEGER nBigDefaultSize = { 0, nDefaultSize };
			pStream->SetSize(nBigDefaultSize);

			while (hr == E_PENDING)
			{
				char szData[nDefaultSize];
				UINT nBytesRead = file.Read(szData, _countof(szData));
				if (nBytesRead == 0)
				{
					break;
				}

				hr = pStream->Seek(nPos, STREAM_SEEK_SET, NULL);								
				hr = pStream->Write(szData, nBytesRead, &nWritten);

				hr = pStream->Seek(nPos, STREAM_SEEK_SET, NULL);				
				hr = pRequest->OnSendRawData(dwContext, pStream, nBytesRead, TRUE, (signed char*) szContentEncoding, _countof(szContentEncoding));
			};
		}
	}

	// TODO: how to handle update of the analysis data when we don't know when the data will be available
	for (size_t i = 0; i < selectedItems.size(); i++)
	{
		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, selectedItems[i]);
		if (pData == NULL)
		{
			continue;
		}

		CPath FullPhysicalPath(pData->m_sPhysicalPath);	
		FullPhysicalPath.Append(pData->m_sFileName);

		bool bStatus = false;				
		for (int j = 0; j < 5; j++)
		{
			bStatus = GetFileHistory(pHistory, FullPhysicalPath, 
				pData->m_nRawSize, pData->m_nCompressedSize, pData->m_nDownloads, pData->m_fRatio, pData->m_stLastDownload);

			if (bStatus == true)
				break;

			::Sleep(10);
		}

		if (bStatus == true)
		{
			UpdateItem(selectedItems[i], *pData);
		}
	}
}

void CWebDirectoryContentsView::OnClearHistory()
{
	CWaitCursor cursor;	

	CComQIPtr<ICompressionHistory> pHistory;
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc != NULL)
	{
		CComPtr<ICompressionRuleManager> pCompressionManager;
		if (pDoc->GetCompressionManager(&pCompressionManager) == S_OK && pCompressionManager != NULL)
		{
			pHistory = pCompressionManager;
		}
	}

	CListCtrl& cListCtrl = GetListCtrl();		

	std::vector<int> selectedItems;
	CListCtrlHelper::GetSelectedItems(cListCtrl, selectedItems);

	for (size_t i = 0; i < selectedItems.size(); i++)
	{
		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, selectedItems[i]);
		if (pData == NULL)
		{
			// TODO: assert here
			continue;
		}

		CPath FullPhysicalPath(pData->m_sPhysicalPath);	
		FullPhysicalPath.Append(pData->m_sFileName);

		// TODO: report error case
		HRESULT hr = pHistory->RemoveEntry(CStringA(FullPhysicalPath));
		if (hr == S_OK)
		{
			ResetHistoryData(*pData);
			UpdateItem(selectedItems[i], *pData);
		}
	}
}

void CWebDirectoryContentsView::OnOpenWithDefaultApplication()
{
	CListCtrl& cListCtrl = GetListCtrl();		

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(cListCtrl);
	if (pData == NULL)
	{
		// TODO: assert here
		return;
	}	

	CPath path(pData->m_sPhysicalPath);
	path.Append(pData->m_sFileName);

	CString sQuotedPath;
	sQuotedPath.Format(_T("\"%s\""), path);

	::ShellExecute(NULL, _T("open"), sQuotedPath, NULL, pData->m_sPhysicalPath, SW_SHOW);
}

void CWebDirectoryContentsView::OnOpenWithNotepad()
{
	CListCtrl& cListCtrl = GetListCtrl();		

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(cListCtrl);
	if (pData == NULL)
	{
		// TODO: assert here
		return;
	}

	TCHAR szWinDir[2048];
	::GetWindowsDirectory(szWinDir, _countof(szWinDir));

	CPath notepadPath(szWinDir);
	notepadPath.Append(_T("notepad.exe"));

	CPath path(pData->m_sPhysicalPath);
	path.Append(pData->m_sFileName);

	CString sQuotedPath;
	sQuotedPath.Format(_T("\"%s\""), path);

	::ShellExecute(NULL, _T("open"), notepadPath, sQuotedPath, pData->m_sPhysicalPath, SW_SHOW);
}

void CWebDirectoryContentsView::OnHeaderContextMenu(CWnd* pWnd, CPoint pos)
{
	std::map<int, int> ColumnMap;
	GetColumnIndexMap(ColumnMap);

	CMenu* pSubMenu = m_HeaderPopupMenu.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;	

	int nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_FILESIZE) == ColumnMap.end())
	{
		nCheck = 0;		
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_FILESIZE, nCheck);
	
	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPRESS) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_COMPRESS, nCheck);

	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_ORIGSIZE) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_ORIGINALSIZE, nCheck);

	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPSIZE) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_COMPRESSEDSIZE, nCheck);

	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPRATIO) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_RATIO, nCheck);
	
	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_CONTENTTYPE) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_CONTENTTYPE, nCheck);

	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_COMPRESPONSES) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_RESPONSES, nCheck);

	nCheck = MF_CHECKED;
	if (ColumnMap.find(IDS_FILEVIEW_COLUMN_LASTDOWNLOAD) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(pSubMenu, ID_WEBDIRECTORYHEADERCONTEXT_LASTDOWNLOAD, nCheck);

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 		
			pos.x, pos.y, this);
}

void CWebDirectoryContentsView::OnListContextMenu(CWnd* pWnd, CPoint pos)
{
	CListCtrl& cListCtrl = GetListCtrl();

	// store the original position
	CPoint menuPos = pos;

	// get the list control window rect
	CRect rcHistory;
	cListCtrl.GetWindowRect(rcHistory);	

	// get the mouse pos
	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	// get the selected item
	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);

	// convert the incoming pos to something useful if it came from the keyboard
	if (pos.x < 0 && pos.y < 0)
	{						
		if (nSelectedItem >= 0)
		{
			// get the selected item's rect
			CRect rcItem;
			cListCtrl.GetItemRect(nSelectedItem, &rcItem, LVIR_LABEL);

			// map the selected item's rect to screen coords
			CPoint ptItem(rcItem.CenterPoint());
			cListCtrl.ClientToScreen(&ptItem);
			pos = ptItem;
		}		
	}	

	CMenu* pSubMenu = m_ListPopupMenu.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;		

	CPoint ptClient(pos);
	cListCtrl.ScreenToClient(&ptClient);	

	if (pos.x < 0 && pos.y < 0)
	{
		// there isn't a selection, just pick the top left
		pos = rcHistory.TopLeft();
	}

	// if the cursor is inside the list control rect then just display at that position
	if (rcHistory.PtInRect(ptCursor) == TRUE)
	{
		pos = ptCursor;
	}
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 			
		pos.x, pos.y, ::AfxGetMainWnd());
}

void CWebDirectoryContentsView::CreateColumns()
{
	CreateColumn(IDS_FILEVIEW_COLUMN_FILENAME);
	CreateColumn(IDS_FILEVIEW_COLUMN_COMPRESS);
	CreateColumn(IDS_FILEVIEW_COLUMN_CONTENTTYPE);	
	CreateColumn(IDS_FILEVIEW_COLUMN_FILESIZE);
	CreateColumn(IDS_FILEVIEW_COLUMN_ORIGSIZE);
	CreateColumn(IDS_FILEVIEW_COLUMN_COMPSIZE);
	CreateColumn(IDS_FILEVIEW_COLUMN_COMPRATIO);	
}

void CWebDirectoryContentsView::CreateColumn(int nId)
{
	int columnOrder[] = { 
		IDS_FILEVIEW_COLUMN_FILENAME, IDS_FILEVIEW_COLUMN_COMPRESS, IDS_FILEVIEW_COLUMN_CONTENTTYPE, 
		IDS_FILEVIEW_COLUMN_FILESIZE, IDS_FILEVIEW_COLUMN_ORIGSIZE, IDS_FILEVIEW_COLUMN_COMPSIZE, 
		IDS_FILEVIEW_COLUMN_COMPRATIO, IDS_FILEVIEW_COLUMN_COMPRESPONSES, IDS_FILEVIEW_COLUMN_LASTDOWNLOAD };
	
	int nFormat = 0;
	int nWidth = 0;
	WebDirectoryContentsViewRowData::SortMethod* pfnSort = NULL;

	switch (nId)
	{
	case IDS_FILEVIEW_COLUMN_FILENAME: nFormat = LVCFMT_LEFT; nWidth = 200; pfnSort = &WebDirectoryContentsViewRowData::SortByFileName; break;
	case IDS_FILEVIEW_COLUMN_COMPRESS: nFormat = LVCFMT_LEFT; nWidth = 80; pfnSort = &WebDirectoryContentsViewRowData::SortByCompress; break;
	case IDS_FILEVIEW_COLUMN_FILESIZE: nFormat = LVCFMT_RIGHT; nWidth = 90; pfnSort = &WebDirectoryContentsViewRowData::SortByFileSize; break;
	case IDS_FILEVIEW_COLUMN_ORIGSIZE: nFormat = LVCFMT_RIGHT; nWidth = 90; pfnSort = &WebDirectoryContentsViewRowData::SortByRawSize; break;
	case IDS_FILEVIEW_COLUMN_COMPSIZE: nFormat = LVCFMT_RIGHT; nWidth = 90; pfnSort = &WebDirectoryContentsViewRowData::SortByCompressedSize; break;
	case IDS_FILEVIEW_COLUMN_COMPRATIO: nFormat = LVCFMT_RIGHT; nWidth = 60; pfnSort = &WebDirectoryContentsViewRowData::SortByRatio; break;
	case IDS_FILEVIEW_COLUMN_CONTENTTYPE: nFormat = LVCFMT_LEFT; nWidth = 100; pfnSort = &WebDirectoryContentsViewRowData::SortByContentType; break;
	case IDS_FILEVIEW_COLUMN_COMPRESPONSES: nFormat = LVCFMT_RIGHT; nWidth = 80; pfnSort = &WebDirectoryContentsViewRowData::SortByDownloads; break;
	case IDS_FILEVIEW_COLUMN_LASTDOWNLOAD: nFormat = LVCFMT_LEFT; nWidth = 120; pfnSort = &WebDirectoryContentsViewRowData::SortByLastDownload; break;
	}

	int nColumn = 0;
	for (int i = 0; i < _countof(columnOrder); i++)
	{
		if (nId == columnOrder[i])
		{
			break;
		}

		if (IsColumnVisible(columnOrder[i]) == TRUE)
		{
			++nColumn;
		}
	}

	if (nColumn >= 0 && pfnSort != NULL)
	{
		InsertColumn(nColumn, nId, nFormat, nWidth, *pfnSort);
	}
}

BOOL CWebDirectoryContentsView::IsColumnVisible(int nId)
{
	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader == NULL)
	{
		return FALSE;
	}

	int nColumns = pcHeader->GetItemCount();
	for (int i = 0; i < nColumns; i++)
	{
		HDITEM item;
		item.mask = HDI_LPARAM;
		pcHeader->GetItem(i, &item);

		if (nId == item.lParam)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CWebDirectoryContentsView::ToggleColumn(int nId)
{
	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader == NULL)
	{
		return;
	}

	std::map<int, int> ColumnMap;
	GetColumnIndexMap(ColumnMap);

	std::map<int, int>::const_iterator iter = ColumnMap.find(nId);
	if (iter == ColumnMap.end())
	{				
		CreateColumn(nId);
		
		int nItems = cListCtrl.GetItemCount();
		for (int i = 0; i < nItems; i++)
		{
			WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, i);
			if (pData != NULL)
			{
				UpdateItem(i, *pData);
			}
		}
	} 
	else	
	{
		cListCtrl.DeleteColumn(iter->second);
	}	
}

void CWebDirectoryContentsView::OnShowHideColumnFileSize()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_FILESIZE);
}

void CWebDirectoryContentsView::OnShowHideColumnOrigSize()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_ORIGSIZE);
}

void CWebDirectoryContentsView::OnShowHideColumnCompSize()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_COMPSIZE);
}

void CWebDirectoryContentsView::OnShowHideColumnRatio()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_COMPRATIO);
}

void CWebDirectoryContentsView::OnShowHideColumnResponses()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_COMPRESPONSES);
}

void CWebDirectoryContentsView::OnShowHideColumnLastDown()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_LASTDOWNLOAD);
}

void CWebDirectoryContentsView::OnShowHideColumnCompress()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_COMPRESS);
}

void CWebDirectoryContentsView::OnShowHideColumnContentType()
{
	ToggleColumn(IDS_FILEVIEW_COLUMN_CONTENTTYPE);
}

void CWebDirectoryContentsView::OnCompressAutomatic()
{
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<IStaticFileRules> pStaticRules = pCompressionManager;
	if (pStaticRules == NULL)
		return;

	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();

	while (1)
	{
		int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
		if (nSelectedItem < 0)
			break;

		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
		if (pData == NULL)
			continue;

		CPath fullPath(pData->m_sPhysicalPath);
		fullPath.Append(pData->m_sFileName);

		int nUserCompress = -1;
		int nSystemCompress = -1;
		HRESULT hr = pStaticRules->GetFileRule(CStringA(fullPath), &nUserCompress, &nSystemCompress);

		nUserCompress = -1;

		if (hr == S_OK)
		{
			hr = pStaticRules->SetFileRule(CStringA(fullPath), nUserCompress, nSystemCompress);
		}
		else
		{
			hr = pStaticRules->AddFileRule(CStringA(fullPath), nUserCompress, nSystemCompress);
		}

		if (hr == S_OK)
		{
			pData->m_CompressState = WebDirectoryContentsViewRowData::Automatic;
			UpdateItem(nSelectedItem, *pData);
		}
		else
		{		
			// TODO: move outside the loop
			::AfxMessageBox(IDS_ERRORFILECOMPRESSIONRULE, MB_ICONEXCLAMATION);
		}
	}
}

void CWebDirectoryContentsView::OnCompressAlways()
{
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<IStaticFileRules> pStaticRules = pCompressionManager;
	if (pStaticRules == NULL)
		return;

	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();

	while (1)
	{
		int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
		if (nSelectedItem < 0)
			break;

		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
		if (pData == NULL)
			continue;					

		CPath fullPath(pData->m_sPhysicalPath);
		fullPath.Append(pData->m_sFileName);

		int nUserCompress = -1;
		int nSystemCompress = -1;
		HRESULT hr = pStaticRules->GetFileRule(CStringA(fullPath), &nUserCompress, &nSystemCompress);

		nUserCompress = 1;

		if (hr == S_OK)
		{
			hr = pStaticRules->SetFileRule(CStringA(fullPath), nUserCompress, nSystemCompress);
		}
		else
		{
			hr = pStaticRules->AddFileRule(CStringA(fullPath), nUserCompress, nSystemCompress);
		}

		if (hr == S_OK)
		{
			pData->m_CompressState = WebDirectoryContentsViewRowData::Always;
			UpdateItem(nSelectedItem, *pData);
		}
		else
		{	
			// TODO: move outside the loop
			::AfxMessageBox(IDS_ERRORFILECOMPRESSIONRULE, MB_ICONEXCLAMATION);
		}
	}
}

void CWebDirectoryContentsView::OnCompressNever()
{
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<IStaticFileRules> pStaticRules = pCompressionManager;
	if (pStaticRules == NULL)
		return;

	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();

	while (1)
	{
		int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
		if (nSelectedItem < 0)
			break;

		WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
		if (pData == NULL)
			continue;			

		CPath fullPath(pData->m_sPhysicalPath);
		fullPath.Append(pData->m_sFileName);

		int nUserCompress = -1;
		int nSystemCompress = -1;
		HRESULT hr = pStaticRules->GetFileRule(CStringA(fullPath), &nUserCompress, &nSystemCompress);

		nUserCompress = 0;

		if (hr == S_OK)
		{
			hr = pStaticRules->SetFileRule(CStringA(fullPath), nUserCompress, nSystemCompress);
		}
		else
		{
			hr = pStaticRules->AddFileRule(CStringA(fullPath), nUserCompress, nSystemCompress);
		}
		
		if (hr == S_OK)
		{
			pData->m_CompressState = WebDirectoryContentsViewRowData::Never;
			UpdateItem(nSelectedItem, *pData);
		}
		else
		{	
			// TODO: move outside the loop
			::AfxMessageBox(IDS_ERRORFILECOMPRESSIONRULE, MB_ICONEXCLAMATION);
		}
	}
}

void CWebDirectoryContentsView::OnExcludeExtension()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;	

	CPathT<CString> path(pData->m_sFileName);
	CString sExtension = path.GetExtension();
	if (sExtension.GetLength() <= 0)
		return;

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddExtensionRule(CStringA(sExtension), "", "", 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_EXTENSIONEXCLUSIONADDED, sExtension);

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CWebDirectoryContentsView::OnExcludeContentType()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;			

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddContentTypeRule(CStringA(pData->m_sContentType), "", "", 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_CONTENTTYPEEXCLUSIONADDED, pData->m_sContentType);

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CWebDirectoryContentsView::OnExcludeMatchingContentTypes()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;			

	CString sMatchingCT = pData->m_sContentType;

	int nFirstSlash = sMatchingCT.Find('/');
	if (nFirstSlash < 0)
		return;

	nFirstSlash++;

	sMatchingCT = sMatchingCT.Left(nFirstSlash);
	sMatchingCT += '*';

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddContentTypeRule(CStringA(sMatchingCT), "", "", 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_CONTENTTYPEEXCLUSIONADDED, sMatchingCT);

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CWebDirectoryContentsView::OnViewRefresh()
{
	PopulateFromDirectory(m_sPhysicalPath);
}

void CWebDirectoryContentsView::OnCmdUIAlwaysEnabled(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CWebDirectoryContentsView::OnEditSelectAll()
{
	CListCtrl& cListCtrl = GetListCtrl();

	int nItems = cListCtrl.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		cListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CWebDirectoryContentsView::OnBrowseInternetExplorer()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;

	CString sURL;
	FormatURL(m_Ports, m_SecurePorts, m_sURI, pData->m_sFileName, sURL);	
	
	if (CBrowserHelper::LaunchIE(sURL) == false)
	{
		::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
	}
}

void CWebDirectoryContentsView::OnBrowseFirefox()
{	
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;

	CString sURL;
	FormatURL(m_Ports, m_SecurePorts, m_sURI, pData->m_sFileName, sURL);	
	
	if (CBrowserHelper::LaunchFirefox(sURL) == false)
	{		
		::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);	
	}
}

void CWebDirectoryContentsView::OnBrowseOpera()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;

	CString sURL;
	FormatURL(m_Ports, m_SecurePorts, m_sURI, pData->m_sFileName, sURL);		
	
	if (CBrowserHelper::LaunchOpera(sURL) == false)
	{
		::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
	}
}

void CWebDirectoryContentsView::OnBrowseSafari()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetItemData<WebDirectoryContentsViewRowData>(cListCtrl, nSelectedItem);
	if (pData == NULL)
		return;

	CString sURL;
	FormatURL(m_Ports, m_SecurePorts, m_sURI, pData->m_sFileName, sURL);		
	
	if (CBrowserHelper::LaunchSafari(sURL) == false)
	{
		::AfxMessageBox(IDS_ERROR_LAUNCHINGBROWSER, MB_OK | MB_ICONWARNING);
	}
}

bool CWebDirectoryContentsView::FormatURL(const CAtlArray<IISWebSiteBindings>& Ports, const CAtlArray<IISWebSiteBindings>& SecurePorts, LPCTSTR pszURI, LPCTSTR pszFilename, CString& sURL)
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

void CWebDirectoryContentsView::OnAnalyzeZLIB()
{	
	// get the custom compression interface
	CComQIPtr<ICustomCompressionMode> pCustomCompressionMode;
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc != NULL)
	{		
		CComPtr<ICompressionRuleManager> pCompressionManager;
		if (pDoc->GetCompressionManager(&pCompressionManager) == S_OK && pCompressionManager != NULL)
		{
			pCustomCompressionMode = pCompressionManager;			
		}		
	}

	CListCtrl& cListCtrl = GetListCtrl();

	std::vector<WebDirectoryContentsViewRowData*> files;
	CListCtrlHelper::GetSelectedItemsData(cListCtrl, files);

	if (files.size() > 0)
	{
		CCompressionOptimizerDlg dlg;
		dlg.SetFiles(files);
		dlg.SetCustomCompressionInterface(pCustomCompressionMode);
		dlg.DoModal();
	}
}

void CWebDirectoryContentsView::OnCmdUIToolbarIE(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nSelectedItems = cListCtrl.GetSelectedCount();
	if (nSelectedItems == 1)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUIToolbarFirefox(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nSelectedItems = cListCtrl.GetSelectedCount();
	if (nSelectedItems == 1 && CBrowserHelper::IsFirefoxInstalled() == true)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUIToolbarOpera(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nSelectedItems = cListCtrl.GetSelectedCount();
	if (nSelectedItems == 1 && CBrowserHelper::IsOperaInstalled() == true)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUIToolbarSafari(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nSelectedItems = cListCtrl.GetSelectedCount();
	if (nSelectedItems == 1 && CBrowserHelper::IsSafariInstalled() == true)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUIListViewHasItems(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nItems = (int) cListCtrl.GetItemCount();
	if (nItems > 0)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUIListViewHasSelectedItem(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	UINT nSelected = cListCtrl.GetSelectedCount();
	if (nSelected == 1)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUIListViewHasSelectedItems(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	UINT nSelected = cListCtrl.GetSelectedCount();
	if (nSelected > 0)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CWebDirectoryContentsView::OnCmdUICompressionMode(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();

	std::vector<WebDirectoryContentsViewRowData*> selectedItemData;
	CListCtrlHelper::GetSelectedItemsData(cListCtrl, selectedItemData);	

	size_t nSelectedItems = selectedItemData.size();
	if (nSelectedItems >= 1)
	{
		int nCheck = 0;

		pCmdUI->Enable(TRUE);

		if (nSelectedItems == 1)
		{
			if (selectedItemData[0]->m_CompressState == WebDirectoryContentsViewRowData::Automatic &&
				pCmdUI->m_nID == ID_WEBDIRCONTEXT_COMPRESSAUTOMATIC)
			{
				nCheck = 1;
			}
			else if (selectedItemData[0]->m_CompressState == WebDirectoryContentsViewRowData::Always &&
				pCmdUI->m_nID == ID_WEBDIRCONTEXT_COMPRESSALWAYS)
			{
				nCheck = 1;
			}
			else if (selectedItemData[0]->m_CompressState == WebDirectoryContentsViewRowData::Never &&
				pCmdUI->m_nID == ID_WEBDIRCONTEXT_COMPRESSNEVER)
			{
				nCheck = 1;
			}
		}

		pCmdUI->SetCheck(nCheck);
	}
	else
	{
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(FALSE);
	}
}

void CWebDirectoryContentsView::OnCmdUIAnalyzeCSS(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	CListCtrl& cListCtrl = GetListCtrl();
	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(cListCtrl);
	if (pData != NULL)
	{
		CString sFileName(pData->m_sFileName);
		sFileName.MakeLower();

		int nLength = sFileName.GetLength();
		if (nLength >= 5 && sFileName.Find(_T(".css"), nLength - 4) >= 0)
		{
			bEnable = TRUE;
		}
	}	

	pCmdUI->Enable(bEnable);
}

// TODO: need to implement this properly
void CWebDirectoryContentsView::OnAnalyzeCSS()
{
	CListCtrl& cListCtrl = GetListCtrl();
	WebDirectoryContentsViewRowData* pData = CListCtrlHelper::GetSelectedItemData<WebDirectoryContentsViewRowData>(cListCtrl);
	if (pData == NULL)
	{
		return;
	}

	CPath filePath(pData->m_sPhysicalPath);
	filePath.Append(pData->m_sFileName);

	CFile file;
	if (file.Open(filePath, CFile::modeRead | CFile::shareDenyWrite) != TRUE)
	{
		// TODO: handle the error condition here
		return;
	}

	CFileStatus fileStatus;
	if (file.GetStatus(fileStatus) != TRUE)
	{
		// TODO: handle the error condition here
		return;
	}

	size_t fileSize = (size_t) fileStatus.m_size;	
	std::auto_ptr<BYTE> pbyFileData(new BYTE[fileSize + 1]);
	if (pbyFileData.get() == NULL)
	{
		// TODO: handle the error condition here
		return;
	}

	file.Read(pbyFileData.get(), (UINT) fileSize);
	pbyFileData.get()[fileSize] = '\0';

	CComPtr<IXMLHTTPRequest> pXMLRequest;
	// TODO: determine the best version of MSXML to call
	//HRESULT hr = pXMLRequest.CoCreateInstance(L"Microsoft.XMLHTTP");
	HRESULT hr = pXMLRequest.CoCreateInstance(L"Msxml2.XMLHTTP.6.0");
	if (hr != S_OK)
	{
		return;
	}

	CStringA sValidatorURL = 
		//L"http://jigsaw.w3.org/css-validator/validator?"
		"http://www.ripcordsoftware.com:11491/css-validator/validator";

	CComBSTR bsValidatorURL(sValidatorURL);		

	hr = pXMLRequest->open(L"POST", bsValidatorURL, CComVariant(0), CComVariant(L""), CComVariant(L""));	

	CStringA sBody =
		"-----------------------------7d8f1d103c2\r\n"
		"Content-Disposition: form-data; name=\"output\"\r\n\r\n"
		"soap12\r\n"
		"-----------------------------7d8f1d103c2\r\n"
		"Content-Disposition: form-data; name=\"file\"; filename=\"C:\\Inetpub\\wwwroot\\default.css\"\r\n"
		"Content-Type: text/css\r\n\r\n";

	sBody += (const char*) pbyFileData.get();

	sBody += "\r\n-----------------------------7d8f1d103c2--";

	hr = pXMLRequest->setRequestHeader(L"Content-Type", L"multipart/form-data; boundary=---------------------------7d8f1d103c2");

	hr = pXMLRequest->send(CComVariant(sBody));	

	CComBSTR bsText;
	hr = pXMLRequest->get_responseText(&bsText);

	::OutputDebugStringW(bsText);
	::OutputDebugStringW(L"\n");

	CComPtr<IDispatch> pDisp;
	hr = pXMLRequest->get_responseXML(&pDisp);

	CComQIPtr<IXMLDOMDocument> pDOM = pDisp;

	CComQIPtr<IXMLDOMDocument2> pDOM2 = pDOM;
	hr = pDOM2->setProperty(CComBSTR(L"SelectionLanguage"), CComVariant(L"XPath"));
	hr = pDOM2->setProperty(CComBSTR(L"SelectionNamespaces"), 
		CComVariant(L"xmlns:env='http://www.w3.org/2003/05/soap-envelope' xmlns:m='http://www.w3.org/2005/07/css-validator'"));	

	CComPtr<IXMLDOMNode> pErrorCountNode;
	hr = pDOM2->selectSingleNode(L"/env:Envelope/env:Body/m:cssvalidationresponse/m:result/m:errors/m:errorcount", &pErrorCountNode);	
}