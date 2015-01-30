// AddContentTypeWelcomePage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeWelcomePage.h"


// CAddContentTypeWelcomePage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeWelcomePage, CPropertyPage)

CAddContentTypeWelcomePage::CAddContentTypeWelcomePage()
	: CPropertyPage(CAddContentTypeWelcomePage::IDD)
{
}

CAddContentTypeWelcomePage::~CAddContentTypeWelcomePage()
{
}

void CAddContentTypeWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICWIZARDHEADER1, m_cHeader1);
	DDX_Control(pDX, IDC_RADIOSEARCHCONTENTTYPES, m_cAutoSearch);
	DDX_Control(pDX, IDC_RADIOMANUALCONTENTTYPES, m_cManualSearch);
}


BEGIN_MESSAGE_MAP(CAddContentTypeWelcomePage, CPropertyPage)	
END_MESSAGE_MAP()


// CAddContentTypeWelcomePage message handlers

BOOL CAddContentTypeWelcomePage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	CWnd* pHeader1 = GetDlgItem(IDC_STATICWIZARDHEADER1);
	ASSERT(pHeader1 != NULL);

	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();
	CFont* pftHeader = pSheet->GetHeaderFont(m_cHeader1.GetDC()->m_hDC);
	if (pftHeader != NULL)
	{
		m_cHeader1.SetFont(pftHeader);
	}

	m_cAutoSearch.SetCheck(BST_CHECKED);

	return TRUE;
}

BOOL CAddContentTypeWelcomePage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();   
	pSheet->SetWizardButtons(PSWIZB_NEXT);

	// we want help
	pSheet->EnableHelp(TRUE);

	return TRUE;
}

LRESULT CAddContentTypeWelcomePage::OnWizardNext(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();   

	if ((m_cAutoSearch.GetCheck() & BST_CHECKED) != 0)
	{
		pSheet->SetAddContentTypeWizardMode(CAddContentTypeWizardSheet::Auto);
	}
	else
	{
		pSheet->SetAddContentTypeWizardMode(CAddContentTypeWizardSheet::Manual);
	}

	return 0;
}