#pragma once

#include "WebSiteContainerWnd.h"

// CStackedView frame

class CStackedView : public CView
{
	DECLARE_DYNCREATE(CStackedView)

protected:
	CStackedView();           // protected constructor used by dynamic creation
	virtual ~CStackedView();

	DECLARE_MESSAGE_MAP()
	
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
			UINT nID, CCreateContext* pContext = NULL);	

	virtual void OnDraw(CDC* pDC) {}
	void OnSize(UINT nType, int cx, int cy);	
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	CBlankView*				m_pBlankView;	
	CWebSiteContainerWnd*	m_pWebSiteContainerWnd;
	CResponseHistoryView*	m_pResponseHistoryView;

private:

	void SetActiveView(CWnd* pWnd);
	CWnd* m_pActiveWnd;
	DWORD m_dwActiveWndCtrlId;
	
};


