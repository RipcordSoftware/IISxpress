#pragma once
#include "afxwin.h"


// CAddExtnFinishPage dialog

class CAddExtnFinishPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddExtnFinishPage)

public:
	CAddExtnFinishPage();
	virtual ~CAddExtnFinishPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDEXTN_FINISH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);

	DECLARE_MESSAGE_MAP()

private:
	CStatic m_cHeader1;
};
