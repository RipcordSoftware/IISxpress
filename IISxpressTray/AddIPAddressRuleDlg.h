#pragma once

#include "HelpAwareDialogBase.h"

// CAddIPAddressRuleDlg dialog

class CAddIPAddressRuleDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CAddIPAddressRuleDlg)

public:
	CAddIPAddressRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddIPAddressRuleDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDIPRULE };

	CString		m_sNewIPAddress;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnOK(void);

	DECLARE_MESSAGE_MAP()

private:

	CEdit m_cIPAddress;
};
