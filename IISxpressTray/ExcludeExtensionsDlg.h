#pragma once

#include "ColouredListCtrl.h"
#include "HelpAwareDialogBase.h"

#include <map>
using namespace std;

// CExcludeExtensionsDlg dialog

class CExcludeExtensionsDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CExcludeExtensionsDlg)

public:
	CExcludeExtensionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExcludeExtensionsDlg();

	void UpdateControls(void);
	void UpdateControls(DWORD dwUpdateCookie);

// Dialog Data
	enum { IDD = IDD_DIALOG_EXCLUDEEXTENSIONS };		

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAddExtensionRule();
	afx_msg void OnBnClickedButtonDeleteExtensionRule();
	afx_msg void OnLvnItemChangedListExtensions(NMHDR *pNMHDR, LRESULT *pResult);	

	DECLARE_MESSAGE_MAP()

private:

	void UpdateExtensionsListControl(void);

	CColouredListCtrl	m_cExtensions;
	CButton				m_cAddExtension;
	CButton				m_cDeleteExtension;

	CImageList			m_cImageList;	

	DWORD				m_dwUpdateCookie;

	HACCEL				m_hAccelerator;
};
