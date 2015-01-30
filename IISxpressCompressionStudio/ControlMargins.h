#pragma once

// CControlMargins command target

class CControlMargins
{
public:

	CControlMargins() : m_nLeft(0), m_nTop(0), m_nRight(0), m_nBottom(0), m_nWidth(0), m_nHeight(0) {}
		
	void GetMargins(CWnd* pParentWnd, CWnd* pControlWnd);	

	int GetLeftMargin() { return m_nLeft; }
	int GetRightMargin() { return m_nRight; }
	int GetTopMargin() { return m_nTop; }
	int GetBottomMargin() { return m_nBottom; }
	int GetHeight() { return m_nHeight; }
	int GetWidth() { return m_nWidth; }

private:

	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;
	int m_nWidth;
	int m_nHeight;
};


