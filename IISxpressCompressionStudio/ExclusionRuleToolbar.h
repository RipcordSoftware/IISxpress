#pragma once

#include "VS2005ToolbarButton.h"

// CExclusionRuleToolbar dialog

class CExclusionRuleToolbar : public CDialogBar
{
	DECLARE_DYNAMIC(CExclusionRuleToolbar)

public:
	CExclusionRuleToolbar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExclusionRuleToolbar();

// Dialog Data
	enum { IDD = IDD_TOOLBAR_EXCLUSIONRULES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT HandleInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:

	HTHEME m_hTheme;

	CVS2005ToolbarButton	m_cExcludeExtn;
	CVS2005ToolbarButton	m_cExcludeCT;
	CVS2005ToolbarButton	m_cExcludeMatchedCT;
	CVS2005ToolbarButton	m_cExcludeDir;
	CVS2005ToolbarButton	m_cExcludeDirTree;
	CVS2005ToolbarButton	m_cExcludeWebSite;
	CVS2005ToolbarButton	m_cExcludeClientIP;
	CVS2005ToolbarButton	m_cExcludeNetworkIP;
};
