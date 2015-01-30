// AddContentTypeWizardSheet.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeWizardSheet.h"


// CAddContentTypeWizardSheet

IMPLEMENT_DYNAMIC(CAddContentTypeWizardSheet, CPropertySheet)

/*CAddContentTypeWizardSheet::CAddContentTypeWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}*/

/*CAddContentTypeWizardSheet::CAddContentTypeWizardSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}*/

CAddContentTypeWizardSheet::CAddContentTypeWizardSheet(
									UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, 
									HBITMAP hbmWatermark, HPALETTE hpalWatermark, HBITMAP hbmHeader) :
	CPropertySheet(nIDCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader),
	m_dwUpdateCookie(0)
{
	m_Mode = CAddContentTypeWizardSheet::Auto;

	if (g_VerInfo.dwMajorVersion < 6)
	{
		m_Metabase.Init();
	}
	else
	{
		CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&m_pConfigXML);
	}
}

CAddContentTypeWizardSheet::~CAddContentTypeWizardSheet()
{
}


BEGIN_MESSAGE_MAP(CAddContentTypeWizardSheet, CPropertySheet)
END_MESSAGE_MAP()


// CAddContentTypeWizardSheet message handlers

BOOL CAddContentTypeWizardSheet::OnInitDialog(void)
{	
	CPropertySheet::OnInitDialog();

	CWnd* pWndHelp = GetDlgItem(IDHELP);
	if (pWndHelp != NULL)
	{
		// get the help button's caption
		CString sCaption;
		pWndHelp->GetWindowText(sCaption);

		// get the button style - force invisible and disabled
		DWORD dwStyle = ::GetWindowLong(*pWndHelp, GWL_STYLE);
		dwStyle &= ~WS_VISIBLE;
		dwStyle |= WS_DISABLED;

		// get the button's rectangle in screen coords
		CRect rcHelp;
		::GetWindowRect(*pWndHelp, &rcHelp);

		// convert the coords to this window
		ScreenToClient(&rcHelp);

		// create the button
		m_cDisabledHelp.Create(sCaption, dwStyle, rcHelp, this, IDHELP + 1);

		// use the existing button font
		CFont* pFont = pWndHelp->GetFont();
		m_cDisabledHelp.SetFont(pFont);		
	}	

	return TRUE;
}

CFont* CAddContentTypeWizardSheet::GetHeaderFont(HDC hDC)
{
	if (m_pftHeader.get() == NULL)
	{
		int nFontSize = 12;

		m_pftHeader = auto_ptr<CFont>(new CFont());

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

bool CAddContentTypeWizardSheet::GetNeverRules(INeverCompressRules** ppNeverRules) 
{ 
	if (m_pNeverRules == NULL || ppNeverRules == NULL)
		return false;

	m_pNeverRules.CopyTo(ppNeverRules);
	return true;
}

void CAddContentTypeWizardSheet::EnableHelp(BOOL bEnable)
{
	if (m_cDisabledHelp.m_hWnd == NULL)
	{
		// we can't disable help on the first page when it is displayed for the first time
		ASSERT(bEnable == TRUE);
		return;
	}

	// get the REAL help button
	CWnd* pWndHelp = GetDlgItem(IDHELP);
	if (pWndHelp != NULL)
	{
		if (bEnable == TRUE)
		{
			// the user wants the real help button
			m_cDisabledHelp.ShowWindow(SW_HIDE);
			pWndHelp->ShowWindow(SW_SHOW);			
		}
		else
		{
			// the user wants the disabled help button
			m_cDisabledHelp.ShowWindow(SW_SHOW);
			pWndHelp->ShowWindow(SW_HIDE);			
		}
	}
}

HRESULT CAddContentTypeWizardSheet::GetXMLConfigDoc(IXMLDOMDocument** ppDoc)
{
	if (ppDoc == NULL)
		return E_POINTER;

	if (m_pConfigXML == NULL)
		return E_FAIL;

	return m_pConfigXML.CopyTo(ppDoc);
}