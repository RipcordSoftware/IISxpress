// AddContentTypeSelectWebSitesPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeSelectWebSitesPage.h"


// CAddContentTypeSelectWebSitesPage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeSelectWebSitesPage, CPropertyPage)

CAddContentTypeSelectWebSitesPage::CAddContentTypeSelectWebSitesPage()
	: CPropertyPage(CAddContentTypeSelectWebSitesPage::IDD)
{
}

CAddContentTypeSelectWebSitesPage::~CAddContentTypeSelectWebSitesPage()
{
}

void CAddContentTypeSelectWebSitesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTWEBSITES, m_cWebSites);
	DDX_Control(pDX, IDC_RADIO_ALLWEBSITES, m_cAllWebSites);
	DDX_Control(pDX, IDC_RADIO_SELECTWEBSITES, m_cSelectWebSites);
}


BEGIN_MESSAGE_MAP(CAddContentTypeSelectWebSitesPage, CPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_ALLWEBSITES, OnBnClickedRadioAllWebSites)
	ON_BN_CLICKED(IDC_RADIO_SELECTWEBSITES, OnBnClickedRadioSelectWebSites)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTWEBSITES, OnLvnItemChangedListWebSites)
END_MESSAGE_MAP()


// CAddContentTypeSelectWebSitesPage message handlers

BOOL CAddContentTypeSelectWebSitesPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	/************************************************************************************/

	// ask for full-row select and checkboxes
	m_cWebSites.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_INSTANCE);
	m_cWebSites.InsertColumn(0, sColumn, LVCFMT_LEFT, 80);

	sColumn.LoadString(IDS_COLUMN_WEBSITEDESC);
	m_cWebSites.InsertColumn(1, sColumn, LVCFMT_LEFT, 140);

	sColumn.LoadString(IDS_COLUMN_WEBSITEPATH);
	m_cWebSites.InsertColumn(2, sColumn, LVCFMT_LEFT, 210);	

	/************************************************************************************/

	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();
	
	if (g_VerInfo.dwMajorVersion < 6)
	{
		CIISMetaBase* pMetabase = *pSheet;
		CComPtr<IMSAdminBase> pAdminBase;
		if (pMetabase != NULL && pMetabase->GetAdminBase(&pAdminBase) == true)
		{
			vector<wstring> Keys;
			if (pMetabase->EnumKeys(_T("/LM/W3SVC"), Keys) == true)
			{
				int nKeys = (int) Keys.size();
				for (int i = 0; i < nKeys; i++)
				{
					wstring sKey = Keys[i];

					wstring sKeyPath = L"/LM/W3SVC/";
					sKeyPath += sKey;

					CIISMetaBaseData KeyType;
					if (KeyType.ReadData(pAdminBase, sKeyPath.c_str(), MD_KEY_TYPE) == false)
						continue;

					wstring sKeyType;
					if (KeyType.GetAsString(sKeyType) == false)
						continue;

					if (sKeyType.compare(L"IIsWebServer") != 0)
						continue;				

					wstring sComment;
					CIISMetaBaseData Comment;
					if (Comment.ReadData(pAdminBase, sKeyPath.c_str(), MD_SERVER_COMMENT) == true)
					{					
						Comment.GetAsString(sComment);					
					}

					wstring sVRPath;
					wstring sRootKeyPath = sKeyPath + L"/ROOT";
					CIISMetaBaseData VRPath;
					if (VRPath.ReadData(pAdminBase, sRootKeyPath.c_str(), MD_VR_PATH) == true)
						VRPath.GetAsString(sVRPath);				

					int nItem = m_cWebSites.InsertItem(m_cWebSites.GetItemCount(), CString(sKey.c_str()));

					m_cWebSites.SetItemText(nItem, 1, CString(sComment.c_str()));

					m_cWebSites.SetItemText(nItem, 2, CString(sVRPath.c_str()));
				}
			}
		}
	}
	else
	{
		CComPtr<IXMLDOMDocument> pDoc;
		CComPtr<IXMLDOMNodeList> pSites;			
		if (pSheet->GetXMLConfigDoc(&pDoc) == S_OK && 
			CIIS7XMLConfigHelper::GetSitesCollection(pDoc, &pSites) == S_OK)
		{			
			long nSites = 0;
			pSites->get_length(&nSites);

			for (long i = 0; i < nSites; i++)
			{
				CComPtr<IXMLDOMNode> pSite;
				HRESULT hr = pSites->get_item(i, &pSite);
				if (hr != S_OK || pSite == NULL)
					continue;

				CAtlString sId;
				CAtlString sDesc;
				CAtlString sPhysicalPath;
				CAtlArray<CAtlString> Ports;
				CAtlArray<CAtlString> SecurePorts;
				if (CIIS7XMLConfigHelper::GetSiteInfo(pSite, sId, sDesc, sPhysicalPath, Ports, SecurePorts) == S_OK)
				{
					int nItem = m_cWebSites.InsertItem(m_cWebSites.GetItemCount(), sId);
					m_cWebSites.SetItemText(nItem, 1, sDesc);
					m_cWebSites.SetItemText(nItem, 2, sPhysicalPath);
				}
			}
		}
	}
		

	/************************************************************************************/

	m_cAllWebSites.SetCheck(BST_CHECKED);

	UpdateControls();

	return TRUE;
}

BOOL CAddContentTypeSelectWebSitesPage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	if (pSheet->GetAddContentTypeWizardMode() != CAddContentTypeWizardSheet::Auto)
		return FALSE;	

	// we want help
	pSheet->EnableHelp(TRUE);

	UpdateControls();

	return TRUE;
}

void CAddContentTypeSelectWebSitesPage::OnBnClickedRadioAllWebSites()
{
	UpdateControls();
}

void CAddContentTypeSelectWebSitesPage::OnBnClickedRadioSelectWebSites()
{
	UpdateControls();
}

void CAddContentTypeSelectWebSitesPage::GetCheckedWebSites(CDWordArray& dwaItems)
{
	dwaItems.RemoveAll();

	int nItems = m_cWebSites.GetItemCount();

	for (int i = 0; i < nItems; i++)
	{
		BOOL bCheck = m_cWebSites.GetCheck(i);
		if (bCheck == TRUE)
		{
			dwaItems.Add(i);
		}
	}
}

void CAddContentTypeSelectWebSitesPage::UpdateControls(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	if ((m_cAllWebSites.GetCheck() & BST_CHECKED) != 0)
	{
		m_cWebSites.EnableWindow(FALSE);

		pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
	else
	{
		m_cWebSites.EnableWindow(TRUE);

		CDWordArray dwaChecks;
		GetCheckedWebSites(dwaChecks);

		if (dwaChecks.GetCount() > 0)
			pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		else
			pSheet->SetWizardButtons(PSWIZB_BACK);
	}
}

void CAddContentTypeSelectWebSitesPage::OnLvnItemChangedListWebSites(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	
	*pResult = 0;

	UpdateControls();
}

LRESULT CAddContentTypeSelectWebSitesPage::OnWizardNext(void)
{
	BOOL bAllChecked = (m_cAllWebSites.GetCheck() & BST_CHECKED) != 0 ? TRUE : FALSE;

	CArray<CString, LPCTSTR> saSelectedInstances;

	int nItems = m_cWebSites.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		if (bAllChecked == TRUE || m_cWebSites.GetCheck(i) == TRUE)
		{
			CString sInstance = m_cWebSites.GetItemText(i, 0);
			saSelectedInstances.Add(sInstance);			
		}
	}
	
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();
	pSheet->SetAutoSearchInstances(saSelectedInstances);

	return 0;
}