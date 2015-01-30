// BlankView.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "BlankView.h"


// CBlankView

IMPLEMENT_DYNCREATE(CBlankView, CView)

CBlankView::CBlankView()
{

}

CBlankView::~CBlankView()
{
}

BEGIN_MESSAGE_MAP(CBlankView, CView)
END_MESSAGE_MAP()


// CBlankView drawing

void CBlankView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here	
}


// CBlankView diagnostics

#ifdef _DEBUG
void CBlankView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBlankView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBlankView message handlers