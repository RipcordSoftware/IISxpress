#pragma once

class COwnerDrawStatic : public CStatic
{

public:

	COwnerDrawStatic() : m_pImage(NULL) {}

	void SetImage(CImage* pImage) { m_pImage = pImage; }

protected:

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();

private:

	CImage* m_pImage;
};