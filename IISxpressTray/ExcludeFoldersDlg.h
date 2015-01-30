#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "iadmw.h"

#include "ColouredListCtrl.h"

#include "CStringCompareNoCase.h"

#include "MenuXP\MenuXP.h"

#include <vector>
#include <set>
using namespace std;

#include "IISConfigHelper.h"

// *************************************************************************************************** //

class CMyRClickTreeCtrl : public CTreeCtrl
{

protected:

	DECLARE_MESSAGE_MAP()	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

// *************************************************************************************************** //

// CExcludeFoldersDlg dialog

class CExcludeFoldersDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CExcludeFoldersDlg)

public:
	CExcludeFoldersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExcludeFoldersDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EXCLUDEFOLDERS };

	void UpdateControls(BOOL bForceDisabled = FALSE);
	void UpdateControls(DWORD dwUpdateCookie);
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	void OnDestroy(void);
	void OnContextMenu(CWnd* pWnd, CPoint pos);
	void OnCustomDraw(NMHDR* pNmHdr, LRESULT* pResult);
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg void OnCbnSelChangeComboInstances();	
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnTvnGetInfoTipTreeExcludedURIs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemExpandingURITree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnExcludeBranch(void);
	afx_msg void OnExcludeFolder(void);
	afx_msg void OnInclude(void);
	afx_msg void OnExplore(void);
	afx_msg void OnBrowse(void);

	DECLARE_MESSAGE_MAP()

	DECLARE_MENUXP()

private:

	void PopulateTree(void);	

	void PopulateExcludedFolders(HTREEITEM htParent = NULL);
	void PopulateExcludedFolders(HTREEITEM htParent, const map<CStringW, DWORD, CStringWCompareNoCase>& ExcludedFolders, bool bForceExclude = false);

	void FreeComboInstanceStrings(void);			

	void FreeTreeItems(void);
	void FreeTreeItems(HTREEITEM htItem);
	
	IISWebSite* GetSelectedComboInstanceData(void);
	bool GetInstanceExclusionFolders(LPCTSTR pszInstance, map<CStringW, DWORD, CStringWCompareNoCase>& Folders);

	bool EnableMenuItem(CMenu* pMenu, UINT nID, UINT nEnable);
	bool CheckMenuItem(CMenu* pMenu, UINT nID, UINT nCheck);		

	int	m_nLastSelectedInstanceItem;	

	CComboBox			m_cInstances;	
	CButton				m_cRefresh;
	CMyRClickTreeCtrl	m_cURITree;

	set<CStringW>			m_IgnoreDirNames;

	CImageList				m_ImageList;
	CBitmap					m_bmpFolder;
	CBitmap					m_bmpFolderExcluded;
	CBitmap					m_bmpVirtualDirectory;
	CBitmap					m_bmpVirtualDirectoryExcluded;
	CBitmap					m_bmpWebServer;	
	CBitmap					m_bmpWebServerExcluded;

	int						m_nFolderIndex;
	int						m_nFolderExcludedIndex;
	int						m_nVirtualDirectoryIndex;
	int						m_nVirtualDirectoryExcludedIndex;
	int						m_nWebServerIndex;	
	int						m_nWebServerExcludedIndex;

	CMenu					m_PopupMenu;		
	CToolTipCtrl			m_cTips;

	CImage					m_bmpMenuFolder;
	CImage					m_bmpMenuBrowse;

	DWORD					m_dwUpdateCookie;
};
