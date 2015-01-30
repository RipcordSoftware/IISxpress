#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "IISxpressRegSettingsEx.h"

#include "UserAgentLookup.h"

// CExcludeUserAgentsDlg dialog

class CExcludeUserAgentsDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CExcludeUserAgentsDlg)

public:
	CExcludeUserAgentsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExcludeUserAgentsDlg();

	void UpdateControls(void);
	void UpdateControls(DWORD dwUpdateCookie);

// Dialog Data
	enum { IDD = IDD_DIALOG_EXCLUDEUSERAGENTS };

protected:
	BOOL OnInitDialog();
	void OnDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckEnableUserAgentRules();
	afx_msg void OnLvnItemChangedListUserAgents(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnLvnEndLabelEditListUserAgents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeyDownListUserAgents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonEditUserAgentRule();
	afx_msg void OnBnClickedButtonDeleteUserAgentRule();
	afx_msg void OnBnClickedButtonAddUserAgentRule();
	afx_msg void OnLvnInsertItemListUserAgents(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:

	void SetUserAgentRuleEnabledState();
	void UpdateControlEnableStates();
	void UpdateUserAgentControl();
	void UpdateEditUserAgentControl();

	void UpdateDeleteUserAgentControl();
	void UpdateUserAgentItemName(int nItem);

	void EditSelectedUserAgentString();

	void SortItems();

	void DeleteSelectedItems();
	void DeleteAllItems();

	void DeleteItemData(int nItem);

	void UpdateUserAgentExclusionConfiguration();

	static void OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult);
	
	static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void SetItemImage(int nItem);

	DWORD m_dwUpdateCookie;

	CIISxpressRegSettingsEx m_Config;

	CUserAgentLookup m_UserAgentLookup;

	CColouredListCtrl m_cExcludedUserAgents;
	CButton m_cAddUserAgentRule;
	CButton m_cDeleteUserAgentRule;
	CButton m_cEditUserAgentRule;
	CButton m_cUserAgentExclusionEnabled;		

	CImageList				m_ImageList;
	CImage					m_imgGreenFlag;
	CImage					m_imgRedFlag;
	
};
