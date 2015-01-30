#pragma once
#include "afxwin.h"


// CConfigGeneralDlg dialog

class CConfigGeneralDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CConfigGeneralDlg)

public:
	CConfigGeneralDlg(CWnd* pParent = NULL);
	virtual ~CConfigGeneralDlg();

	void UpdateControls(void);	

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIGGENERAL };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);	
	afx_msg void OnBnClickedCheckFilterEnable();	
	afx_msg void OnBnClickedButtonIISxpressServerStart();
	afx_msg void OnBnClickedButtonIISxpressServerStop();	
	afx_msg void OnCbnSelchangeComboCompressionMode();
	afx_msg void OnCbnSelchangeComboBZIP2Mode();
	afx_msg void OnBnClickedResetCompressionStats();

	DECLARE_MESSAGE_MAP()

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:
	
	void UpdateFilterState(void);
	void UpdateIISxpressStatus(void);

	CEdit m_cFilterStatus;
	CButton m_cFilterEnable;
	CEdit m_cIISxpressStatus;
	CButton m_cIISxpresStart;
	CButton m_cIISxpressStop;	
	CComboBox m_cCompressionLevel;
	CButton m_cResetCompression;
	CComboBox m_cBZIP2Mode;
};
