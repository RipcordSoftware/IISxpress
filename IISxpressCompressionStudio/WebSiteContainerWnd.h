#pragma once

#include "WebSelectFormView.h"

// CWebSiteContainerWnd frame

class CWebSiteContainerWnd : public CWnd
{
	DECLARE_DYNCREATE(CWebSiteContainerWnd)

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
			UINT nID, CCreateContext* pContext = NULL);	

protected:
	CWebSiteContainerWnd();           // protected constructor used by dynamic creation
	virtual ~CWebSiteContainerWnd();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);	
	void OnSize(UINT nType, int cx, int cy);
	void PostNcDestroy();

private:

	CBorderedSplitterWnd m_wndSplitter;	
};


