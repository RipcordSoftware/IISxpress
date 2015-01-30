#pragma once
#include "afxwin.h"


// CWelcomePage dialog

class CWelcomePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWelcomePage)

public:
	CWelcomePage();   // standard constructor
	virtual ~CWelcomePage();

// Dialog Data
	enum { IDD = IDD_DIALOG_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnSetActive(void);
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()	
private:
	CStatic m_cHeader;
};
