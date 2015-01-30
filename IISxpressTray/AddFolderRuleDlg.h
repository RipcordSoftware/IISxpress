#pragma once

#include "HelpAwareDialogBase.h"


// CAddFolderRuleDlg dialog

class CAddFolderRuleDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CAddFolderRuleDlg)

public:
	CAddFolderRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddFolderRuleDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDFOLDER };

	void Init(const CStringArray& saExcludedFolders);

	CString		m_sFolder;
	DWORD		m_dwFlags;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	void OnOK(void);

	DECLARE_MESSAGE_MAP()

private:

	CStringArray	m_saExcludedFolders;

	CEdit m_cFolder;
	CButton m_cWildcard;
};
