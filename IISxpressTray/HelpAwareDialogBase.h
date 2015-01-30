#pragma once


// CHelpAwareDialogBase dialog

class CHelpAwareDialogBase : public CDialog
{
	DECLARE_DYNAMIC(CHelpAwareDialogBase)

protected:

	CHelpAwareDialogBase(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CHelpAwareDialogBase();

	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL OnNotify(WPARAM, LPARAM, LRESULT* pResult);

private:

	CToolTipCtrl		m_cContextHelp;
	TCHAR				m_szBuffer[1024];
	TOOLINFO			m_DelToolInfo;


};