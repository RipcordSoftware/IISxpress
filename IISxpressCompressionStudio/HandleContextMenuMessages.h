#pragma once

#define DECLARE_HANDLECONTEXTMENUMESSAGES() protected: afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);

#define ON_HANDLECONTEXTMENUMESSAGES() ON_WM_MENUSELECT()

#define IMPLEMENT_HANDLECONTEXTMENUMESSAGES(theClass)									\
void theClass::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)					\
{																						\
	CFrameWnd* pFrame = dynamic_cast<CFrameWnd*>(::AfxGetMainWnd());					\
	if (pFrame == NULL)																	\
		return;																			\
																						\
	if (nFlags == 0xffff)																\
	{																					\
		nItemID = AFX_IDS_IDLEMESSAGE;													\
	}																					\
																						\
	CString sMessage;																	\
	pFrame->GetMessageString(nItemID, sMessage);										\
																						\
	CWnd* pMsgWnd = pFrame->GetMessageBar();											\
	if (pMsgWnd != NULL)																\
	{																					\
		pMsgWnd->SetWindowText(sMessage);												\
	}																					\
}																						
