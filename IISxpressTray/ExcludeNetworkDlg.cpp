// ExcludeNetworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ExcludeNetworkDlg.h"
#include "AddIPAddressRuleDlg.h"

#include "RegistryKeys.h"

#include "EnumExclusionRuleHelper.h"

// CExcludeNetworkDlg dialog

IMPLEMENT_DYNAMIC(CExcludeNetworkDlg, CHelpAwareDialogBase)

CExcludeNetworkDlg::CExcludeNetworkDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CExcludeNetworkDlg::IDD, pParent), m_dwUpdateCookie(0)
{
}

CExcludeNetworkDlg::~CExcludeNetworkDlg()
{
}

void CExcludeNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelpAwareDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTNETWORKS, m_cNetworks);
	DDX_Control(pDX, IDC_BUTTONNETWORKADD, m_cAdd);
	DDX_Control(pDX, IDC_BUTTONNETWORKDELETE, m_cDelete);
	DDX_Control(pDX, IDC_CHECKCOMPRESSLOCALHOST, m_cCompressLocalhost);
}


BEGIN_MESSAGE_MAP(CExcludeNetworkDlg, CHelpAwareDialogBase)
	ON_BN_CLICKED(IDC_CHECKCOMPRESSLOCALHOST, OnBnClickedCheckCompressLocalhost)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTNETWORKS, OnLvnItemChangedListNetworks)
	ON_BN_CLICKED(IDC_BUTTONNETWORKADD, OnBnClickedButtonNetworkAdd)
	ON_BN_CLICKED(IDC_BUTTONNETWORKDELETE, OnBnClickedButtonNetworkDelete)
END_MESSAGE_MAP()


// CExcludeNetworkDlg message handlers

BOOL CExcludeNetworkDlg::OnInitDialog(void)
{
	CHelpAwareDialogBase::OnInitDialog();

	m_cNetworks.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString sNetwork;
	sNetwork.LoadString(IDS_COLUMN_NETWORK);
	m_cNetworks.InsertColumn(0, sNetwork, LVCFMT_LEFT, 300);

	// load the keyboard accelerators
	m_hAccelerator = ::LoadAccelerators(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_ACCELERATOR_EXCLUDENETWORK));

	UpdateControls();

	return TRUE;
}

BOOL CExcludeNetworkDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::TranslateAccelerator(GetSafeHwnd(), m_hAccelerator, pMsg) != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CExcludeNetworkDlg::UpdateControls(DWORD dwUpdateCookie)
{
	if (dwUpdateCookie == 0 || m_dwUpdateCookie != dwUpdateCookie)
	{
		UpdateControls();
	}
}

void CExcludeNetworkDlg::UpdateControls(void)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	BOOL bServerAvailable = FALSE;

	m_cNetworks.DeleteAllItems();

	CComPtr<ICompressionRuleManager> pCompressionRuleManager;
	if (pSheet->GetIISxpressInterfaces() == S_OK &&
		pSheet->GetCompressionManager(&pCompressionRuleManager) == S_OK && pCompressionRuleManager != NULL)
	{								
		CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
		if (pNeverCompressRules != NULL)
		{
			bServerAvailable = TRUE;			

			// read the 'compress localhost' state from the registry
			DWORD dwCompressLocalhost = 0;
			CRegKey FilterConfig;		
			if (FilterConfig.Open(HKEY_LOCAL_MACHINE, IISXPRESSFILTER_REGKEY, KEY_READ) == ERROR_SUCCESS)
			{
				m_cCompressLocalhost.EnableWindow(TRUE);
				
				FilterConfig.QueryDWORDValue(IISXPRESS_COMPRESSLOCALHOST, dwCompressLocalhost);			
			}

			// set the state of the checkbox
			if (dwCompressLocalhost == 1)
			{
				m_cCompressLocalhost.SetCheck(BST_UNCHECKED);	
			}
			else
			{				
				m_cCompressLocalhost.SetCheck(BST_CHECKED);
			}

			CComPtr<IEnumExclusionRule> pIPRules;
			if (pNeverCompressRules->EnumIPAddresses(&pIPRules) == S_OK && pIPRules != NULL)
			{
				ExclusionRuleInfo Info;
				ULONG nFetched = 0;
				while (pIPRules->Next(1, &Info, &nFetched) == S_OK)
				{
					m_cNetworks.InsertItem(m_cNetworks.GetItemCount(), CString(Info.bsRule));

					CEnumExclusionRuleHelper::DestroyExclusionRuleInfo(Info);
				}
			}

			m_dwUpdateCookie = pSheet->GetUpdateCookie();
		}
	}
	
	if (bServerAvailable == TRUE)
	{
		m_cNetworks.EnableWindow(TRUE);
		m_cAdd.EnableWindow(TRUE);
	}
	else
	{
		m_cCompressLocalhost.EnableWindow(FALSE);
		m_cNetworks.EnableWindow(FALSE);
		m_cAdd.EnableWindow(FALSE);
		m_cDelete.EnableWindow(FALSE);
	}
}

void CExcludeNetworkDlg::OnBnClickedCheckCompressLocalhost()
{
	CRegKey FilterConfig;
	if (FilterConfig.Open(HKEY_LOCAL_MACHINE, IISXPRESSFILTER_REGKEY, KEY_WRITE) != ERROR_SUCCESS)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERROR_FAILEDTOWRITECOMPRESSLOCALHOST);

		MessageBox(sMsg, sCaption, MB_OK | MB_ICONERROR);
		return;
	}

	// get the check state before we change it
	bool bChecked = ((m_cCompressLocalhost.GetCheck() & BST_CHECKED) == BST_CHECKED) ? true : false;

	// set the state of the check correctly
	if (bChecked == true)
	{
		m_cCompressLocalhost.SetCheck(BST_UNCHECKED);
		bChecked = false;
	}
	else
	{
		m_cCompressLocalhost.SetCheck(BST_CHECKED);
		bChecked = true;
	}

	// if check is set then we want to enable localhost compression
	BOOL bCompressLocalhost = (bChecked == true ? 0 : 1);

	FilterConfig.SetDWORDValue(IISXPRESS_COMPRESSLOCALHOST, bCompressLocalhost);
}

void CExcludeNetworkDlg::OnLvnItemChangedListNetworks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_cNetworks.GetSelectedCount() == 0)
	{
		m_cDelete.EnableWindow(FALSE);
	}
	else
	{
		m_cDelete.EnableWindow(TRUE);
	}

	*pResult = 0;
}

void CExcludeNetworkDlg::OnBnClickedButtonNetworkAdd()
{
	CAddIPAddressRuleDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		HRESULT hr = E_FAIL;

		CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

		CComPtr<ICompressionRuleManager> pCompressionRuleManager;
		if (pSheet->GetIISxpressInterfaces() == S_OK &&
			pSheet->GetCompressionManager(&pCompressionRuleManager) == S_OK && pCompressionRuleManager != NULL)
		{
			CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
			if (pNeverCompressRules != NULL)
			{
				// add the new rule
				hr = pNeverCompressRules->AddIPRule(CStringA(dlg.m_sNewIPAddress), m_dwUpdateCookie);
				
				if (hr == S_OK)
				{
					// we need to reset the compression cache since the config has changed		
					CIISxpressTrayPropertySheet::ResetResponseCache();

					// add the entry to the control
					m_cNetworks.InsertItem(m_cNetworks.GetItemCount(), dlg.m_sNewIPAddress);
				}								
			}
		}

		// we didn't succeed, inform the user
		if (hr != S_OK)
		{
			CString sCaption;
			sCaption.LoadString(IDS_IISXPRESS_APPNAME);

			CString sError;
			sError.LoadString(IDS_ERROR_IPADDRESSADDFAILED);

			MessageBox(sError, sCaption, MB_OK | MB_ICONWARNING);
		}
	}
}

void CExcludeNetworkDlg::OnBnClickedButtonNetworkDelete()
{
	int nSelectedItems = m_cNetworks.GetSelectedCount();
	if (nSelectedItems <= 0)
		return;

	CString sAppName;
	sAppName.LoadString(IDS_IISXPRESS_APPNAME);

	int nPrompt = -1;
	if (nSelectedItems == 1)
	{
		CString sMsg;
		sMsg.LoadString(IDS_REALLYDELETE_IPADDRESSRULE);

		nPrompt = MessageBox(sMsg, sAppName, MB_YESNO | MB_ICONQUESTION);
	}
	else
	{		
		CString sMsg;
		sMsg.LoadString(IDS_REALLYDELETE_IPADDRESSRULES);

		nPrompt = MessageBox(sMsg, sAppName, MB_YESNO | MB_ICONQUESTION);
	}

	if (nPrompt != IDYES)
		return;

	CDWordArray dwaItems;
	POSITION pos = m_cNetworks.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = m_cNetworks.GetNextSelectedItem(pos);

		dwaItems.InsertAt(0, nItem);
	}

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent()->GetParent();

	if (pSheet->GetIISxpressInterfaces() == S_OK)
	{
		CComPtr<ICompressionRuleManager> pCompressionRuleManager;
		pSheet->GetCompressionManager(&pCompressionRuleManager);
		if (pCompressionRuleManager != NULL)
		{
			CComQIPtr<INeverCompressRules> pNeverCompressRules = pCompressionRuleManager;
			if (pNeverCompressRules != NULL)
			{
				int nItems = (int) dwaItems.GetCount();
				for (int i = 0; i < nItems; i++)
				{
					int nItem = dwaItems[i];

					CString sIPAddress = m_cNetworks.GetItemText(nItem, 0);

					if (pNeverCompressRules->RemoveIPRule(CStringA(sIPAddress), m_dwUpdateCookie) == S_OK)
					{
						m_cNetworks.DeleteItem(nItem);
					}
				}
			}
		}
	}
}
