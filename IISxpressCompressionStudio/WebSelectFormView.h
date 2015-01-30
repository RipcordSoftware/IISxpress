#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "IISConfigHelper.h"

// *************************************************************************************************** //

class CMyRClickTreeCtrl : public CTreeCtrl
{

protected:

	DECLARE_MESSAGE_MAP()	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

// CWebSelectFormView dialog

class CWebSelectFormView : public CFormView
{
	DECLARE_DYNCREATE(CWebSelectFormView)	

public:
	CWebSelectFormView();   // standard constructor
	virtual ~CWebSelectFormView();

// Dialog Data
	enum { IDD = IDD_DIALOG_WEBSITESELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	

	void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()

	DECLARE_MENUXP()

	DECLARE_HANDLECONTEXTMENUMESSAGES()	

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelChangeComboWebSites();
	afx_msg void OnTvnItemExpandingTreeWebSiteDirectories(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelChangedTreeWebSiteDirectories(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);	

	afx_msg void OnExploreFolder();
	afx_msg void OnBrowseInternetExplorer();
	afx_msg void OnBrowseFirefox();
	afx_msg void OnBrowseOpera();
	afx_msg void OnBrowseSafari();

	afx_msg void OnViewRefresh();

	afx_msg void OnCmdUIViewRefresh(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIExploreFolder(CCmdUI* pCmdUI);

	afx_msg void OnCmdUIToolbarIE(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIToolbarFirefox(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIToolbarOpera(CCmdUI* pCmdUI);
	afx_msg void OnCmdUIToolbarSafari(CCmdUI* pCmdUI);

private:

	void PositionChildControls();

	void OnWebTreeContextMenu(CWnd* pWnd, CPoint pos);

	static CView* GetParentView(CWnd*);

	void PopulateTree();
	void PopulateWebSiteCombo();

	IISWebSite* GetSelectedComboInstanceData(void);

	void FreeComboItems();
	void FreeTreeItems();
	void FreeTreeItems(HTREEITEM htItem);

	void SelectWebSiteRoot();

	static bool FormatURL(const CAtlArray<IISWebSiteBindings>& Ports, const CAtlArray<IISWebSiteBindings>& SecurePorts, LPCTSTR pszURI, LPCTSTR pszFilename, CString& sURL);

	CComboBox					m_cWebSites;
	CMyRClickTreeCtrl			m_cWebDirectories;
	CButton						m_cRefresh;
	CStatic						m_cDlgMargins;

	CControlMargins				m_WebSitesMargins;
	CControlMargins				m_nWebDirectoriesMargins;
	CControlMargins				m_nRefreshMargins;

	std::set<CStringW>			m_IgnoreDirNames;

	CImageList					m_ImageList;
	CBitmap						m_bmpFolder;	
	CBitmap						m_bmpVirtualDirectory;	
	CBitmap						m_bmpWebServer;		

	int							m_nFolderIndex;	
	int							m_nVirtualDirectoryIndex;	
	int							m_nWebServerIndex;					

	CMenu						m_WebTreeContext;
	CImage						m_bmpIE;
	CImage						m_bmpFolderOpen;	
};
