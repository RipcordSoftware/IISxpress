// ContextSelectionView.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "ContextSelectionView.h"

// CContextSelectionView

IMPLEMENT_DYNCREATE(CContextSelectionView, CListView)

CContextSelectionView::CContextSelectionView()
{

}

CContextSelectionView::~CContextSelectionView()
{
}

BEGIN_MESSAGE_MAP(CContextSelectionView, CListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemChanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()


// CContextSelectionView diagnostics

#ifdef _DEBUG
void CContextSelectionView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CContextSelectionView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CContextSelectionView message handlers

BOOL CContextSelectionView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_AUTOARRANGE;
	cs.dwExStyle = LVS_EX_DOUBLEBUFFER;

	CPNGHelper::LoadPNG(_T("HIERARCHY48.PNG"), m_bmpIcons[0]);	
	CPNGHelper::LoadPNG(_T("DOCUMENT48.PNG"), m_bmpIcons[1]);	
	CPNGHelper::LoadPNG(_T("MAGNIFY48.PNG"), m_bmpIcons[2]);	
	CPNGHelper::LoadPNG(_T("COG48.PNG"), m_bmpIcons[3]);	
	CPNGHelper::LoadPNG(_T("CHART48.PNG"), m_bmpIcons[4]);	

	return CListView::PreCreateWindow(cs);
}

void CContextSelectionView::OnInitialUpdate()
{
	CListCtrl& cListCtrl = GetListCtrl();

	m_images.Create(48, 48, ILC_COLOR32 | ILC_MASK, 1, 8);
	cListCtrl.SetImageList(&m_images, LVSIL_NORMAL);

	cListCtrl.SetIconSpacing(128, 96);

	int nItem = cListCtrl.InsertItem(0, _T("Sites"), 0);
	cListCtrl.SetItemData(nItem, Sites);

	nItem = cListCtrl.InsertItem(1, _T("Rules"), 1);
	cListCtrl.SetItemData(nItem, Rules);

	nItem = cListCtrl.InsertItem(2, _T("Live"), 2);
	cListCtrl.SetItemData(nItem, Live);

	nItem = cListCtrl.InsertItem(3, _T("Configuration"), 3);
	cListCtrl.SetItemData(nItem, Configuration);

	nItem = cListCtrl.InsertItem(4, _T("Stats"), 4);
	cListCtrl.SetItemData(nItem, Stats);

	// default to selecting the first item in the list
	LVITEM item;
	item.mask = LVIF_STATE;
	item.iItem = 0;
	item.iSubItem = 0;
	item.state = LVIS_SELECTED;
	item.stateMask = LVIS_SELECTED;
	cListCtrl.SetItem(&item);
}

void CContextSelectionView::OnLvnItemChanged(NMHDR* pNotifyStruct, LRESULT* result)
{
	NMLISTVIEW* pNmListView = (NMLISTVIEW*) pNotifyStruct;

	if (pNmListView != NULL && pNmListView->iItem >= 0 && ((pNmListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) && ((pNmListView->uOldState & LVIS_SELECTED) == 0))
	{
		CListCtrl& cListCtrl = GetListCtrl();

		StackedViewTypes itemType = (StackedViewTypes) cListCtrl.GetItemData(pNmListView->iItem);

		CNotifyContextSelectionViewChanged notify(itemType);

		GetDocument()->UpdateAllViews(this, 0, &notify);
	}
}

void CContextSelectionView::OnCustomDraw(NMHDR* pNotifyStruct, LRESULT* result)
{
	//return;

	NMLVCUSTOMDRAW* pCustomDraw = (NMLVCUSTOMDRAW*) pNotifyStruct;

	if (pCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*result = CDRF_NOTIFYITEMDRAW;
		return;
	}
	
	if (pCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		CListCtrl& cListCtrl = GetListCtrl();

		CRect rcItem;
		cListCtrl.GetItemRect(pCustomDraw->nmcd.dwItemSpec, &rcItem, LVIR_BOUNDS);		

		CRect rcIcon;
		cListCtrl.GetItemRect(pCustomDraw->nmcd.dwItemSpec, &rcIcon, LVIR_ICON);				

		CRect rcLabel;
		cListCtrl.GetItemRect(pCustomDraw->nmcd.dwItemSpec, &rcLabel, LVIR_LABEL);

		CString sText = cListCtrl.GetItemText(pCustomDraw->nmcd.dwItemSpec, 0);
						
		CDC* pDC = CDC::FromHandle(pCustomDraw->nmcd.hdc);				

		UINT nState = cListCtrl.GetItemState(pCustomDraw->nmcd.dwItemSpec, LVIS_SELECTED | LVIS_FOCUSED);		
		if (nState != 0)
		{	
			CRgn region;
			region.CreateRoundRectRgn(rcLabel.left - 10, rcLabel.top - 1, rcLabel.right + 10, rcLabel.bottom + 1, 10, 10);

			CBrush brLabel;
			brLabel.CreateSysColorBrush(COLOR_HIGHLIGHT);

			pDC->FillRgn(&region, &brLabel);			

			pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));			
		}
		else
		{			
			pDC->FillSolidRect(rcItem, pDC->GetBkColor());			
		}

		CPoint ptCenter = rcIcon.CenterPoint();						
		CImage& bmpIcon = m_bmpIcons[pCustomDraw->nmcd.dwItemSpec];
		bmpIcon.Draw(pDC->m_hDC, ptCenter.x - (bmpIcon.GetWidth() / 2), ptCenter.y - (bmpIcon.GetHeight() / 2));				

		pDC->DrawText(sText, sText.GetLength(), rcLabel, DT_CENTER);

		*result = CDRF_SKIPDEFAULT;
	}
}