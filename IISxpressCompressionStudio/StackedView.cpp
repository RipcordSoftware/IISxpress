// StackedView.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "StackedView.h"

// CStackedView

IMPLEMENT_DYNCREATE(CStackedView, CView)

CStackedView::CStackedView() : m_pActiveWnd(NULL), m_dwActiveWndCtrlId(0), m_pBlankView(NULL), m_pWebSiteContainerWnd(NULL), m_pResponseHistoryView(NULL)
{
}

CStackedView::~CStackedView()
{	
}


BEGIN_MESSAGE_MAP(CStackedView, CView)
	ON_WM_SIZE()	
END_MESSAGE_MAP()


// CStackedView message handlers

BOOL CStackedView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
			UINT nID, CCreateContext* pContext)
{
	BOOL bStatus = CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if (bStatus == TRUE)
	{
		SetWindowText(_T("CStackedView"));

		int nViewOffset = 1;

		m_pBlankView = dynamic_cast<CBlankView*>(RUNTIME_CLASS(CBlankView)->CreateObject());
		m_pBlankView->Create(NULL, _T("BlankView"), 0, CFrameWnd::rectDefault, this, 100, pContext);		
		m_pBlankView->SetDlgCtrlID(AFX_IDW_PANE_FIRST + nViewOffset++);

		m_pWebSiteContainerWnd = dynamic_cast<CWebSiteContainerWnd*>(RUNTIME_CLASS(CWebSiteContainerWnd)->CreateObject());		
		m_pWebSiteContainerWnd->Create(NULL, _T("CWebSiteContainerWnd"), WS_CHILD | WS_VISIBLE, CFrameWnd::rectDefault, this, 101, pContext);
		m_pWebSiteContainerWnd->SetDlgCtrlID(AFX_IDW_PANE_FIRST + nViewOffset++);

		m_pResponseHistoryView = dynamic_cast<CResponseHistoryView*>(RUNTIME_CLASS(CResponseHistoryView)->CreateObject());
		m_pResponseHistoryView->Create(NULL, _T("CResponseHistoryView"), WS_CHILD | WS_VISIBLE, CFrameWnd::rectDefault, this, 102, pContext);
		m_pResponseHistoryView->SetDlgCtrlID(AFX_IDW_PANE_FIRST + nViewOffset++);

		SetActiveView(m_pBlankView);
	}

	return bStatus;
}

void CStackedView::SetActiveView(CWnd* pWnd)
{	
	if (this->m_hWnd == NULL)
	{
		return;
	}

	if (m_pActiveWnd != NULL)
	{
		m_pActiveWnd->ShowWindow(SW_HIDE);		
		m_pActiveWnd->SetDlgCtrlID(m_dwActiveWndCtrlId);
	}

	if (pWnd == NULL)
	{
		pWnd = m_pBlankView;
	}

	pWnd->ShowWindow(SW_SHOW);	
	m_dwActiveWndCtrlId = pWnd->GetDlgCtrlID();
	pWnd->SetDlgCtrlID(AFX_IDW_PANE_FIRST);
	m_pActiveWnd = pWnd;

	CRect rcStackedView;
	GetClientRect(rcStackedView);
	m_pActiveWnd->SetWindowPos(NULL, 0, 0, rcStackedView.Width(), rcStackedView.Height(), SWP_NOREPOSITION | SWP_NOMOVE);	
}

void CStackedView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pHint == NULL || pHint->IsKindOf(RUNTIME_CLASS(CNotifyContextSelectionViewChanged)) == FALSE)
	{
		return;
	}

	CNotifyContextSelectionViewChanged* pNotify = dynamic_cast<CNotifyContextSelectionViewChanged*>(pHint);
	if (pNotify == NULL)
	{
		return;
	}

	switch (pNotify->GetSelectedView())
	{
	case Sites:
		SetActiveView(m_pWebSiteContainerWnd);
		break;
	case Live:
		SetActiveView(m_pResponseHistoryView);
		break;
	default:
		SetActiveView(m_pBlankView);
		break;
	}
}

void CStackedView::OnSize(UINT nType, int cx, int cy)
{
	if (m_pActiveWnd != NULL && m_pActiveWnd->m_hWnd != NULL)
	{
		m_pActiveWnd->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOREPOSITION | SWP_NOMOVE);
	}
}