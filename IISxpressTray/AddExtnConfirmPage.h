#pragma once
#include "afxcmn.h"


// CAddExtnConfirmPage dialog

class CAddExtnConfirmPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddExtnConfirmPage)

public:
	CAddExtnConfirmPage();
	virtual ~CAddExtnConfirmPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDEXTN_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);
	LRESULT OnWizardNext(void);

	DECLARE_MESSAGE_MAP()

private:

	CColouredListCtrl m_cExtns;
};
