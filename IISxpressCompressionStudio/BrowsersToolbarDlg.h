#pragma once

#include "VS2005ToolbarButton.h"

// CBrowsersToolbarDlg dialog

class CBrowsersToolbarDlg : public CDialogBar
{
	typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hwnd, HDC hdc, RECT *prc);

	DECLARE_DYNAMIC(CBrowsersToolbarDlg)

public:
	CBrowsersToolbarDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrowsersToolbarDlg();

// Dialog Data
	enum { IDD = IDD_TOOLBAR_BROWSERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual LRESULT HandleInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	

private:

	HTHEME m_hTheme;

	CVS2005ToolbarButton	m_cIE;
	CVS2005ToolbarButton	m_cFirefox;
	CVS2005ToolbarButton	m_cOpera;
	CVS2005ToolbarButton	m_cSafari;
};
