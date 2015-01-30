// AddExtensionRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddExtensionRuleDlg.h"

// CAddExtensionRuleDlg dialog

IMPLEMENT_DYNAMIC(CAddExtensionRuleDlg, CHelpAwareDialogBase)

CAddExtensionRuleDlg::CAddExtensionRuleDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CAddExtensionRuleDlg::IDD, pParent)
{
}

CAddExtensionRuleDlg::~CAddExtensionRuleDlg()
{
}

void CAddExtensionRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_EXTENSIONRULE, m_cExtension);
}


BEGIN_MESSAGE_MAP(CAddExtensionRuleDlg, CHelpAwareDialogBase)
END_MESSAGE_MAP()


// CAddExtensionRuleDlg message handlers

void CAddExtensionRuleDlg::Init(const CStringArray& saExcludedExtensions)
{
	m_saExcludedExtensions.Copy(saExcludedExtensions);
}

void CAddExtensionRuleDlg::OnOK(void)
{
	CString sAppName;
	sAppName.LoadString(IDS_IISXPRESS_APPNAME);

	m_cExtension.GetWindowText(m_sNewExtension);
	if (m_sNewExtension.GetLength() <= 0)
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_NEEDEXTENSION);
	
		MessageBox(sMsg, sAppName, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_sNewExtension.MakeLower();

	int nAlreadyExcluded = (int) m_saExcludedExtensions.GetCount();
	for (int i = 0; i < nAlreadyExcluded; i++)
	{
		if (m_saExcludedExtensions[i].Compare(m_sNewExtension) == 0)
		{
			CString sMsg;
			sMsg.LoadString(IDS_ERROR_EXTENSIONALREADYEXCLUDED);
			
			MessageBox(sMsg, sAppName, MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	CDialog::OnOK();
}