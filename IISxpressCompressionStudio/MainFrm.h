// MainFrm.h : interface of the CMainFrame class
//
#pragma once

#include "LocateIPToolbarDlg.h"
#include "BrowsersToolbarDlg.h"
#include "ExclusionRuleToolbar.h"

#define MAINFRAME_TIMER_ID				1234
#define MAINFRAME_TIMER_INTERVAL		2000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	DECLARE_MENUXP()

// Attributes
protected:
	CFixedSplitterWnd m_wndSplitter;

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	//CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	//CDialogBar  m_wndDlgBar;
	CLocateIPToolbarDlg m_wndLocateIPDlg;
	CBrowsersToolbarDlg m_wndBrowserDlg;
	CExclusionRuleToolbar m_wndExclusionRuleToolbar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLocateIP();
	afx_msg void OnAdminTool(void);

	afx_msg void OnToolbarsLocate();
	afx_msg void OnUpdateUIToolbarsLocate(CCmdUI *pCmdUI);

	afx_msg void OnToolbarsBrowsers();
	afx_msg void OnUpdateUIToolbarsBrowsers(CCmdUI *pCmdUI);

	afx_msg void OnToolbarsExclusionRules();
	afx_msg void OnUpdateUIToolbarsExclusionRules(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()

private:

	void UpdateStatusBarActivity(void);

	bool ConnectToServer();
	void StartTimer();
	void StopTimer();

	void RestoreWindowPosition();
	void SaveWindowPosition();
	void MakeDisplayCookie(CString& sCookie);

	bool	m_bTimerRunning;
	DWORD	m_dwLastTimerEvent;

	CString		m_sKBUnits;	
	CString		m_sMBUnits;	
	
};