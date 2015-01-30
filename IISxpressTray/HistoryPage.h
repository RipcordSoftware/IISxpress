#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "HTTPNotifyHelper.h"

#include "ColouredListCtrl.h"
#include "MenuXP\MenuXP.h"

#include "FormattedTimeStringCache.h"
#include "FormattedNumberStringCache.h"

#include <map>

#define HISTORYWINDOW_MAXSIZE		1000
#define HISTORYWINDOW_DELETEBLOCK	100

// CHistoryPage dialog

class CHistoryPage : public CIISxpressPageBase, public CHTTPNotifyBase
{
	DECLARE_DYNAMIC(CHistoryPage)

public:
	CHistoryPage();
	virtual ~CHistoryPage();

// Dialog Data
	enum { IDD = IDD_PAGE_HISTORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void OnDestroy(void);	
	void OnLvnItemChangedListHistory(NMHDR *pNMHDR, LRESULT *pResult);
	void OnContextMenu(CWnd* pWnd, CPoint pos);
	void OnHistoryContextAutoScroll();
	void OnHistoryContextClear();
	void OnHistoryContextExcludeExtn();
	void OnHistoryContextExcludeCT();
	void OnHistoryContextExcludeMatchingCT();
	void OnHistoryContextExcludeDirectory();
	void OnHistoryContextExcludeDirectoryTree();
	void OnHistoryContextExcludeWebSite();
	void OnHistoryContextExcludeClient();
	void OnHistoryContextExcludeSubnet();
	void OnHistoryContextEnableHistory();

	DECLARE_MESSAGE_MAP()

	DECLARE_MENUXP()

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

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	void FreeEntries(void);
	void ShrinkEntries(int nLeave = HISTORYWINDOW_MAXSIZE, int nDeleteBlock = HISTORYWINDOW_DELETEBLOCK);

	bool CheckMenuItem(CMenu* pMenu, UINT nID, UINT nCheck);
	bool EnableMenuItem(CMenu* pMenu, UINT nID, UINT nEnable);

	void UpdateDetails(void);

	bool RegisterConnectionPoint(void);
	bool UnregisterConnectionPoint(void);

	static void OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult);

	static COLORREF GetAlternateCacheLineColour(COLORREF color);

	void FreeActiveResponses();

	volatile bool						m_bAutoScroll;

	CComPtr<IConnectionPoint>							m_pHTTPNotifyCP;
	CComObject<CHTTPNotify <CHTTPNotifyBase> >*			m_pNotify;
	DWORD												m_dwCookie;

	CColouredListCtrl					m_cHistory;
	CEdit								m_cHostname;
	CEdit								m_cClientIP;
	CEdit								m_cLocation;
	CEdit								m_cContentType;
	CEdit								m_cClientBrowser;
	CEdit								m_cURI;
	CEdit								m_cQueryString;

	CComAutoCriticalSection				m_csActiveResponses;
	typedef std::map<DWORD, CIISResponseData*> ActiveResponseColl;
	ActiveResponseColl	m_ActiveResponses;
	
	CToolTipCtrl						m_cTips;

	CMenu								m_PopupMenu;
	CImage								m_bmpFolderOpen;
	CImage								m_bmpMixedMedia;
	CImage								m_bmpNetworkGroup;
	CImage								m_bmpIDRDLL;

	CFormattedTimeStringCache			m_TimeStringCache;
	CFormattedNumberStringCache			m_NumberStringCache;

	BOOL								m_bEnabled;
};
