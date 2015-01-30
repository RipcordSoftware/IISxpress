#pragma once
#include "afxwin.h"


// CAboutPage dialog

class CAboutPage : public CIISxpressPageBase
{
	DECLARE_DYNAMIC(CAboutPage)

public:
	CAboutPage();
	virtual ~CAboutPage();

// Dialog Data
	enum { IDD = IDD_PAGE_ABOUT };

	void ServerOnline(void);
	void ServerOffline(void);
	void Heartbeat(void) {}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void OnLButtonDown(UINT nFlags, CPoint point);	
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void OnDestroy(void);
	
	afx_msg void OnBnClickedButtonPurchase();
	afx_msg void OnBnClickedButtonEnterKey();

	afx_msg void OnURLClick(NMHDR* pNotifyStruct, LRESULT * result);

	afx_msg void OnBnClickedButtonCheckForUpdates();

	DECLARE_MESSAGE_MAP()

private:

	void UpdateRegistrationControls(bool bQuiet = false);

	CEdit m_cCopyrightWarning;
	CStatic m_cRipcordSoftwareHotspot;

	HCURSOR m_hHandCursor;
	HCURSOR m_hOldCursor;

	CWnd*		m_pcRegStatus;
	CStatic		m_cPlainRegStatus;
	CLinkCtrl	m_cLinkedRegStatus;

	CButton m_cPurchase;
	CButton m_cEnterKey;
	
};
