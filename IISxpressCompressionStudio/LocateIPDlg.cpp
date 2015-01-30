// LocateIPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "LocateIPDlg.h"


// CLocateIPDlg dialog

IMPLEMENT_DYNAMIC(CLocateIPDlg, CDialog)

CLocateIPDlg::CLocateIPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocateIPDlg::IDD, pParent)
{

}

CLocateIPDlg::~CLocateIPDlg()
{
}

void CLocateIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_LOCATE, m_cIPAddress);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_cLocation);
}


BEGIN_MESSAGE_MAP(CLocateIPDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOCATE, &CLocateIPDlg::OnBnClickedButtonLocate)
END_MESSAGE_MAP()


// CLocateIPDlg message handlers

void CLocateIPDlg::OnBnClickedButtonLocate()
{	
	DWORD dwAddress = 0;
	m_cIPAddress.GetAddress(dwAddress);

	LPCTSTR pszCountry = CGeoIPHelper::LookupCountry(dwAddress);
	if (pszCountry != NULL)
	{
		m_cLocation.SetWindowText(pszCountry);
	}
	else
	{
		m_cLocation.SetWindowText(_T(""));	
	}
}
