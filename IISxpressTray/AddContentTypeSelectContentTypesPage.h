#pragma once

// CAddContentTypeSelectContentTypesPage dialog

class CAddContentTypeSelectContentTypesPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeSelectContentTypesPage)

public:
	CAddContentTypeSelectContentTypesPage();
	virtual ~CAddContentTypeSelectContentTypesPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_AUTOADD_SELECTCONTENTTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnLvnItemChangedListSelectCTs(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);
	LRESULT OnWizardNext(void);

	DECLARE_MESSAGE_MAP()

private:

	void UpdateControls();	

	bool GetSelectedContentTypes(CArray<CString, LPCTSTR>& saContentTypes);

	CColouredListCtrl m_cContentTypes;	
};
