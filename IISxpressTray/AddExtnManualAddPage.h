#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CAddExtnEdit : public CEdit
{

public:

	CAddExtnEdit(void) : CEdit(), m_hwndPartner(NULL) { }

	void SetPartner(HWND hwndPartner) { m_hwndPartner = hwndPartner; }

protected:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	UINT OnGetDlgCode();

private:

	HWND	m_hwndPartner;

};

// CAddExtnManualAddPage dialog

class CAddExtnManualAddPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAddExtnManualAddPage)

public:
	CAddExtnManualAddPage();
	virtual ~CAddExtnManualAddPage();

// Dialog Data
	enum { IDD = IDD_WIZARDPAGE_ADDEXTN_MANUALADD };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL OnSetActive(void);

	DECLARE_MESSAGE_MAP()

private:

	afx_msg void OnBnClickedButtonManualAdd();
	afx_msg void OnBnClickedButtonManualAddRemove();
	afx_msg void OnBnClickedButtonManualAddClear();
	afx_msg void OnLvnItemChangedListManualAddExtensions(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnWizardNext(void);

	void AddExtension(LPCTSTR pszExtension);

	void UpdateControls(void);

	CMap<CString, LPCTSTR, bool, bool> m_PendingExtensions;

	CImageList			m_cImageList;
	
	CComboBoxEx			m_cExtensionsCombo;
	CColouredListCtrl	m_cExtensions;
	CButton				m_cRemove;
	CButton				m_cClear;	

	CAddExtnEdit		m_cAddExtnEdit;
};
