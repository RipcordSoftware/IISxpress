#include "stdafx.h"

#include "VS2005ToolbarButton.h"

///////////////////////////////////////////////////////////////////////////////
// Performs some modifications on the specified color : luminance and saturation
COLORREF HLS_TRANSFORM (COLORREF rgb, int percent_L, int percent_S);

CVS2005ToolbarButton::CVS2005ToolbarButton() : m_bCapture(false), m_nBorder(0)
{
	m_crHighlightBorder = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighlightFill = HLS_TRANSFORM(m_crHighlightBorder, +70, -57);

	m_crSelectedBorder = m_crHighlightBorder;
	m_crSelectedFill = HLS_TRANSFORM(m_crHighlightBorder, +50, -47);
}

BEGIN_MESSAGE_MAP(CVS2005ToolbarButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

BOOL CVS2005ToolbarButton::LoadPNG(LPCTSTR pszImage, int nBorder)
{
	if (pszImage == NULL)
	{
		return FALSE;
	}

	bool bStatus = CPNGHelper::LoadPNG(pszImage, m_imgButton);
	if (bStatus == true)
	{
		CPNGHelper::ApplyAlpha(m_imgButton);

		bStatus = CPNGHelper::LoadPNG(pszImage, m_imgDisabledButton);
		if (bStatus == true)
		{
			CPNGHelper::ApplyGreyScale(m_imgDisabledButton);
			CPNGHelper::ApplyAlpha(m_imgDisabledButton, 0x3f);
		}
	}

	if (bStatus == true)
	{
		m_nBorder = nBorder;
	}

	if (bStatus == true && m_hWnd != NULL)
	{
		SetWindowPos(NULL, 0, 0, m_imgButton.GetWidth() + nBorder + nBorder, m_imgButton.GetHeight() + nBorder + nBorder, 
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}	

	return bStatus == true ? TRUE : FALSE;
}

void CVS2005ToolbarButton::DrawItem(LPDRAWITEMSTRUCT pDis)
{
	if (pDis == NULL)
	{
		return;
	}
	
	if (m_bCapture == true || (pDis->itemState & ODS_SELECTED) != 0)
	{
		CRect rcClient;
		GetClientRect(rcClient);

		CDC dc;
		dc.Attach(pDis->hDC);

		COLORREF crBorder = m_crHighlightBorder;
		COLORREF crFill = m_crHighlightFill;
		if ((pDis->itemState & ODS_SELECTED) != 0)
		{
			crBorder = m_crSelectedBorder;
			crFill = m_crSelectedFill;
		}
			
		CPen pen(PS_SOLID, 1, crBorder);

		CPen* pOldPen = dc.SelectObject(&pen);	
		dc.Rectangle(rcClient);			

		CRect rcFill(rcClient);
		rcFill.DeflateRect(1, 1);
		dc.FillSolidRect(rcFill, crFill);

		if (pOldPen != NULL)
		{
			dc.SelectObject(pOldPen);
		}

		dc.Detach();	
	}
	else
	{
		CRect rcClient;
		GetClientRect(rcClient);
		::DrawThemeParentBackground(GetSafeHwnd(), pDis->hDC, rcClient);
	}

	if (m_imgButton.IsNull() == false && (pDis->itemState & (ODS_GRAYED | ODS_DISABLED)) == 0)
	{
		m_imgButton.Draw(pDis->hDC, 
			m_nBorder, m_nBorder, m_imgButton.GetWidth(), m_imgButton.GetHeight(), 
			0, 0, m_imgButton.GetWidth(), m_imgButton.GetHeight());		
	}
	else if (m_imgDisabledButton.IsNull() == false && (pDis->itemState & (ODS_GRAYED | ODS_DISABLED)) != 0)
	{
		m_imgDisabledButton.Draw(pDis->hDC, 
			m_nBorder, m_nBorder, m_imgDisabledButton.GetWidth(), m_imgDisabledButton.GetHeight(), 
			0, 0, m_imgDisabledButton.GetWidth(), m_imgDisabledButton.GetHeight());
	}
}

void CVS2005ToolbarButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rcButton;
	GetClientRect(rcButton);

	if (m_bCapture == true)
	{
		if (point.x < 0 || point.y < 0 || point.x >= rcButton.right || point.y >= rcButton.bottom)
		{
			ReleaseCapture();
			m_bCapture = false;

			InvalidateRect(rcButton);
		}
	}
	else
	{
		if ((nFlags & MK_LBUTTON) == 0)
		{
			SetCapture();
			m_bCapture = true;

			InvalidateRect(rcButton);
		}
	}

	CButton::OnMouseMove(nFlags, point);
}

void CVS2005ToolbarButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bCapture == true)
	{
		ReleaseCapture();
		m_bCapture = false;
	}

	CButton::OnLButtonDown(nFlags, point);
}

void CVS2005ToolbarButton::OnCaptureChanged(CWnd* pWnd)
{
	m_bCapture = false;

	CRect rcButton;
	GetClientRect(rcButton);
	InvalidateRect(rcButton);

	CButton::OnCaptureChanged(pWnd);
}