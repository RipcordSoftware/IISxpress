// AddExtnFinishPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddExtnFinishPage.h"


// CAddExtnFinishPage dialog

IMPLEMENT_DYNAMIC(CAddExtnFinishPage, CPropertyPage)
CAddExtnFinishPage::CAddExtnFinishPage()
	: CPropertyPage(CAddExtnFinishPage::IDD)
{
}

CAddExtnFinishPage::~CAddExtnFinishPage()
{
}

void CAddExtnFinishPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICWIZARDHEADER1, m_cHeader1);
}


BEGIN_MESSAGE_MAP(CAddExtnFinishPage, CPropertyPage)
END_MESSAGE_MAP()


// CAddExtnFinishPage message handlers

BOOL CAddExtnFinishPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	CFont* pftHeader = pSheet->GetHeaderFont();
	if (pftHeader != NULL)
	{
		m_cHeader1.SetFont(pftHeader);
	}

	return TRUE;
}

BOOL CAddExtnFinishPage::OnSetActive(void)
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_FINISH);

	// disable the cancel button
	CWnd* pwndCancel = pSheet->GetDlgItem(IDCANCEL);
	if (pwndCancel != NULL)
	{
		pwndCancel->EnableWindow(FALSE);
	}

	// disable the help button
	pSheet->EnableHelp(FALSE);

	return TRUE;
}