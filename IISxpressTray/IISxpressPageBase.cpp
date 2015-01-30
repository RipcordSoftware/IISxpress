#include "stdafx.h"

IMPLEMENT_DYNAMIC(CIISxpressPageBase, CPropertyPage)

BEGIN_MESSAGE_MAP(CIISxpressPageBase, CPropertyPage)	
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL CIISxpressPageBase::OnHelpInfo(HELPINFO* pHelpInfo)
{
	if (::IsWindow(m_cContextHelp) == FALSE)
	{	
		CString sTitle;
		sTitle.LoadString(IDS_IISXPRESS_APPNAME);

		m_cContextHelp.Create(this, TTS_BALLOON);				
		m_cContextHelp.Activate(FALSE);
		m_cContextHelp.SetTitle(TTI_INFO, sTitle);

		m_cContextHelp.SetDelayTime(TTDT_INITIAL, 0);
		m_cContextHelp.SetDelayTime(TTDT_AUTOPOP, 30000);
		m_cContextHelp.SetMaxTipWidth(600);				
	}

	// if we have either SHIFT+F1 or the left mouse button down then assume it's a context help request
	short nF1Key = ::GetKeyState(VK_F1);
	short nShiftKey = ::GetKeyState(VK_LSHIFT);
	short nLButton = ::GetKeyState(VK_LBUTTON);
	bool bGotContext = ((nF1Key < 0 && nShiftKey < 0) || nLButton < 0) ? true : false;

	// if we've got a context help request
	if (bGotContext &&
		pHelpInfo->iContextType == HELPINFO_WINDOW &&
		::IsWindow((HWND) pHelpInfo->hItemHandle) == TRUE &&
		pHelpInfo->iCtrlId >= 0)
	{
		CString sHelp;
		if (sHelp.LoadString(pHelpInfo->iCtrlId) == TRUE)
		{			
			//int nTools = m_cContextHelp.GetToolCount();
			//ASSERT(nTools == 0);
						
			m_cContextHelp.AddTool(CWnd::FromHandle((HWND) pHelpInfo->hItemHandle));			
			m_cContextHelp.Activate(TRUE);			
		}
	}
	else
	{
		// it wasn't a context menu request, tell the sheet we got an F1
		CWnd* pWnd = GetParent();
		while (pWnd != NULL)
		{
			if (pWnd->IsKindOf(RUNTIME_CLASS(CIISxpressTrayPropertySheet)) == TRUE)
			{
				pWnd->SendMessage(WM_HELP, 0, (LPARAM) pHelpInfo);
				break;
			}

			pWnd = pWnd->GetParent();
		}
	}

	return TRUE;  
}

BOOL CIISxpressPageBase::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message >= WM_MOUSEMOVE && pMsg->message <= WM_MOUSELAST) && ::IsWindow(m_cContextHelp) == TRUE)
	{
		m_cContextHelp.RelayEvent(pMsg);
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CIISxpressPageBase::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNmHdr = (NMHDR*) lParam;

	if (pNmHdr->code == TTN_POP && pNmHdr->hwndFrom == m_cContextHelp)
	{				
		m_cContextHelp.PostMessage(TTM_ACTIVATE, FALSE, 0);				

		memset(&m_DelToolInfo, 0, sizeof(m_DelToolInfo));
		m_DelToolInfo.uFlags = TTF_IDISHWND;
		m_DelToolInfo.cbSize = sizeof(m_DelToolInfo);
		m_DelToolInfo.hwnd = GetSafeHwnd();
		m_DelToolInfo.uId = (UINT_PTR) pNmHdr->idFrom;

		m_cContextHelp.PostMessage(TTM_DELTOOL, 0, (LPARAM) &m_DelToolInfo);		
	}	
	else if (pNmHdr->code == TTN_NEEDTEXT && pNmHdr->hwndFrom == m_cContextHelp)
	{
		NMTTDISPINFO* pDispInfo = (NMTTDISPINFO*) lParam;

		LONG_PTR nId = ::GetWindowLongPtr((HWND) pDispInfo->hdr.idFrom, GWLP_ID);
		
		CString sMsg;
		if (sMsg.LoadString((UINT) nId) == TRUE)
		{
			_tcscpy_s((TCHAR*) m_szBuffer, _countof(m_szBuffer), sMsg);		

			pDispInfo->lpszText = (TCHAR*) m_szBuffer;
		}
	}

	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}