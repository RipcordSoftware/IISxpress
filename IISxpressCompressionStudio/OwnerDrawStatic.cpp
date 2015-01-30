#include "stdafx.h"

#include "OwnerDrawStatic.h"

BEGIN_MESSAGE_MAP(COwnerDrawStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void COwnerDrawStatic::OnPaint()
{
	if (m_pImage != NULL)
	{
		CRect rcUpdate;
		GetUpdateRect(rcUpdate);

		if (rcUpdate.IsRectEmpty() == FALSE)
		{
			PAINTSTRUCT ps;
			CDC* pDC = BeginPaint(&ps);
			
			if (pDC != NULL)
			{
				m_pImage->Draw(pDC->GetSafeHdc(), rcUpdate, rcUpdate);
			}

			EndPaint(&ps);
		}
	}
}