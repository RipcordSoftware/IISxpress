// AddExtnAutoAddSelectExtnsPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddExtnAutoAddSelectExtnsPage.h"

#include <atlpath.h>
#include ".\addextnautoaddselectextnspage.h"

// CAddExtnAutoAddSelectExtnsPage dialog

IMPLEMENT_DYNAMIC(CAddExtnAutoAddSelectExtnsPage, CPropertyPage)

CAddExtnAutoAddSelectExtnsPage::CAddExtnAutoAddSelectExtnsPage()
	: CPropertyPage(CAddExtnAutoAddSelectExtnsPage::IDD)
{
}

CAddExtnAutoAddSelectExtnsPage::~CAddExtnAutoAddSelectExtnsPage()
{
}

void CAddExtnAutoAddSelectExtnsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTSELECTEXTNS, m_cExtensions);
}


BEGIN_MESSAGE_MAP(CAddExtnAutoAddSelectExtnsPage, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTSELECTEXTNS, OnLvnItemChangedListSelectExtns)
END_MESSAGE_MAP()


// CAddExtnAutoAddSelectExtnsPage message handlers

BOOL CAddExtnAutoAddSelectExtnsPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	// we want full row select and checkboxes
	m_cExtensions.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);	

	// set the shell image list in
	m_cExtensions.SetImageList(g_ShellImageList, LVSIL_SMALL);
	
	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_EXTENSION);
	m_cExtensions.InsertColumn(0, sColumn, LVCFMT_LEFT, 120);

	sColumn.LoadString(IDS_COLUMN_EXTNDESC);
	m_cExtensions.InsertColumn(1, sColumn, LVCFMT_LEFT, 300);

	return TRUE;
}

BOOL CAddExtnAutoAddSelectExtnsPage::OnSetActive(void)
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	if (pSheet->GetAddExtnWizardMode() != CAddExtnWizardSheet::Auto)
		return FALSE;

	// we want the help button
	pSheet->EnableHelp(TRUE);

	CWaitCursor Cursor;

	// get the previously selected extensions
	CArray<CString, LPCTSTR> saSelectedExtensions;
	GetSelectedExtensions(saSelectedExtensions);

	// create a fast lookup for previously selected extensions
	CMap<CString, LPCTSTR, bool, bool> PreviouslySelectedExtensions;
	for (int i = 0; i < saSelectedExtensions.GetCount(); i++)
	{
		CString sExtension = saSelectedExtensions[i];
		PreviouslySelectedExtensions[sExtension] = true;
	}

	// remove all items from the list
	m_cExtensions.DeleteAllItems();

	// we need a collection of all the web server paths (for VDs, etc.)
	set<wstring> WebSitePaths;

	// get the metabase from the sheet
	CIISMetaBase* pMetaBase = *pSheet;

	// get the instances from the sheet
	const CArray<CString, LPCTSTR>& saInstances = pSheet->GetAutoSearchInstances();

	// loop through all the instances
	int nInstances = (int) saInstances.GetCount();
	bool bClear = true;
	for (int i = 0; i < nInstances; i++)
	{
		if (g_VerInfo.dwMajorVersion < 6)
		{
			CStringW sMBPath = L"/LM/W3SVC/";
			sMBPath += saInstances[i];
			sMBPath += L"/ROOT";

			// get the paths for each of the instances
			GetWebServerPathsFromMetaBase(pMetaBase, sMBPath, WebSitePaths, bClear);

			// don't clear the array next time
			bClear = false;
		}
		else
		{
			CComPtr<IXMLDOMDocument> pDoc;
			CComPtr<IXMLDOMNode> pSiteNode;
			if (pSheet->GetXMLConfigDoc(&pDoc) == S_OK &&
				CIIS7XMLConfigHelper::GetSiteNode(pDoc, saInstances[i], &pSiteNode) == S_OK)
			{
				CComPtr<IXMLDOMNodeList> pPhysicalPathNodes;
				HRESULT hr = pSiteNode->selectNodes(CComBSTR(L"application/virtualDirectory/@physicalPath"), &pPhysicalPathNodes);

				long nPhysicalPathNodes = 0;
				hr = pPhysicalPathNodes->get_length(&nPhysicalPathNodes);

				for (long j = 0; j < nPhysicalPathNodes; j++)
				{
					CComPtr<IXMLDOMNode> pPhysicalPathNode;
					hr = pPhysicalPathNodes->get_item(j, &pPhysicalPathNode);
					if (hr != S_OK || pPhysicalPathNode == NULL)
						continue;

					CComBSTR bsPhysicalPath;
					hr = pPhysicalPathNode->get_text(&bsPhysicalPath);

					wstring sPhysicalPath;
					CIIS7XMLConfigHelper::ExpandEnvironmentStrings(bsPhysicalPath, sPhysicalPath);

					WebSitePaths.insert(sPhysicalPath);
				}
			}
		}
	}

	// get a list of file extensions for each of the directories in the web server
	InstanceExtensionSearch Search;	
	for (set<wstring>::const_iterator iter = WebSitePaths.begin(); iter != WebSitePaths.end(); iter++)	
	{
		LPCWSTR pszWebSitePath = iter->c_str();

		GetPathExtensions(Search, pszWebSitePath);		
	}	

	// don't allow a redraw when we are doing the inserting
	m_cExtensions.SetRedraw(FALSE);

	// look through the found extensions	
	for (set<CStringW, CStringWCompareNoCase>::const_iterator iter = Search.FileExtensionsFound.begin(); 
		iter != Search.FileExtensionsFound.end(); 
		iter++)
	{
		// strip off the leading '.'
		LPCWSTR pszExtension = *iter;
		if (pszExtension[0] == '.')
			pszExtension++;		

		// get a multi-byte/unicode friendly version of the extension
		CString sExt(pszExtension);

		// get the image offset in the imagelist
		int nOffset = g_ShellImageList.GetExtensionOffset(sExt);

		CString sDesc;
		g_ShellImageList.GetExtensionDescription(sExt, sDesc);

		// add the item to the list control
		int nItem = m_cExtensions.InsertItem(m_cExtensions.GetItemCount(), sExt, nOffset);

		// set in the description
		m_cExtensions.SetItemText(nItem, 1, sDesc);

		// if the check was set previously then set it again
		bool bTemp;
		if (PreviouslySelectedExtensions.Lookup(sExt, bTemp) == TRUE)
		{
			m_cExtensions.SetCheck(nItem, TRUE);
		}		
	}

	// enable redraw again
	m_cExtensions.SetRedraw(TRUE);

	// we've changed stuff, set the wizard buttons right
	UpdateControls();

	return TRUE;
}

bool CAddExtnAutoAddSelectExtnsPage::GetPathExtensions(InstanceExtensionSearch& Search, LPCWSTR pszPath)
{
	if (pszPath == NULL || wcslen(pszPath) <= 0)
		return false;

#ifdef _DEBUG
	CStringW sDebugMsg;
	sDebugMsg.Format(L"Searching dir: '%ls'\n", pszPath);
	ATLTRACE(sDebugMsg);
#endif	

	// check that we haven't already been to this part of the file system
	if (Search.FileSystemPathsSearched.find(pszPath) != Search.FileSystemPathsSearched.end())
	{
		#ifdef _DEBUG
		ATLTRACE(_T("^^^ Already searched\n"));
		#endif
		return true;
	}

	vector<wstring> saDirectories;

	// make the search path
	CPathT<CStringW> Path(pszPath);
	Path.Append(L"*.*");

	// start the search
	WIN32_FIND_DATAW FindData;	
	HANDLE hFind = ::FindFirstFileW(Path, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return true;
	
	do
	{		
		CPathT<CStringW> File(pszPath);
		File.Append(FindData.cFileName);

		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{						
			// ignore dirs beginning with '.' (like . and ..)
			if (FindData.cFileName[0] != '.')			
			{
				// we have found a sub-dir, store for later
				saDirectories.push_back((LPCWSTR) File);
			}
		}
		else
		{
			// we have a file, get its extension
			CStringW sExtension = File.GetExtension();
			if (sExtension.GetLength() > 0)
			{
				sExtension.MakeLower();
				Search.FileExtensionsFound.insert((LPCWSTR) sExtension);
			}
		}
	}
	while (::FindNextFileW(hFind, &FindData) == TRUE);

	// get rid of the find handle
	::FindClose(hFind);
	hFind = NULL;

	// mark this directory as having been searched
	Search.FileSystemPathsSearched.insert(pszPath);

	// for a all the sub-dirs we encountered: get their extensions too 
	for (int i = 0; i < (int) saDirectories.size(); i++)
	{
		GetPathExtensions(Search, saDirectories[i].c_str());
	}

	return true;
}

LRESULT CAddExtnAutoAddSelectExtnsPage::OnWizardNext(void)
{
	CArray<CString, LPCTSTR> saSelectedExtensions;
	GetSelectedExtensions(saSelectedExtensions);

	if (saSelectedExtensions.GetCount() <= 0)
		return -1;

	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	pSheet->SetSelectedExtensions(saSelectedExtensions);

	return 0;
}

bool CAddExtnAutoAddSelectExtnsPage::GetSelectedExtensions(CArray<CString, LPCTSTR>& saSelectedExtensions)
{
	saSelectedExtensions.RemoveAll();

	int nItems = m_cExtensions.GetItemCount();
	for (int i = 0; i < nItems; i++)
	{
		if (m_cExtensions.GetCheck(i) == TRUE)
		{
			CString sExtension = m_cExtensions.GetItemText(i, 0);
			saSelectedExtensions.Add(sExtension);
		}
	}

	return true;
}
void CAddExtnAutoAddSelectExtnsPage::OnLvnItemChangedListSelectExtns(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateControls();
}

void CAddExtnAutoAddSelectExtnsPage::UpdateControls()
{
	CAddExtnWizardSheet* pSheet = (CAddExtnWizardSheet*) GetParent();

	CArray<CString, LPCTSTR> saSelectedExtensions;
	GetSelectedExtensions(saSelectedExtensions);

	if (saSelectedExtensions.GetCount() == 0)
	{
		pSheet->SetWizardButtons(PSWIZB_BACK);
	}
	else
	{
		pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
}

bool CAddExtnAutoAddSelectExtnsPage::GetWebServerPathsFromMetaBase(
															CIISMetaBase* pMetaBase,
															LPCWSTR pszMBPath, 
															set<wstring>& WebSitePaths, 
															bool bClear)
{
	if (pMetaBase == NULL || pszMBPath == NULL)
		return false;

	CComPtr<IMSAdminBase> pAdminBase;
	if (pMetaBase->GetAdminBase(&pAdminBase) != true || pAdminBase == NULL)
		return false;

	if (bClear == true)
	{
		WebSitePaths.clear();
	}

	// get the path of the key
	CIISMetaBaseData VRPath;
	if (VRPath.ReadData(pAdminBase, pszMBPath, MD_VR_PATH) == true)
	{
		wstring sPath;
		if (VRPath.GetAsString(sPath) == true && sPath.size() > 0)
		{
			// store the filesystem path
			WebSitePaths.insert(sPath.c_str());
		}
	}

	// enumerate the keys of the current MB item
	vector<wstring> Keys;
	if (pMetaBase->EnumKeys(CString(pszMBPath), Keys) == true)
	{
		// for each of the child keys
		for (int i = 0; i < (int) Keys.size(); i++)
		{
			// make the path to the child key
			CStringW sMBPath = pszMBPath;
			sMBPath += '/';
			sMBPath += Keys[i].c_str();

			// get the child filesystem paths
			GetWebServerPathsFromMetaBase(pMetaBase, sMBPath, WebSitePaths, false);
		}
	}

	return true;
}