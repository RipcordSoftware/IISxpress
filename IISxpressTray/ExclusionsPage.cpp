// ExclusionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ExclusionsPage.h"

#include "resource.h"

#define EXCLUSIONITEM_EXTENSIONS	1
#define EXCLUSIONITEM_CONTENTTYPES	2
#define EXCLUSIONITEM_FOLDERS		3
#define EXCLUSIONITEM_USERAGENTS	4
#define EXCLUSIONITEM_NETWORK		5

#define WM_MYUPDATECONTROLS (WM_USER + 1)

// CExclusionsPage dialog

IMPLEMENT_DYNAMIC(CExclusionsPage, CIISxpressPageBase)

CExclusionsPage::CExclusionsPage()
	: CIISxpressPageBase(CExclusionsPage::IDD), m_pNotify(NULL)
{
	m_pExtensions = NULL;
	m_pContentTypes = NULL;
	m_pFolders = NULL;
	m_pNetwork = NULL;
	m_pUserAgents = NULL;
}

CExclusionsPage::~CExclusionsPage()
{
	if (m_pExtensions != NULL)
	{
		delete m_pExtensions;
	}

	if (m_pContentTypes != NULL)
	{
		delete m_pContentTypes;
	}

	if (m_pFolders != NULL)
	{
		delete m_pFolders;
	}

	if (m_pNetwork != NULL)
	{
		delete m_pNetwork;
	}

	if (m_pUserAgents != NULL)
	{
		delete m_pUserAgents;
	}
}

void CExclusionsPage::DoDataExchange(CDataExchange* pDX)
{
	CIISxpressPageBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EXCLUSIONTYPES, m_cExclusionTypes);
	DDX_Control(pDX, IDC_CHILDFRAME, m_cChildFrame);
}

BEGIN_MESSAGE_MAP(CExclusionsPage, CIISxpressPageBase)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EXCLUSIONTYPES, OnLvnItemChangedListExclusionTypes)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MYUPDATECONTROLS, OnUpdateControls)
END_MESSAGE_MAP()

// CExclusionsPage message handlers

BOOL CExclusionsPage::OnInitDialog(void)
{
	BOOL bStatus = CPropertyPage::OnInitDialog();

	// ***********************************************************

	CRect rcChildFrame;
	m_cChildFrame.GetWindowRect(rcChildFrame);
	ScreenToClient(rcChildFrame);

	m_pExtensions = new CExcludeExtensionsDlg();
	m_pExtensions->Create(CExcludeExtensionsDlg::IDD, this);
	m_pExtensions->MoveWindow(rcChildFrame);	
	m_pExtensions->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pExtensions->ShowWindow(SW_HIDE);

	m_pContentTypes = new CExcludeContentTypesDlg();
	m_pContentTypes->Create(CExcludeContentTypesDlg::IDD, this);
	m_pContentTypes->MoveWindow(rcChildFrame);	
	m_pContentTypes->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pContentTypes->ShowWindow(SW_HIDE);

	m_pFolders = new CExcludeFoldersDlg();
	m_pFolders->Create(CExcludeFoldersDlg::IDD, this);
	m_pFolders->MoveWindow(rcChildFrame);	
	m_pFolders->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pFolders->ShowWindow(SW_HIDE);

	m_pNetwork = new CExcludeNetworkDlg();
	m_pNetwork->Create(CExcludeNetworkDlg::IDD, this);
	m_pNetwork->MoveWindow(rcChildFrame);	
	m_pNetwork->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pNetwork->ShowWindow(SW_HIDE);

	m_pUserAgents = new CExcludeUserAgentsDlg();
	m_pUserAgents->Create(CExcludeUserAgentsDlg::IDD, this);
	m_pUserAgents->MoveWindow(rcChildFrame);	
	m_pUserAgents->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	m_pUserAgents->ShowWindow(SW_HIDE);

	// ***********************************************************

	m_Icons.Create(32, 32, ILC_COLOR24 | ILC_MASK, 1, 8);	

	m_ExtensionsBmp.LoadBitmap(IDB_BITMAP_FILEEXTENSIONS);
	m_Icons.Add(&m_ExtensionsBmp, RGB(255, 0, 255));

	m_ContentTypeBmp.LoadBitmap(IDB_BITMAP_CONTENT);
	m_Icons.Add(&m_ContentTypeBmp, RGB(255, 0, 255));

	m_URIBmp.LoadBitmap(IDB_BITMAP_URI);
	m_Icons.Add(&m_URIBmp, RGB(255, 0, 255));

	m_UserAgentBmp.LoadBitmap(IDB_BITMAP_USERAGENT);
	m_Icons.Add(&m_UserAgentBmp, RGB(255, 0, 255));

	m_NetworkBmp.LoadBitmap(IDB_BITMAP_NETWORK);
	m_Icons.Add(&m_NetworkBmp, RGB(255, 0, 255));	

	m_cExclusionTypes.SetImageList(&m_Icons, LVSIL_NORMAL);

	// override the default icon spacing
	//m_cExclusionTypes.SetIconSpacing(80, 42);

	CString sExtensions;
	sExtensions.LoadString(IDS_EXCLUSION_EXTENSIONS);
	int nExtensionItem = m_cExclusionTypes.InsertItem(0, sExtensions, 0);	
	m_cExclusionTypes.SetItemData(nExtensionItem, EXCLUSIONITEM_EXTENSIONS);

	CString sContentTypes;
	sContentTypes.LoadString(IDS_EXCLUSION_CONTENTYPES);
	int nContentTypeItem = m_cExclusionTypes.InsertItem(1, sContentTypes, 1);
	m_cExclusionTypes.SetItemData(nContentTypeItem, EXCLUSIONITEM_CONTENTTYPES);

	CString sFolders;
	sFolders.LoadString(IDS_EXCLUSION_FOLDERS);
	int nFolderItem = m_cExclusionTypes.InsertItem(2, sFolders, 2);
	m_cExclusionTypes.SetItemData(nFolderItem, EXCLUSIONITEM_FOLDERS);	

	CString sUserAgents;
	sUserAgents.LoadString(IDS_EXCLUSION_USERAGENTS);
	int nUserAgentsItems = m_cExclusionTypes.InsertItem(3, sUserAgents, 3);
	m_cExclusionTypes.SetItemData(nUserAgentsItems, EXCLUSIONITEM_USERAGENTS);

	CString sNetwork;
	sNetwork.LoadString(IDS_EXCLUSION_NETWORK);
	int nNetworkItem = m_cExclusionTypes.InsertItem(4, sNetwork, 4);
	m_cExclusionTypes.SetItemData(nNetworkItem, EXCLUSIONITEM_NETWORK);	

	m_cExclusionTypes.SetItemState(nExtensionItem, LVIS_SELECTED, LVIS_SELECTED);

	if (m_pNotify == NULL)
	{
		CComObject<CNeverCompressRulesNotify<CExclusionsPage> >::CreateInstance(&m_pNotify);
		m_pNotify->AddRef();
		m_pNotify->Init(this);
	}

	RegisterConnectionPoint();

	return bStatus;
}

void CExclusionsPage::UpdateControls(void)
{
	if (m_pExtensions != NULL)
	{
		m_pExtensions->UpdateControls();
	}

	if (m_pContentTypes != NULL)
	{
		m_pContentTypes->UpdateControls();
	}

	if (m_pFolders != NULL)
	{
		m_pFolders->UpdateControls();
	}

	if (m_pNetwork != NULL)
	{
		m_pNetwork->UpdateControls();
	}

	if (m_pUserAgents != NULL)
	{
		m_pUserAgents->UpdateControls();
	}
}

void CExclusionsPage::ServerOnline(void)
{
	RegisterConnectionPoint();

	UpdateControls();
}

void CExclusionsPage::ServerOffline(void)
{
	UpdateControls();
}

void CExclusionsPage::Heartbeat(void)
{
}

void CExclusionsPage::OnLvnItemChangedListExclusionTypes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ((pNMLV->uChanged & LVIF_STATE) != 0)
	{
		POSITION pos = m_cExclusionTypes.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nSel = m_cExclusionTypes.GetNextSelectedItem(pos);
			DWORD dwExclusionType = (DWORD) m_cExclusionTypes.GetItemData(nSel);

			if (dwExclusionType == EXCLUSIONITEM_EXTENSIONS)
			{
				m_pExtensions->ShowWindow(SW_SHOW);
				m_pContentTypes->ShowWindow(SW_HIDE);
				m_pFolders->ShowWindow(SW_HIDE);
				m_pNetwork->ShowWindow(SW_HIDE);
				m_pUserAgents->ShowWindow(SW_HIDE);
			}
			else if (dwExclusionType == EXCLUSIONITEM_CONTENTTYPES)
			{
				m_pExtensions->ShowWindow(SW_HIDE);
				m_pContentTypes->ShowWindow(SW_SHOW);
				m_pFolders->ShowWindow(SW_HIDE);
				m_pNetwork->ShowWindow(SW_HIDE);
				m_pUserAgents->ShowWindow(SW_HIDE);
			}
			else if (dwExclusionType == EXCLUSIONITEM_FOLDERS)
			{
				m_pExtensions->ShowWindow(SW_HIDE);
				m_pContentTypes->ShowWindow(SW_HIDE);
				m_pFolders->ShowWindow(SW_SHOW);
				m_pNetwork->ShowWindow(SW_HIDE);
				m_pUserAgents->ShowWindow(SW_HIDE);
			}
			else if (dwExclusionType == EXCLUSIONITEM_NETWORK)
			{
				m_pExtensions->ShowWindow(SW_HIDE);
				m_pContentTypes->ShowWindow(SW_HIDE);
				m_pFolders->ShowWindow(SW_HIDE);
				m_pNetwork->ShowWindow(SW_SHOW);
				m_pUserAgents->ShowWindow(SW_HIDE);
			}

			else if (dwExclusionType == EXCLUSIONITEM_USERAGENTS)
			{
				m_pExtensions->ShowWindow(SW_HIDE);
				m_pContentTypes->ShowWindow(SW_HIDE);
				m_pFolders->ShowWindow(SW_HIDE);
				m_pNetwork->ShowWindow(SW_HIDE);
				m_pUserAgents->ShowWindow(SW_SHOW);
			}
			else
			{			
				m_pExtensions->ShowWindow(SW_HIDE);
				m_pContentTypes->ShowWindow(SW_HIDE);
				m_pFolders->ShowWindow(SW_HIDE);
				m_pNetwork->ShowWindow(SW_HIDE);
				m_pUserAgents->ShowWindow(SW_HIDE);
			}
		}
	}

	*pResult = 0;
}


void CExclusionsPage::OnDestroy(void)
{
	UnregisterConnectionPoint();

	if (m_pNotify != NULL)
	{	
		m_pNotify->Release();
		m_pNotify = NULL;
	}
}

bool CExclusionsPage::RegisterConnectionPoint(void)
{
	if (m_pNotify == NULL)
		return false;

	UnregisterConnectionPoint();
	m_pNeverCompressRulesNotifyCP.Release();

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pRuleManager;
	if (pSheet->GetCompressionManager(&pRuleManager) == S_OK)
	{
		CComQIPtr<IConnectionPointContainer> pConnPointCont = pRuleManager;
		if (pConnPointCont != NULL)
		{				
			if (pConnPointCont != NULL)
			{					
				HRESULT hr = pConnPointCont->FindConnectionPoint(IID_INeverCompressRulesNotify, &m_pNeverCompressRulesNotifyCP);
				if (m_pNeverCompressRulesNotifyCP != NULL)
				{
					CComPtr<IUnknown> pUnk;
					m_pNotify->QueryInterface(IID_IUnknown, (void**) &pUnk);

					m_dwCookie = 0;
					hr = m_pNeverCompressRulesNotifyCP->Advise(pUnk, &m_dwCookie);
				}
			}
		}
	}

	return true;
}

bool CExclusionsPage::UnregisterConnectionPoint(void)
{
	if (m_pNeverCompressRulesNotifyCP == NULL || m_dwCookie == -1)
		return false;

	HRESULT hr = m_pNeverCompressRulesNotifyCP->Unadvise(m_dwCookie);
	if (hr != S_OK)
	{
		// now panic since the remote will not be releasing the ref count it has been holding,
		// so get rid of all ref counts except 1
		if (m_pNotify != NULL)
		{
			m_pNotify->Lock();

			while (m_pNotify->m_dwRef > 1)
			{
				m_pNotify->Release();
			}

			m_pNotify->Unlock();
		}
	}

	m_dwCookie = -1;

	return true;
}

HRESULT CExclusionsPage::OnRuleChanged(NeverRuleChangeHint hint, DWORD dwUpdateCookie)
{	
	PostMessage(WM_MYUPDATECONTROLS, hint, dwUpdateCookie);

	return S_OK;
}

LRESULT CExclusionsPage::OnUpdateControls(WPARAM hint, LPARAM lUpdateCookie)
{
	if (hint == NeverRuleChangeHintExtension && m_pExtensions != NULL)
	{
		m_pExtensions->UpdateControls((DWORD) lUpdateCookie);
	}
	else if (hint == NeverRuleChangeHintContentType && m_pContentTypes != NULL)
	{
		m_pContentTypes->UpdateControls((DWORD) lUpdateCookie);
	}
	else if (hint == NeverRuleChangeHintURI && m_pFolders != NULL)
	{
		m_pFolders->UpdateControls((DWORD) lUpdateCookie);
	}
	else if (hint == NeverRuleChangeHintIP && m_pNetwork != NULL)
	{
		m_pNetwork->UpdateControls((DWORD) lUpdateCookie);
	}
	else if (hint == NeverRuleChangeHintUserAgent && m_pUserAgents != NULL)
	{
		m_pUserAgents->UpdateControls((DWORD) lUpdateCookie);
	}

	return 0;
}