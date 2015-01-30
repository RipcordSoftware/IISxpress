// FixedSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "FixedSplitterWnd.h"

IMPLEMENT_DYNCREATE(CFixedSplitterWnd, CSplitterWnd)

BEGIN_MESSAGE_MAP(CFixedSplitterWnd, CSplitterWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CFixedSplitterWnd

CFixedSplitterWnd::CFixedSplitterWnd()
{	
}

CFixedSplitterWnd::CFixedSplitterWnd(int cxBorder, int cyBorder)
{
	m_cxBorder = cxBorder;
	m_cyBorder = cyBorder;
}

CFixedSplitterWnd::~CFixedSplitterWnd()
{
}


// CFixedSplitterWnd member functions

afx_msg void CFixedSplitterWnd::OnMouseMove(UINT, CPoint)
{
}

afx_msg void CFixedSplitterWnd::OnLButtonDown( UINT, CPoint )
{
}

afx_msg void CFixedSplitterWnd::OnLButtonUp( UINT, CPoint )
{
}