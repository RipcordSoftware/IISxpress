#pragma once
#include "afxwin.h"


// CAddExtnWelcomePage dialog

class CAddExtnWelcomePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddExtnWelcomePage)

public:
	CAddExtnWelcomePage();
	virtual ~CAddExtnWelcomePage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDEXTN_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);
	LRESULT OnWizardNext(void);

	DECLARE_MESSAGE_MAP()

private:

	CStatic m_cHeader1;
	CButton m_cAutoSearch;
	CButton m_cManualSearch;
};
