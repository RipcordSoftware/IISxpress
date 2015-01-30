#pragma once
#include "afxwin.h"

#include "RegistryKeys.h"
#include "IISxpressRegSettingsEx.h"

class CWantReturnEdit : public CEdit
{

public:

	CWantReturnEdit(void)  { }	

protected:

	DECLARE_MESSAGE_MAP()		
	afx_msg LRESULT OnGetDlgCode(WPARAM, LPARAM);

};

// CConfigCacheDlg dialog

class CConfigCacheDlg : public CHelpAwareDialogBase
{
	DECLARE_DYNAMIC(CConfigCacheDlg)

public:
	CConfigCacheDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigCacheDlg();

	void UpdateControls(void);	

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIGCACHE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);	
	afx_msg void OnBnClickedButtonCacheFlush();
	afx_msg void OnBnClickedCheckEnableCache();
	afx_msg void OnEnKillFocusEditMaxCacheSize();
	afx_msg void OnEnKillFocusEditMaxCacheResponses();
	afx_msg void OnNmReturnMaxCacheSize(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnNmReturnMaxResponses(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnEnChangeEditMaxCacheSize();
	afx_msg void OnEnChangeEditMaxCacheResponses();

	DECLARE_MESSAGE_MAP()

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	CIISxpressRegSettingsEx m_RegSettings;

	CButton					m_cEnableCache;
	CWantReturnEdit			m_cMaxMemory;
	CWantReturnEdit			m_cMaxResponses;			
	CButton					m_cFlushButton;

	DWORD					m_dwMaxLimitKB;		

	CFont					m_ftNormal;
	CFont					m_ftItalic;	
	
};
