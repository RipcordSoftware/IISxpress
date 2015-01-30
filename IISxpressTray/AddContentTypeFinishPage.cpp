// AddContentTypeFinishPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeFinishPage.h"


// CAddContentTypeFinishPage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeFinishPage, CPropertyPage)

CAddContentTypeFinishPage::CAddContentTypeFinishPage()
	: CPropertyPage(CAddContentTypeFinishPage::IDD)
{
}

CAddContentTypeFinishPage::~CAddContentTypeFinishPage()
{
}

void CAddContentTypeFinishPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICWIZARDHEADER1, m_cHeader1);
}


BEGIN_MESSAGE_MAP(CAddContentTypeFinishPage, CPropertyPage)
END_MESSAGE_MAP()


// CAddContentTypeFinishPage message handlers

BOOL CAddContentTypeFinishPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	CFont* pftHeader = pSheet->GetHeaderFont();
	if (pftHeader != NULL)
	{
		m_cHeader1.SetFont(pftHeader);
	}

	return TRUE;
}

BOOL CAddContentTypeFinishPage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	pSheet->SetWizardButtons(PSWIZB_FINISH);

	// disable the cancel button
	CWnd* pwndCancel = pSheet->GetDlgItem(IDCANCEL);
	if (pwndCancel != NULL)
	{
		pwndCancel->EnableWindow(FALSE);
	}

	// we don't want help
	pSheet->EnableHelp(FALSE);

	return TRUE;
}