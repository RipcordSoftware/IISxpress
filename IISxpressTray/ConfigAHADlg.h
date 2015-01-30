#pragma once

#include "OwnerDrawStatic.h"
#include "PNGHelper.h"
#include "afxwin.h"

// CConfigAHADlg dialog

class CConfigAHADlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CConfigAHADlg)

public:
	CConfigAHADlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigAHADlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIGAHA };

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

	void UpdateControls(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	BOOL OnInitDialog();

	afx_msg void OnBnClickedCheckEnableAHA();		

	DECLARE_MESSAGE_MAP()

private:

	CIISxpressTrayPropertySheet* GetIISxpressSheet();

	COwnerDrawStatic m_cAHABmp;
	CImage m_imgAHA;

	CButton m_cEnableAHA;	
	CEdit m_cFPGATemp;
	CEdit m_cBoardTemp;
	CEdit m_cBoardDesc;
};
