// AddContentTypeRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeRuleDlg.h"


// CAddContentTypeRuleDlg dialog

IMPLEMENT_DYNAMIC(CAddContentTypeRuleDlg, CHelpAwareDialogBase)

CAddContentTypeRuleDlg::CAddContentTypeRuleDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CAddContentTypeRuleDlg::IDD, pParent)
{
}

CAddContentTypeRuleDlg::~CAddContentTypeRuleDlg()
{
}

void CAddContentTypeRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CONTENTRULE, m_cContentType);
}


BEGIN_MESSAGE_MAP(CAddContentTypeRuleDlg, CHelpAwareDialogBase)
END_MESSAGE_MAP()


// CAddContentTypeRuleDlg message handlers

void CAddContentTypeRuleDlg::Init(const CStringArray& saExcludedContentTypes)
{
	m_saExcludedContentTypes.Copy(saExcludedContentTypes);
}

void CAddContentTypeRuleDlg::OnOK(void)
{
	CString sAppName;
	sAppName.LoadString(IDS_IISXPRESS_APPNAME);

	m_cContentType.GetWindowText(m_sNewContentType);
	if (m_sNewContentType.GetLength() <= 0)
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_NEEDCONTENTTYPE);
	
		MessageBox(sMsg, sAppName, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_sNewContentType.MakeLower();

	int nAlreadyExcluded = (int) m_saExcludedContentTypes.GetCount();
	for (int i = 0; i < nAlreadyExcluded; i++)
	{
		if (m_saExcludedContentTypes[i].Compare(m_sNewContentType) == 0)
		{
			CString sMsg;
			sMsg.LoadString(IDS_ERROR_CONTENTTYPEALREADYEXCLUDED);
			
			MessageBox(sMsg, sAppName, MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	CDialog::OnOK();
}