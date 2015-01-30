#pragma once
#include "afxwin.h"


// CAddUserAgentRuleDlg dialog

class CAddUserAgentRuleDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CAddUserAgentRuleDlg)

public:
	CAddUserAgentRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddUserAgentRuleDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDUSERAGENTRULE };

	CString selectedUserAgent;

protected:
	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedRadioUAPredefined();
	afx_msg void OnBnClickedRadioUACustom();
	afx_msg void OnCbnSelchangeComboUAPredefined();
	afx_msg void OnCbnSelchangeComboUACustom();
	afx_msg void OnBnClickedCheckUAInclude();
	afx_msg void OnCbnEditChangeComboUACustom();
	afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()	

private:

	void UpdateOKButtonState(bool edited);

	CButton m_cPredefinedRadio;
	CButton m_cCustomRadio;
	CComboBox m_cPredefinedCombo;
	CComboBox m_cCustomCombo;
	CButton m_cInclude;	
	CButton m_cOK;
};
