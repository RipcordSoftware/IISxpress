// AddFolderRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddFolderRuleDlg.h"

#include "..\IISxpressServer\IISxpressServer.h"

// CAddFolderRuleDlg dialog

IMPLEMENT_DYNAMIC(CAddFolderRuleDlg, CHelpAwareDialogBase)
CAddFolderRuleDlg::CAddFolderRuleDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CAddFolderRuleDlg::IDD, pParent)
{
	m_dwFlags = RULEFLAGS_FOLDER_WILDCARDMATCH;
}

CAddFolderRuleDlg::~CAddFolderRuleDlg()
{
}

void CAddFolderRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FOLDERRULE, m_cFolder);
	DDX_Control(pDX, IDC_CHECKWILDCARD, m_cWildcard);
}


BEGIN_MESSAGE_MAP(CAddFolderRuleDlg, CHelpAwareDialogBase)
END_MESSAGE_MAP()


// CAddFolderRuleDlg message handlers

BOOL CAddFolderRuleDlg::OnInitDialog(void)
{
	BOOL bStatus = CHelpAwareDialogBase::OnInitDialog();	

	m_cFolder.SetWindowText(_T("/"));
	m_cWildcard.SetCheck(BST_CHECKED);

	return bStatus;
}

void CAddFolderRuleDlg::Init(const CStringArray& saExcludedFolders)
{
	m_saExcludedFolders.Copy(saExcludedFolders);
}

void CAddFolderRuleDlg::OnOK(void)
{
	CString sCaption;
	sCaption.LoadString(IDS_IISXPRESS_APPNAME);

	CString sFolder;
	m_cFolder.GetWindowText(sFolder);

	if (sFolder.GetLength() <= 0)
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_ADDFOLDER_EMPTYPATH);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONEXCLAMATION);

		m_cFolder.SetFocus();

		return;
	}

	if (sFolder[0] != '/')
	{
		CString sMsg;
		sMsg.LoadString(IDS_ERROR_ADDFOLDER_BADPATH);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONEXCLAMATION);

		m_cFolder.SetFocus();

		return;
	}

	for (int i = 0; i < m_saExcludedFolders.GetCount(); i++)
	{
		if (sFolder.CompareNoCase(m_saExcludedFolders[i]) == 0)
		{
			CString sMsg;
			sMsg.LoadString(IDS_ERROR_ADDFOLDER_ALREADYADDED);

			MessageBox(sMsg, sCaption, MB_OK | MB_ICONEXCLAMATION);

			return;
		}
	}

	m_sFolder = sFolder;

	if ((m_cWildcard.GetCheck() & BST_CHECKED) == BST_CHECKED)
	{
		m_dwFlags = RULEFLAGS_FOLDER_WILDCARDMATCH;
	}
	else
	{
		m_dwFlags = RULEFLAGS_FOLDER_EXACTMATCH;
	}

	CHelpAwareDialogBase::OnOK();
}