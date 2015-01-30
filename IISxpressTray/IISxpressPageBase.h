#ifndef _IISXPRESSPAGEBASE_H
#define _IISXPRESSPAGEBASE_H

class CIISxpressPageBase : public CPropertyPage
{

	DECLARE_DYNAMIC(CIISxpressPageBase)

protected:

	CIISxpressPageBase(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE)) :
		 CPropertyPage(nIDTemplate, nIDCaption, dwSize) { }

	BOOL PreTranslateMessage(MSG* pMsg);

	BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	BOOL OnNotify(WPARAM, LPARAM, LRESULT* pResult);	

	DECLARE_MESSAGE_MAP()

public:

	virtual void ServerOnline(void) = 0;
	virtual void ServerOffline(void) = 0;
	virtual void Heartbeat(void) = 0;
	
	CToolTipCtrl		m_cContextHelp;
	LPTSTR				m_szBuffer[1024];
	TOOLINFO			m_DelToolInfo;

};

#endif