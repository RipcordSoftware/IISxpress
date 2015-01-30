#pragma once

#include "resource.h"

#include "HelpAwareDialogBase.h"

// CAddExtensionRuleDlg dialog

class CAddExtensionRuleDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CAddExtensionRuleDlg)

public:
	CAddExtensionRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddExtensionRuleDlg();

	void Init(const CStringArray& saExcludedExtensions);

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDEXTENSIONRULE };

	CString		m_sNewExtension;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnOK(void);

	DECLARE_MESSAGE_MAP()

private:

	CStringArray	m_saExcludedExtensions;

	CEdit			m_cExtension;
};
