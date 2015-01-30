#pragma once

class CAddContentTypeEdit : public CEdit
{

public:

	CAddContentTypeEdit(void) : CEdit(), m_hwndPartner(NULL) { }

	void SetPartner(HWND hwndPartner) { m_hwndPartner = hwndPartner; }

protected:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	UINT OnGetDlgCode();

private:

	HWND	m_hwndPartner;

};

// CAddContentTypeManualAddPage dialog

class CAddContentTypeManualAddPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddContentTypeManualAddPage)

public:
	CAddContentTypeManualAddPage();
	virtual ~CAddContentTypeManualAddPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDCONTENTTYPE_MANUALADD };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);

	DECLARE_MESSAGE_MAP()

private:

	afx_msg void OnBnClickedButtonManualAdd();
	afx_msg void OnBnClickedButtonManualAddRemove();
	afx_msg void OnBnClickedButtonManualAddClear();
	afx_msg void OnLvnItemChangedListManualAddContentTypes(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnWizardNext(void);

	bool AddContentType(LPCTSTR pszContentType);

	void UpdateControls(void);

	CMap<CString, LPCTSTR, bool, bool> m_PendingContentTypes;

	CComboBox				m_cContentTypesCombo;
	CColouredListCtrl		m_cContentTypes;
	CButton					m_cRemove;
	CButton					m_cClear;	

	CAddContentTypeEdit		m_cAddContentTypeEdit;
};
