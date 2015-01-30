// ColouredListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColouredListCtrl.h"

#define MYLISTCTRL_ROWCOLOUR	RGB(248, 248, 248);

BOOL CColouredListCtrlBase::DeleteItem(CListCtrl& ctrl, int nItem)
{
	BOOL bStatus = ctrl.DeleteItem(nItem);
	if (bStatus == TRUE)
	{
		// the item has been deleted, we need to update the other items otherwise the
		// alternate colour banding will be wrong
		ctrl.Invalidate();
		ctrl.UpdateWindow();
	}

	return bStatus;
}

BOOL CColouredListCtrlBase::RegisterCallback(OnCustomDrawCallback pfnCallback)
{
	m_pfnCallback = pfnCallback;
	return TRUE;
}

void CColouredListCtrlBase::OnCustomDraw(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*) pNmHdr;

	*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYPOSTERASE;

	DWORD dwDrawStage = pDraw->nmcd.dwDrawStage;
	if ((dwDrawStage & CDDS_ITEM) != 0)
	{
		if ((pDraw->nmcd.dwItemSpec & 1) == 1 && pWnd != NULL && ::IsWindowEnabled(pWnd->m_hWnd) == TRUE)
			pDraw->clrTextBk = MYLISTCTRL_ROWCOLOUR;
	}

	if (m_pfnCallback != NULL)
	{
		m_pfnCallback(pWnd, pNmHdr, pResult);
	}
}

// CColouredListCtrl

IMPLEMENT_DYNAMIC(CColouredListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CColouredListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

#ifdef __AFXCVIEW_H__
// CColouredListView message handlers
IMPLEMENT_DYNAMIC(CColouredListView, CListView)

BEGIN_MESSAGE_MAP(CColouredListView, CListView)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()
#endif