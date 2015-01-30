#pragma once

// CLocateIPToolbarDlg dialog

//class CLocateIPToolbarDlg : public CDialog
class CLocateIPToolbarDlg : public CDialogBar
{
	typedef HRESULT (WINAPI *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hwnd, HDC hdc, RECT *prc);

	DECLARE_DYNAMIC(CLocateIPToolbarDlg)

public:
	CLocateIPToolbarDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLocateIPToolbarDlg();

// Dialog Data
	enum { IDD = IDD_TOOLBAR_LOCATEIP };

protected:
	LRESULT HandleInitDialog(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support		

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonLocate();
	afx_msg void OnCmdUIButtonLocate(CCmdUI* pCmdUI);
	BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:
	CIPAddressCtrl	m_cIPAddress;
	CEdit			m_cLocation;

	HTHEME m_hTheme;

};
