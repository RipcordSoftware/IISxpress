#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

#include "ExcludeExtensionsDlg.h"
#include "ExcludeContentTypesDlg.h"
#include "ExcludeFoldersDlg.h"
#include "ExcludeNetworkDlg.h"
#include "ExcludeUserAgentsDlg.h"

#include "NeverCompressRulesNotify.h"

// CExclusionsPage dialog

class CExclusionsPage : public CIISxpressPageBase
{
	DECLARE_DYNAMIC(CExclusionsPage)

public:
	CExclusionsPage();
	virtual ~CExclusionsPage();

	HRESULT OnRuleChanged(NeverRuleChangeHint hint, DWORD dwChangeSource);

// Dialog Data
	enum { IDD = IDD_PAGE_EXCLUSIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	afx_msg void OnDestroy();
	afx_msg void OnLvnItemChangedListExclusionTypes(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnUpdateControls(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	void UpdateControls(void);

	CListCtrl					m_cExclusionTypes;
	CStatic						m_cChildFrame;

	CExcludeExtensionsDlg*		m_pExtensions;
	CExcludeContentTypesDlg*	m_pContentTypes;
	CExcludeFoldersDlg*			m_pFolders;
	CExcludeNetworkDlg*			m_pNetwork;
	CExcludeUserAgentsDlg*		m_pUserAgents;

	CImageList					m_Icons;

	CBitmap						m_ExtensionsBmp;
	CBitmap						m_ContentTypeBmp;	
	CBitmap						m_URIBmp;
	CBitmap						m_NetworkBmp;
	CBitmap						m_UserAgentBmp;
	
	bool RegisterConnectionPoint(void);
	bool UnregisterConnectionPoint(void);

	CComObject<CNeverCompressRulesNotify<CExclusionsPage> >*	m_pNotify;
	CComPtr<IConnectionPoint>									m_pNeverCompressRulesNotifyCP;	
	DWORD														m_dwCookie;
	
};