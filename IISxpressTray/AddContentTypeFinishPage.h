#pragma once


// CAddContentTypeFinishPage dialog

class CAddContentTypeFinishPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeFinishPage)

public:
	CAddContentTypeFinishPage();
	virtual ~CAddContentTypeFinishPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_FINISH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);

	DECLARE_MESSAGE_MAP()

private:
	CStatic m_cHeader1;
};
