#pragma once

#include "resource.h"

#include "ColouredListCtrl.h"

// CExcludeContentTypesDlg dialog

class CExcludeContentTypesDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CExcludeContentTypesDlg)

public:
	CExcludeContentTypesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExcludeContentTypesDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EXCLUDECONTENTTYPES };

	void UpdateControls(void);
	void UpdateControls(DWORD dwUpdateCookie);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAddContentTypeRule();
	afx_msg void OnBnClickedButtonDeleteContentTypeRule();
	afx_msg void OnLvnItemChangedListContentTypes(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:

	void UpdateContentTypesListControl(void);

	CColouredListCtrl	m_cContentTypes;
	CButton				m_cDeleteContentType;
	CButton				m_cAddContentType;

	DWORD				m_dwUpdateCookie;

	HACCEL				m_hAccelerator;
};
