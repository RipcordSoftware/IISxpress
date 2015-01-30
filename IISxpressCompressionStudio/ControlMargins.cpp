// ControlMargins.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "ControlMargins.h"

// CControlMargins

void CControlMargins::GetMargins(CWnd* pParentWnd, CWnd* pControlWnd) 
{
	if (pParentWnd != NULL && pControlWnd != NULL)
	{
		CRect rcParent;
		pParentWnd->GetClientRect(&rcParent);

		CRect rcControl;
		pControlWnd->GetWindowRect(&rcControl);
		pParentWnd->ScreenToClient(&rcControl);

		m_nLeft = rcControl.left;
		m_nTop = rcControl.top;
		m_nRight = rcParent.right - rcControl.right;
		m_nBottom = rcParent.bottom - rcControl.bottom;

		m_nHeight = rcControl.Height();
		m_nWidth = rcControl.Width();
	}
}
