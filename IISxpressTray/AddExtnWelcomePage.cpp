// AddExtnWelcomePage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddExtnWelcomePage.h"


// CAddExtnWelcomePage dialog

IMPLEMENT_DYNAMIC(CAddExtnWelcomePage, CPropertyPage)
CAddExtnWelcomePage::CAddExtnWelcomePage()
	: CPropertyPage(CAddExtnWelcomePage::IDD)	
{
}

CAddExtnWelcomePage::~CAddExtnWelcomePage()
{
}

void CAddExtnWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICWIZARDHEADER1, m_cHeader1);
	DDX_Control(pDX, IDC_RADIOSEARCHEXTNS, m_cAutoSearch);
	DDX_Control(pDX, IDC_RADIOMANUALEXTNS, m_cManualSearch);
}


BEGIN_MESSAGE_MAP(CAddExtnWelcomePage, CPropertyPage)
END_MESSAGE_MAP()


// CAddExtnWelcomePage message handlers

BOOL CAddExtnWelcomePage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();	

	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();
	CFont* pftHeader = pSheet->GetHeaderFont(m_cHeader1.GetDC()->m_hDC);
	if (pftHeader != NULL)
	{
		m_cHeader1.SetFont(pftHeader);
	}

	m_cAutoSearch.SetCheck(BST_CHECKED);

	// we want the help button
	pSheet->EnableHelp(TRUE);

	return TRUE;
}

BOOL CAddExtnWelcomePage::OnSetActive(void)
{
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();   
	pSheet->SetWizardButtons(PSWIZB_NEXT);

	return TRUE;
}

LRESULT CAddExtnWelcomePage::OnWizardNext(void)
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();   

	if ((m_cAutoSearch.GetCheck() & BST_CHECKED) != 0)
	{
		pSheet->SetAddExtnWizardMode(CAddExtnWizardSheet::Auto);
	}
	else
	{
		pSheet->SetAddExtnWizardMode(CAddExtnWizardSheet::Manual);
	}

	return 0;
}