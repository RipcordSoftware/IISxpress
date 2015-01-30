#pragma once

class CVS2005ToolbarButton : public CButton
{
public:

	CVS2005ToolbarButton();

	BOOL LoadPNG(LPCTSTR pszImage, int nBorder);

protected:	

	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);

	virtual void DrawItem(LPDRAWITEMSTRUCT);

private:

	bool		m_bCapture;

	COLORREF	m_crHighlightBorder;
	COLORREF	m_crHighlightFill;

	COLORREF	m_crSelectedBorder;
	COLORREF	m_crSelectedFill;

	CImage		m_imgButton;
	CImage		m_imgDisabledButton;

	int			m_nBorder;
};