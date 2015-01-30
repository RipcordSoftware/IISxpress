#pragma once

#include "HTTPNotifyBase.h"
#include "HTTPNotify.h"
#include "HTTPNotifyHelper.h"

#include <map>

// CResponseHistoryView view

#define HISTORYWINDOW_MAXSIZE		5000
#define HISTORYWINDOW_DELETEBLOCK	100

class CResponseHistoryView : public CColouredListView, public CHTTPNotifyBase
{
	DECLARE_DYNCREATE(CResponseHistoryView)

protected:
	CResponseHistoryView();           // protected constructor used by dynamic creation
	virtual ~CResponseHistoryView();

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

	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);

	afx_msg void OnClear();
	afx_msg void OnAutoScroll();
	afx_msg void OnWriteToCSV();
	afx_msg void OnExploreContainingFolder();
	afx_msg void OnHistoryContextExcludeExtn();
	afx_msg void OnHistoryContextExcludeCT();
	afx_msg void OnHistoryContextExcludeMatchingCT();
	afx_msg void OnHistoryContextExcludeDirectory();
	afx_msg void OnHistoryContextExcludeDirectoryTree();
	afx_msg void OnHistoryContextExcludeWebSite();
	afx_msg void OnHistoryContextExcludeClient();
	afx_msg void OnHistoryContextExcludeSubnet();
	afx_msg void OnHeaderContextURI();
	afx_msg void OnHeaderContextHostname();
	afx_msg void OnHeaderContextPort();
	afx_msg void OnHeaderContextQueryString();
	afx_msg void OnHeaderContextCached();
	afx_msg void OnHeaderContextOriginalSize();
	afx_msg void OnHeaderContextCompressedSize();
	afx_msg void OnHeaderContextRatio();
	afx_msg void OnHeaderContextContentType();
	afx_msg void OnHeaderContextClientIP();
	afx_msg void OnHeaderContextLocation();
	afx_msg void OnHeaderContextBrowser();
	afx_msg void OnHeaderContextUserAgent();
	afx_msg void OnHeaderContextCompressionTime();

	afx_msg void OnCmdUIClear(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIAutoScroll(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIWriteToCSV(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIListViewHasSelectedItem(CCmdUI* pCmdUI);

	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);	

protected:

	virtual HRESULT OnResponseStart(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwFilterContext);

	virtual HRESULT OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize);

	virtual HRESULT OnResponseEnd(DWORD dwFilterContext);

	virtual HRESULT OnResponseAbort(DWORD dwFilterContext);

	virtual HRESULT OnCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize);

	static void OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult);

	void OnServerOnline();
	void OnServerOffline();

private:

	void OnHeaderContextMenu(CWnd* pWnd, CPoint pos);
	void OnListContextMenu(CWnd* pWnd, CPoint pos);

	void CreateItem(const CIISResponseData* pData);
	void UpdateItem(int nItem, const CIISResponseData& data);

	void InsertColumn(int nColumn, int nId, int nFormat, int nWidth);
	int GetColumnId(int nColumn);
	void GetColumnIndexMap(std::map<int, int>& indexes);
	void CreateColumns();
	void CreateColumn(int nId);
	void ToggleColumn(int nId);	
	BOOL IsColumnVisible(int nId);

	void SetHeaderContextMenuItemState(CMenu& SubMenu, const std::map<int, int>& ColumnMap, int nColumnId, int nCmd);

	void FreeEntries(void);
	void ShrinkEntries(int nLeave = HISTORYWINDOW_MAXSIZE, int nDeleteBlock = HISTORYWINDOW_DELETEBLOCK);

	bool RegisterConnectionPoint(void);
	bool UnregisterConnectionPoint(void);

	volatile bool						m_bAutoScroll;

	CComPtr<IConnectionPoint>							m_pHTTPNotifyCP;
	CComObject<CHTTPNotify <CHTTPNotifyBase> >*			m_pNotify;
	DWORD												m_dwCookie;

	CComAutoCriticalSection				m_csActiveResponses;
	std::map<DWORD, CIISResponseData*>	m_ActiveResponses;	

	CMenu								m_HeaderPopupMenu;
	CMenu								m_ListPopupMenu;
	CImage								m_bmpFolderOpen;
	CImage								m_bmpMixedMedia;
	CImage								m_bmpNetworkGroup;
	CImage								m_bmpIDRDLL;

	CString								m_sCacheStatusYes;
	CString								m_sCacheStatusNo;
};


