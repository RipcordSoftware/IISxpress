// ConfigAHADlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ConfigAHADlg.h"


// CConfigAHADlg dialog

IMPLEMENT_DYNAMIC(CConfigAHADlg, CHelpAwareDialogBase)

CConfigAHADlg::CConfigAHADlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CConfigAHADlg::IDD, pParent)
{

}

CConfigAHADlg::~CConfigAHADlg()
{
}

void CConfigAHADlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_AHABMP, m_cAHABmp);
	DDX_Control(pDX, IDC_CHECK_ENABLEAHA, m_cEnableAHA);	
	DDX_Control(pDX, IDC_EDIT_FPGATEMP, m_cFPGATemp);
	DDX_Control(pDX, IDC_EDIT_BOARDTEMP, m_cBoardTemp);
	DDX_Control(pDX, IDC_EDIT_BOARDDESCRIPTION, m_cBoardDesc);
}


BEGIN_MESSAGE_MAP(CConfigAHADlg, CHelpAwareDialogBase)
	ON_BN_CLICKED(IDC_CHECK_ENABLEAHA, &CConfigAHADlg::OnBnClickedCheckEnableAHA)
END_MESSAGE_MAP()


// CConfigAHADlg message handlers

BOOL CConfigAHADlg::OnInitDialog()
{
	BOOL status = CHelpAwareDialogBase::OnInitDialog();

	if (CPNGHelper::LoadPNG(_T("COMTECHAHA-SMALL.PNG"), m_imgAHA) == true)
	{
		CPNGHelper::ApplyAlpha(m_imgAHA);
		m_cAHABmp.SetWindowPos(NULL, 0, 0, m_imgAHA.GetWidth(), m_imgAHA.GetHeight(), 
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER);
		m_cAHABmp.SetImage(&m_imgAHA);
	}	

	UpdateControls();

	return status;
}

void CConfigAHADlg::UpdateControls(void)
{
	CIISxpressTrayPropertySheet* pSheet = GetIISxpressSheet();
	if (pSheet == NULL)
	{
		return;
	}

	DWORD dwAHAEnabled = 0;
	CComPtr<IComIISxpressRegSettings> pServerRegSettings;
	pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);
	if (pServerRegSettings != NULL)
	{				
		pServerRegSettings->get_AHAEnabled(&dwAHAEnabled);		
	}

	if (dwAHAEnabled != 0)
	{
		m_cEnableAHA.SetCheck(BST_CHECKED);
	}
	else
	{
		m_cEnableAHA.SetCheck(BST_UNCHECKED);
	}

	CString sBoardDesc;
	CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
	if (pSheet->GetHTTPRequest(&pHTTPRequest) == S_OK && pHTTPRequest != NULL)
	{
		CComQIPtr<IAHAState> pAHAState(pHTTPRequest);
		if (pAHAState != NULL)
		{			
			AHABoardInfo info;
			if (pAHAState->GetBoardInfo(&info) == S_OK)
			{
				sBoardDesc = info.szDescription;
			}		
		}
	}	

	m_cBoardDesc.SetWindowText(sBoardDesc);
}

void CConfigAHADlg::OnBnClickedCheckEnableAHA()
{
	CIISxpressTrayPropertySheet* pSheet = GetIISxpressSheet();
	if (pSheet == NULL)
	{
		return;
	}

	CComPtr<IComIISxpressRegSettings> pServerRegSettings;
	pSheet->GetIISxpressServerRegSettings(&pServerRegSettings);
	if (pServerRegSettings == NULL)
	{
		return;
	}

	if (m_cEnableAHA.GetCheck() == BST_CHECKED)
	{
		pServerRegSettings->put_AHAEnabled(1);
	}
	else
	{
		pServerRegSettings->put_AHAEnabled(0);
	}
}

CIISxpressTrayPropertySheet* CConfigAHADlg::GetIISxpressSheet()
{
	CIISxpressTrayPropertySheet* pSheet = NULL;

	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent = pParent->GetParent();
		if (pParent != NULL && pParent->IsKindOf(RUNTIME_CLASS(CIISxpressTrayPropertySheet)) == TRUE)
		{
			pSheet = (CIISxpressTrayPropertySheet*) pParent;
		}
	}

	return pSheet;
}

void CConfigAHADlg::Heartbeat(void)
{
	CString sFPGA;
	CString sBoard;

	CIISxpressTrayPropertySheet* pSheet = GetIISxpressSheet();
	if (pSheet != NULL)
	{		
		CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
		if (pSheet->GetHTTPRequest(&pHTTPRequest) == S_OK && pHTTPRequest != NULL)
		{
			CComQIPtr<IAHAState> pAHAState(pHTTPRequest);
			if (pAHAState != NULL)
			{
				DWORD dwFPGA = 0;
				DWORD dwBoard = 0;
				if (pAHAState->GetBoardTemps(&dwFPGA, &dwBoard) == S_OK)
				{					
					sFPGA.Format(_T("%u"), dwFPGA);										
					sBoard.Format(_T("%u"), dwBoard);					
				}				
			}
		}
	}

	m_cFPGATemp.SetWindowText(sFPGA);
	m_cBoardTemp.SetWindowText(sBoard);
}

void CConfigAHADlg::ServerOnline(void)
{
	m_cBoardDesc.EnableWindow(TRUE);
	m_cEnableAHA.EnableWindow(TRUE);

	m_cFPGATemp.EnableWindow(TRUE);
	m_cBoardTemp.EnableWindow(TRUE);

	UpdateControls();
}

void CConfigAHADlg::ServerOffline(void) 
{
	UpdateControls();

	m_cBoardDesc.EnableWindow(FALSE);
	m_cEnableAHA.EnableWindow(FALSE);

	m_cFPGATemp.EnableWindow(FALSE);
	m_cBoardTemp.EnableWindow(FALSE);
}