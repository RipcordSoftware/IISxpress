// WebSiteFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "WebSiteContainerWnd.h"


// CWebSiteContainerWnd

IMPLEMENT_DYNCREATE(CWebSiteContainerWnd, CWnd)

CWebSiteContainerWnd::CWebSiteContainerWnd()
{

}

CWebSiteContainerWnd::~CWebSiteContainerWnd()
{
}


BEGIN_MESSAGE_MAP(CWebSiteContainerWnd, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CWebSiteContainerWnd message handlers

BOOL CWebSiteContainerWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
			UINT nID, CCreateContext* pContext)
{
	BOOL bStatus = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if (bStatus == TRUE)
	{
		// create splitter window
		if (!m_wndSplitter.CreateStatic(this, 1, 2))
			return FALSE;

		if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CWebSelectFormView), CSize(240, 100), pContext) ||
			!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CWebDirectoryContentsView), CSize(100, 100), pContext))
		{
			m_wndSplitter.DestroyWindow();
			return FALSE;
		}					
	}

	return bStatus;
}

void CWebSiteContainerWnd::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndSplitter.m_hWnd != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOREPOSITION | SWP_NOMOVE);
	}
}

// self destruction
void CWebSiteContainerWnd::PostNcDestroy()
{
	// since we have been allocated by a call to CreateObject() rather than 'new' we need
	// to destruct ourselves to prevent leaks
	delete this;
}