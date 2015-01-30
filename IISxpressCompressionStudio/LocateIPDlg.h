#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CLocateIPDlg dialog

class CLocateIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CLocateIPDlg)

public:
	CLocateIPDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLocateIPDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOCATEIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonLocate();

	DECLARE_MESSAGE_MAP()	

private:
	CIPAddressCtrl	m_cIPAddress;
	CEdit			m_cLocation;
};
