#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CExcludeNetworkDlg dialog

class CExcludeNetworkDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CExcludeNetworkDlg)

public:

	CExcludeNetworkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExcludeNetworkDlg();

	void UpdateControls(void);
	void UpdateControls(DWORD dwUpdateCookie);

// Dialog Data
	enum { IDD = IDD_DIALOG_EXCLUDENETWORK };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog(void);
	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedCheckCompressLocalhost();
	afx_msg void OnLvnItemChangedListNetworks(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonNetworkAdd();
	afx_msg void OnBnClickedButtonNetworkDelete();

	DECLARE_MESSAGE_MAP()

private:

	CColouredListCtrl	m_cNetworks;
	CButton				m_cAdd;
	CButton				m_cDelete;
	CButton				m_cCompressLocalhost;

	DWORD				m_dwUpdateCookie;

	HACCEL				m_hAccelerator;

};
