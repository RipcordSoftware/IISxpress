#pragma once


// CAddContentTypeConfirmPage dialog

class CAddContentTypeConfirmPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeConfirmPage)

public:
	CAddContentTypeConfirmPage();
	virtual ~CAddContentTypeConfirmPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);
	LRESULT OnWizardNext(void);

	DECLARE_MESSAGE_MAP()

private:

	CColouredListCtrl m_cContentTypes;
};
