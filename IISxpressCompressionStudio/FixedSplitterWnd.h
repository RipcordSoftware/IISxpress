#pragma once

// CFixedSplitterWnd command target

class CFixedSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNCREATE(CFixedSplitterWnd)

public:
	CFixedSplitterWnd();
	CFixedSplitterWnd(int cxBorder, int cyBorder);

	virtual ~CFixedSplitterWnd();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnLButtonDown( UINT, CPoint );
	afx_msg void OnLButtonUp( UINT, CPoint );

};


