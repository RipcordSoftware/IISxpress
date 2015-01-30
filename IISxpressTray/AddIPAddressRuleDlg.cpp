// AddIPAddressRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddIPAddressRuleDlg.h"

#include <atlrx.h>

// CAddIPAddressRuleDlg dialog

IMPLEMENT_DYNAMIC(CAddIPAddressRuleDlg, CHelpAwareDialogBase)
CAddIPAddressRuleDlg::CAddIPAddressRuleDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CAddIPAddressRuleDlg::IDD, pParent)
{
}

CAddIPAddressRuleDlg::~CAddIPAddressRuleDlg()
{
}

void CAddIPAddressRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IPRULE, m_cIPAddress);
}


BEGIN_MESSAGE_MAP(CAddIPAddressRuleDlg, CHelpAwareDialogBase)
END_MESSAGE_MAP()


// CAddIPAddressRuleDlg message handlers

void CAddIPAddressRuleDlg::OnOK(void)
{
	CString sIPAddress;
	m_cIPAddress.GetWindowText(sIPAddress);		

	// this reg exp should allow entry of an IP address of up to 4 octets
	CAtlRegExp<> re;
	re.Parse(_T("{[0-2]?\\d?\\d}{\\.[0-2]?\\d?\\d}?{\\.[0-2]?\\d?\\d}?{\\.[0-2]?\\d?\\d}?"));

	CAtlREMatchContext<> mc;
	const TCHAR* pszStart = sIPAddress;
	const TCHAR* pszEnd = NULL;
	if (sIPAddress.GetLength() <= 0 ||
		re.Match(sIPAddress, &mc, &pszEnd) == FALSE || ((int) (pszEnd - pszStart)) != sIPAddress.GetLength())
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sError;
		sError.LoadString(IDS_ERROR_INCORRECTIPADDRESS);

		MessageBox(sError, sCaption, MB_OK | MB_ICONWARNING);
		m_cIPAddress.SetFocus();
		return;
	}	

	m_sNewIPAddress = sIPAddress;

	CHelpAwareDialogBase::OnOK();
}