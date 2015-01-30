// WelcomePage.cpp : implementation file
//

#include "stdafx.h"
#include "ComtechAHASetup.h"
#include "WelcomePage.h"

#include "ComtechAHASetupSheet.h"

// CWelcomePage dialog

IMPLEMENT_DYNAMIC(CWelcomePage, CPropertyPage)

CWelcomePage::CWelcomePage()
	: CPropertyPage(CWelcomePage::IDD)
{
	m_psp.dwFlags |= PSP_HIDEHEADER | PSP_USETITLE;
	m_psp.dwFlags &= (~PSP_HASHELP);
	m_psp.pszTitle = MAKEINTRESOURCE(IDS_CAPTION);
}

CWelcomePage::~CWelcomePage()
{
}

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WELCOMEHEADER, m_cHeader);
}


BEGIN_MESSAGE_MAP(CWelcomePage, CPropertyPage)
END_MESSAGE_MAP()

BOOL CWelcomePage::OnInitDialog()
{
	BOOL status = CPropertyPage::OnInitDialog();

	CComtechAHASetupSheet* pSheet = (CComtechAHASetupSheet*) GetParent();
	CFont* pftHeader = pSheet->GetHeaderFont(m_cHeader.GetDC()->m_hDC);
	if (pftHeader != NULL)
	{
		m_cHeader.SetFont(pftHeader);
	}

	return status;
}

BOOL CWelcomePage::OnSetActive(void)
{
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();   
	pSheet->SetWizardButtons(PSWIZB_NEXT);

	return TRUE;
}