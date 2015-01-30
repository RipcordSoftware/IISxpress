#pragma once

#include <memory>

// CComtechAHASetupSheet

class CComtechAHASetupSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CComtechAHASetupSheet)

public:
	CComtechAHASetupSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, HBITMAP hbmWatermark = NULL, 
		HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	
	virtual ~CComtechAHASetupSheet();

	CFont* GetHeaderFont(HDC hDC);

protected:
	DECLARE_MESSAGE_MAP()

private:	

	std::auto_ptr<CFont>	m_pftHeader;
	
};


