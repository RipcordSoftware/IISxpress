#pragma once
#include "afxwin.h"


// CConfigAdvancedDlg dialog

class CConfigAdvancedDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CConfigAdvancedDlg)

public:
	CConfigAdvancedDlg(CWnd* pParent =NULL);
	virtual ~CConfigAdvancedDlg();

	void UpdateControls(void);	

	void UpdateAffinityControls();
	void UpdateAffinityState();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIGADVANCED };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	afx_msg void OnBnClickedButtonRepair();	
	afx_msg void OnCbnSelchangeComboFilterLogginglevel();
	afx_msg void OnCbnSelchangeComboIISxpressServerLoggingLevel();	
	afx_msg void OnBnClickedCheckEnableCPU1();
	afx_msg void OnBnClickedCheckEnableCPU2();
	afx_msg void OnBnClickedCheckEnableCPU3();
	afx_msg void OnBnClickedCheckEnableCPU4();
	afx_msg void OnBnClickedCheckEnableCPU5();
	afx_msg void OnBnClickedCheckEnableCPU6();
	afx_msg void OnBnClickedCheckEnableCPU7();
	afx_msg void OnBnClickedCheckEnableCPU8();
	afx_msg void OnBnClickedRadioAffinityModeNone();
	afx_msg void OnBnClickedRadioAffinityModeAuto();
	afx_msg void OnBnClickedRadioAffinityModeManual();

	DECLARE_MESSAGE_MAP()

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	int GetNumberOfSelectedCPUs();

	DWORD		m_dwNumberOfCores;

	CComboBox	m_cFilterLoggingLevel;
	CComboBox	m_cServerLoggingLevel;			
	CButton		m_cAffinityModeNone;
	CButton		m_cAffinityModeAutomatic;
	CButton		m_cAffinityModeManual;
	CButton		m_cCPU1;
	CButton		m_cCPU2;
	CButton		m_cCPU3;
	CButton		m_cCPU4;
	CButton		m_cCPU5;
	CButton		m_cCPU6;
	CButton		m_cCPU7;
	CButton		m_cCPU8;
	
	std::vector<CButton*> m_chkCPUs;		
};
