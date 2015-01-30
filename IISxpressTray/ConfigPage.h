#pragma once
#include "afxwin.h"

#include "ConfigGeneralDlg.h"
#include "ConfigCacheDlg.h"
#include "ConfigAdvancedDlg.h"
#include "ConfigAHADlg.h"

// CConfigPage dialog

class CConfigPage : public CIISxpressPageBase
{
	DECLARE_DYNAMIC(CConfigPage)

public:
	CConfigPage();
	virtual ~CConfigPage();

// Dialog Data
	enum { IDD = IDD_PAGE_CONFIG };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	afx_msg void OnLvnItemChangedListConfigTypes(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	void UpdateControls(void);	

	bool IsAHALicenseMode();

	CListCtrl					m_cConfigTypes;
	CStatic						m_cChildFrame;

	CConfigGeneralDlg*			m_pGeneralDlg;
	CConfigCacheDlg*			m_pCacheDlg;
	CConfigAdvancedDlg*			m_pAdvancedDlg;
	CConfigAHADlg*				m_pAHADlg;

	CImageList					m_Icons;

	CBitmap						m_GeneralBmp;
	CBitmap						m_CacheBmp;	
	CBitmap						m_AdvancedBmp;
	CBitmap						m_HardwareCompressionBmp;
};
