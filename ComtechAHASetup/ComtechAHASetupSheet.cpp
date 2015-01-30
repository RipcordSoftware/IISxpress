// ComtechAHASetupSheet.cpp : implementation file
//

#include "stdafx.h"
#include "ComtechAHASetup.h"
#include "ComtechAHASetupSheet.h"

// CComtechAHASetupSheet

IMPLEMENT_DYNAMIC(CComtechAHASetupSheet, CPropertySheet)

CComtechAHASetupSheet::CComtechAHASetupSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, HBITMAP hbmWatermark, 
		HPALETTE hpalWatermark, HBITMAP hbmHeader)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader)
{

}

CComtechAHASetupSheet::~CComtechAHASetupSheet()
{
}


BEGIN_MESSAGE_MAP(CComtechAHASetupSheet, CPropertySheet)
END_MESSAGE_MAP()


// CComtechAHASetupSheet message handlers

CFont* CComtechAHASetupSheet::GetHeaderFont(HDC hDC)
{
	if (m_pftHeader.get() == NULL)
	{
		int nFontSize = 12;

		m_pftHeader = std::auto_ptr<CFont>(new CFont());

		if (hDC == NULL)
			hDC = GetDC()->m_hDC;

		m_pftHeader->CreateFont(
			0 - GetDeviceCaps(hDC, LOGPIXELSY) * nFontSize / 72,		
			0,
			0,
			0,
			FW_BOLD,
			FALSE,
			FALSE,
			0,
			ANSI_CHARSET,
			OUT_TT_ONLY_PRECIS, //OUT_DEFAULT_PRECIS,		
			CLIP_DEFAULT_PRECIS,
			//CLEARTYPE_NATURAL_QUALITY, 
			CLEARTYPE_QUALITY, 
			//ANTIALISED_QUALITY, 
			//DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			_T("Verdana"));
	}

	return m_pftHeader.get();
}