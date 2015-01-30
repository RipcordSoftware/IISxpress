#pragma once


// CAddContentTypeWelcomePage dialog

class CAddContentTypeWelcomePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeWelcomePage)

public:
	CAddContentTypeWelcomePage();
	virtual ~CAddContentTypeWelcomePage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_WELCOME };

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
