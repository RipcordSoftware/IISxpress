#pragma once

#include "resource.h"

#include "HelpAwareDialogBase.h"

// CAddContentTypeRuleDlg dialog

class CAddContentTypeRuleDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CAddContentTypeRuleDlg)

public:
	CAddContentTypeRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddContentTypeRuleDlg();

	void Init(const CStringArray& saExcludedContentTypes);

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDCONTENTTYPERULE };

	CString		m_sNewContentType;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnOK(void);

	DECLARE_MESSAGE_MAP()

private:

	CStringArray	m_saExcludedContentTypes;

	CEdit			m_cContentType;
};
