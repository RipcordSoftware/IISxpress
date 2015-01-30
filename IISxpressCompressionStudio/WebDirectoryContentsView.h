#pragma once

#include <algorithm>

#include "ContentTypeFromExtension.h"

#include "WebDirectoryContentsViewRowData.h"

// CWebDirectoryContentsView view

class CWebDirectoryContentsView : 	
	public CAutoSortableHeaderListView<CColouredListView>
{
	DECLARE_DYNCREATE(CWebDirectoryContentsView)

protected:
	CWebDirectoryContentsView();           // protected constructor used by dynamic creation
	virtual ~CWebDirectoryContentsView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	DECLARE_MENUXP()

	DECLARE_HANDLECONTEXTMENUMESSAGES()

	BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);	

	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);	

	void OnInitialUpdate();	

	void OnServerOnline();
	void OnServerOffline();

	afx_msg void OnAnalyze();	
	afx_msg void OnAnalyzeZLIB();	
	afx_msg void OnAnalyzeCSS();	
	afx_msg void OnClearHistory();

	afx_msg void OnCompressAutomatic();
	afx_msg void OnCompressAlways();
	afx_msg void OnCompressNever();

	afx_msg void OnExcludeExtension();
	afx_msg void OnExcludeContentType();
	afx_msg void OnExcludeMatchingContentTypes();

	afx_msg void OnBrowseInternetExplorer();
	afx_msg void OnBrowseFirefox();
	afx_msg void OnBrowseOpera();
	afx_msg void OnBrowseSafari();

	afx_msg void OnExploreFolder();
	afx_msg void OnWriteToCSV();
	afx_msg void OnOpenWithDefaultApplication();
	afx_msg void OnOpenWithNotepad();

	afx_msg void OnShowHideColumnFileSize();
	afx_msg void OnShowHideColumnCompress();
	afx_msg void OnShowHideColumnContentType();
	afx_msg void OnShowHideColumnOrigSize();
	afx_msg void OnShowHideColumnCompSize();
	afx_msg void OnShowHideColumnRatio();	
	afx_msg void OnShowHideColumnResponses();
	afx_msg void OnShowHideColumnLastDown();

	afx_msg void OnViewRefresh();
	afx_msg void OnEditSelectAll();	

	afx_msg void OnCmdUIToolbarIE(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIToolbarFirefox(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIToolbarOpera(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIToolbarSafari(CCmdUI* pCmdUI);

	afx_msg void OnCmdUIAlwaysEnabled(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIListViewHasItems(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIListViewHasSelectedItem(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIListViewHasSelectedItems(CCmdUI* pCmdUI);

	afx_msg void OnCmdUICompressionMode(CCmdUI* pCmdUI);

	afx_msg void OnCmdUIAnalyzeCSS(CCmdUI* pCmdUI);

private:	

	void OnHeaderContextMenu(CWnd* pWnd, CPoint pos);
	void OnListContextMenu(CWnd* pWnd, CPoint pos);
	
	void InsertColumn(int nColumn, int nId, int nFormat, int nWidth, WebDirectoryContentsViewRowData::SortMethod fnSort);
	int GetColumnId(int nColumn);
	void GetColumnIndexMap(std::map<int, int>& indexes);
	void CreateColumns();
	void CreateColumn(int nId);
	void ToggleColumn(int nId);	
	BOOL IsColumnVisible(int nId);

	void FreeItemData();
	void ResetHistoryData(WebDirectoryContentsViewRowData& data);

	void OnSortColumn(int nColumn, BOOL bAscending);
	void ApplySort(int nColumn, BOOL bAscending);
	void ApplySort();

	void PopulateFromDirectory(LPCWSTR pszDir);
	int InsertItem(int nPosition, LPCWSTR pszDir, LPCWSTR pszFilename, __int64 nFileSize, ICompressionHistory* pHistory, IStaticFileRules* pStaticRules);

	bool GetFileHistory(ICompressionHistory* pHistory, LPCWSTR pszFullPath, __int64& nRawSize, __int64& nCompressedSize, __int64& nDownloads, float& fRatio, SYSTEMTIME& stLastDownload);
	void UpdateItem(int nItem, const WebDirectoryContentsViewRowData& data);

	static bool FormatURL(const CAtlArray<IISWebSiteBindings>& Ports, const CAtlArray<IISWebSiteBindings>& SecurePorts, LPCTSTR pszURI, LPCTSTR pszFilename, CString& sURL);

	CMenu						m_HeaderPopupMenu;
	CMenu						m_ListPopupMenu;

	CImage						m_bmpMixedMedia;
	CImage						m_bmpIDRDLL;
	CImage						m_bmpIE;
	CImage						m_bmpFolderOpen;
	
	std::map<int, WebDirectoryContentsViewRowData::SortMethod*> m_ColumnSortMethodMap;

	CAtlArray<IISWebSiteBindings>	m_Ports;
	CAtlArray<IISWebSiteBindings>	m_SecurePorts;
	CString							m_sPhysicalPath;
	CString							m_sURI;

	CContentTypeFromExtension	m_ContentTypes;

	CString						m_sCompressStateAuto;
	CString						m_sCompressStateAlways;
	CString						m_sCompressStateNever;
};

