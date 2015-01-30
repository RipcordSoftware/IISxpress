#pragma once


// CAddContentTypeSelectWebSitesPage dialog

class CAddContentTypeSelectWebSitesPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeSelectWebSitesPage)

public:
	CAddContentTypeSelectWebSitesPage();
	virtual ~CAddContentTypeSelectWebSitesPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_AUTOADD_WEBSITESEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);
	LRESULT OnWizardNext(void);
	afx_msg void OnLvnItemChangedListWebSites(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedRadioAllWebSites();
	afx_msg void OnBnClickedRadioSelectWebSites();

	DECLARE_MESSAGE_MAP()

private:

	void UpdateControls(void);

	void GetCheckedWebSites(CDWordArray& dwaItems);

	CColouredListCtrl	m_cWebSites;
	CButton				m_cAllWebSites;
	CButton				m_cSelectWebSites;	
};
