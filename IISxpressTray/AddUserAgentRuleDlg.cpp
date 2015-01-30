// AddUserAgentRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddUserAgentRuleDlg.h"

#include "UserAgentLookup.h"

// CAddUserAgentRuleDlg dialog

IMPLEMENT_DYNAMIC(CAddUserAgentRuleDlg, CHelpAwareDialogBase)

CAddUserAgentRuleDlg::CAddUserAgentRuleDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CAddUserAgentRuleDlg::IDD, pParent)
{

}

CAddUserAgentRuleDlg::~CAddUserAgentRuleDlg()
{
}

void CAddUserAgentRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_UA_PREDEFINED, m_cPredefinedRadio);
	DDX_Control(pDX, IDC_RADIO_UA_CUSTOM, m_cCustomRadio);
	DDX_Control(pDX, IDC_COMBO_UA_PREDEFINED, m_cPredefinedCombo);
	DDX_Control(pDX, IDC_COMBO_UA_CUSTOM, m_cCustomCombo);
	DDX_Control(pDX, IDC_CHECK_UA_INCLUDE, m_cInclude);
	DDX_Control(pDX, IDOK, m_cOK);
}


BEGIN_MESSAGE_MAP(CAddUserAgentRuleDlg, CHelpAwareDialogBase)
	ON_BN_CLICKED(IDC_RADIO_UA_PREDEFINED, &CAddUserAgentRuleDlg::OnBnClickedRadioUAPredefined)
	ON_BN_CLICKED(IDC_RADIO_UA_CUSTOM, &CAddUserAgentRuleDlg::OnBnClickedRadioUACustom)
	ON_CBN_SELCHANGE(IDC_COMBO_UA_PREDEFINED, &CAddUserAgentRuleDlg::OnCbnSelchangeComboUAPredefined)
	ON_CBN_SELCHANGE(IDC_COMBO_UA_CUSTOM, &CAddUserAgentRuleDlg::OnCbnSelchangeComboUACustom)
	ON_BN_CLICKED(IDC_CHECK_UA_INCLUDE, &CAddUserAgentRuleDlg::OnBnClickedCheckUAInclude)
	ON_CBN_EDITCHANGE(IDC_COMBO_UA_CUSTOM, &CAddUserAgentRuleDlg::OnCbnEditChangeComboUACustom)
END_MESSAGE_MAP()

// CAddUserAgentRuleDlg message handlers

BOOL CAddUserAgentRuleDlg::OnInitDialog()
{
	BOOL status = CHelpAwareDialogBase::OnInitDialog();

	m_cPredefinedRadio.SetCheck(BST_CHECKED);

	m_cPredefinedCombo.SetExtendedUI(TRUE);

	for (int i = 0; i < g_numberOfUserAgents; i++)	
	{
		UserAgentInitData* pUserAgentData = &g_UAInitData[i];

		LPCSTR pszUserAgentName = pUserAgentData->pszUserAgentLongName;
		if (pszUserAgentName == NULL)
		{
			pszUserAgentName = pUserAgentData->pszUserAgentName;
		}

		int nItem = m_cPredefinedCombo.AddString(CString(pszUserAgentName));
		m_cPredefinedCombo.SetItemData(nItem, (DWORD_PTR) pUserAgentData);
	}	

	m_cPredefinedCombo.SetCurSel(0);

	for (int i = 0; i < g_numberOfUserAgents; i++)
	{
		UserAgentInitData* pUserAgentData = &g_UAInitData[i];

		int nItem = m_cCustomCombo.AddString(CString(pUserAgentData->pszUserAgentString));
		m_cCustomCombo.SetItemData(nItem, (DWORD_PTR) pUserAgentData);
	}

	return status;
}

void CAddUserAgentRuleDlg::OnBnClickedRadioUAPredefined()
{
	m_cPredefinedCombo.EnableWindow(TRUE);	
	m_cCustomCombo.EnableWindow(FALSE);

	UpdateOKButtonState(false);
}

void CAddUserAgentRuleDlg::OnBnClickedRadioUACustom()
{
	m_cPredefinedCombo.EnableWindow(FALSE);	
	m_cCustomCombo.EnableWindow(TRUE);

	UpdateOKButtonState(false);
}

void CAddUserAgentRuleDlg::OnCbnSelchangeComboUAPredefined()
{
	UpdateOKButtonState(false);
}

void CAddUserAgentRuleDlg::OnCbnSelchangeComboUACustom()
{
	UpdateOKButtonState(false);
}

void CAddUserAgentRuleDlg::OnBnClickedCheckUAInclude()
{
	// TODO: Add your control notification handler code here
}

void CAddUserAgentRuleDlg::OnOK()
{
	if (m_cPredefinedRadio.GetCheck() == BST_CHECKED)
	{
		int nSelected = m_cPredefinedCombo.GetCurSel();
		if (nSelected == CB_ERR)
		{
			return;
		}

		UserAgentInitData* pSelectedAgent = (UserAgentInitData*) m_cPredefinedCombo.GetItemData(nSelected);

		selectedUserAgent = CString(pSelectedAgent->pszUserAgentString);
	}
	else
	{
		CString sUserAgentString;
		m_cCustomCombo.GetWindowText(sUserAgentString);
		if (sUserAgentString.GetLength() <= 0)
		{
			return;
		}

		HttpUserAgent::UserAgentProducts<Ripcord::Types::tstring> userAgent;
		const Ripcord::Types::tstring::value_type* pszUserAgentString = sUserAgentString;
		if (userAgent.ParseUserAgentString(pszUserAgentString) != S_OK)
		{
			return;
		}

		selectedUserAgent = sUserAgentString;
	}	

	// handle the inversion
	if (m_cInclude.GetCheck() == BST_CHECKED)
	{
		selectedUserAgent = _T("!") + selectedUserAgent;
	}

	CHelpAwareDialogBase::OnOK();
}

void CAddUserAgentRuleDlg::OnCbnEditChangeComboUACustom()
{
	UpdateOKButtonState(true);
}

void CAddUserAgentRuleDlg::UpdateOKButtonState(bool edited)
{
	BOOL status = FALSE;

	if (m_cPredefinedRadio.GetCheck() == BST_CHECKED)
	{
		status = TRUE;
	}
	else
	{
		CString sUserAgentString;

		if (edited)
		{
			m_cCustomCombo.GetWindowText(sUserAgentString);
		}
		else
		{
			int nSel = m_cCustomCombo.GetCurSel();		
			if (nSel != CB_ERR)
			{
				m_cCustomCombo.GetLBText(nSel, sUserAgentString);
			}
		}		

		if (sUserAgentString.GetLength() > 0)
		{
			HttpUserAgent::UserAgentProducts<Ripcord::Types::tstring> userAgent;
			const Ripcord::Types::tstring::value_type* pszUserAgentString = sUserAgentString;
			if (userAgent.ParseUserAgentString(pszUserAgentString) == S_OK)
			{					
				status = TRUE;
			}
		}
	}

	m_cOK.EnableWindow(status);
}