// AddContentTypeAutoAddSearchPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "AddContentTypeAutoAddSearchPage.h"

#include <atlpath.h>

// CAddContentTypeAutoAddSearchPage dialog

IMPLEMENT_DYNAMIC(CAddContentTypeAutoAddSearchPage, CPropertyPage)

CAddContentTypeAutoAddSearchPage::CAddContentTypeAutoAddSearchPage()
	: CPropertyPage(CAddContentTypeAutoAddSearchPage::IDD), 
	  m_SearchStop(FALSE, TRUE)
{
	m_pSheet = NULL;	

	m_pSearchThread = NULL;

	m_hAppStarting = ::LoadCursor(NULL, IDC_APPSTARTING);
}

CAddContentTypeAutoAddSearchPage::~CAddContentTypeAutoAddSearchPage()
{	
	if (m_pSearchThread != NULL)
	{
		delete m_pSearchThread;
	}
}

void CAddContentTypeAutoAddSearchPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEARCHDIRS, m_cDirectoriesSearched);
}


BEGIN_MESSAGE_MAP(CAddContentTypeAutoAddSearchPage, CPropertyPage)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CAddContentTypeAutoAddSearchPage message handlers
BOOL CAddContentTypeAutoAddSearchPage::OnInitDialog(void)
{
	CPropertyPage::OnInitDialog();

	CString sColumn;
	sColumn.LoadString(IDS_COLUMN_DIRECTORY);

	m_cDirectoriesSearched.InsertColumn(0, sColumn, LVCFMT_LEFT, 430);

	m_Inet = ::InternetOpen(_T("IISxpress/1.0"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);		

	return TRUE;
}

BOOL CAddContentTypeAutoAddSearchPage::OnSetActive(void)
{
	CAddContentTypeWizardSheet* pSheet = (CAddContentTypeWizardSheet*) GetParent();

	if (pSheet->GetAddContentTypeWizardMode() != CAddExtnWizardSheet::Auto)
		return FALSE;

	// we don't want help
	pSheet->EnableHelp(FALSE);

	// compare the newly selected instances to see if they match the previously
	// selected ones, if they do then we don't need to perform a search
	const CArray<CString, LPCTSTR>& Instances = pSheet->GetAutoSearchInstances();
	if (Instances.GetCount() == m_KnownInstances.GetCount())
	{
		// keep count of the number of matches we have
		int nMatches = 0;

		// loop through the instance arrays counting the matches
		for (int i = 0; i < m_KnownInstances.GetCount(); i++)
		{
			for (int k = 0; k < Instances.GetCount(); k++)
			{
				if (m_KnownInstances[i] == Instances[k])
				{
					nMatches++;
					break;
				}
			}
		}		

		// if the match count equals the number of known instances then the user
		// hasn't changed their selection since they were here last, so pass on the
		// opportunity to perform the search again
		if (nMatches == m_KnownInstances.GetCount())
		{
			return FALSE;
		}
	}

	// only allow the back button for the moment
	pSheet->SetWizardButtons(PSWIZB_BACK);

	// remove everything from the list control
	m_cDirectoriesSearched.DeleteAllItems();

	// get the metabase from the sheet
	CIISMetaBase* pMetaBase = *pSheet;		
	m_pSheet = pSheet;

	// make sure the search stop event is clear	
	m_SearchStop.ResetEvent();

	// get rid of the old thread data (if there was one)
	if (m_pSearchThread != NULL)
	{
		delete m_pSearchThread;
	}

	// create a thread to do the search
	m_pSearchThread = ::AfxBeginThread(SearchThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	
	if (m_pSearchThread != NULL)
	{
		// don't allow the thread to delete itself
		m_pSearchThread->m_bAutoDelete = FALSE;

		// start the thread
		m_pSearchThread->ResumeThread();
	}

	return TRUE;
}

LRESULT CAddContentTypeAutoAddSearchPage::OnWizardNext(void)
{
	// take a copy of the known instances
	m_KnownInstances.Copy(m_pSheet->GetAutoSearchInstances());

	return CPropertyPage::OnWizardNext();
}

UINT __cdecl CAddContentTypeAutoAddSearchPage::SearchThread(void* pData)
{
	CAddContentTypeAutoAddSearchPage* pThis = (CAddContentTypeAutoAddSearchPage*) pData;
	return pThis->SearchThread();
}

UINT CAddContentTypeAutoAddSearchPage::SearchThread(void)
{
	if (m_pSheet == NULL || m_Inet == NULL || m_SearchStop == NULL)
		return 0;

	// we are going to use COM
	::CoInitialize(NULL);

	CComPtr<IMSAdminBase> pAdminBase;
	CComPtr<IXMLDOMDocument> pDoc;
	if (g_VerInfo.dwMajorVersion < 6)
	{
		// get the metabase from the sheet
		CIISMetaBase* pMetaBase = *m_pSheet;	
				
		if (pMetaBase->GetAdminBase(&pAdminBase) == false)
		{
			::CoUninitialize();
			return 0;
		}
	}
	else
	{
		HRESULT hr = CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDoc);
		if (hr != S_OK || pDoc == NULL)
		{
			::CoUninitialize();
			return 0;
		}
	}	

	// get the instances from the sheet
	const CArray<CString, LPCTSTR>& saInstances = m_pSheet->GetAutoSearchInstances();

	// we need to keep track of known content types
	set<CString, CStringCompareNoCase> KnownContentTypes;

	// loop through the instances
	int nInstances = (int) saInstances.GetCount();
	bool bCancel = false;
	for (int i = 0; i < nInstances; i++)
	{
		// has a cancel been requested?
		if (::WaitForSingleObject(m_SearchStop, 0) != WAIT_TIMEOUT)
		{
			bCancel = true;
			break;
		}		

		// create a path to the metabase instance
		CString sInstancePath;
		
		if (g_VerInfo.dwMajorVersion < 6)
		{
			sInstancePath = _T("/LM/W3SVC/") + saInstances[i];
		}

		CComPtr<IXMLDOMNode> pSite;
		
		// get the server port and hostname (if there is one)
		DWORD dwPort;
		CString sHostname;
		if (g_VerInfo.dwMajorVersion < 6)
		{			
			if (CIIS6ConfigHelper::GetWebServerHostnameAndPort(pAdminBase, sInstancePath, sHostname, dwPort) == false)				
			{
				continue;
			}
		}
		else
		{			
			CIIS7XMLConfigHelper::GetSiteNode(pDoc, saInstances[i], &pSite);

			if (CIIS7XMLConfigHelper::GetWebServerHostnameAndPort(pSite, sHostname, dwPort) != S_OK)			
			{
				continue;
			}
		}
		

		// has a cancel been requested?
		if (::WaitForSingleObject(m_SearchStop, 0) != WAIT_TIMEOUT)
		{
			bCancel = true;
			break;
		}

		// if there was no hostname then assume it's localhost
		if (sHostname.GetLength() <= 0)
		{
			sHostname = _T("localhost");
		}

		map<wstring, wstring> WebSitePaths;		

		if (g_VerInfo.dwMajorVersion < 6)
		{		
			// create a metabase path to the instance root
			CStringW sInstanceRootPath(sInstancePath);
			sInstanceRootPath += L"/ROOT";

			// get the web server filesystem paths from the metabase			
			
			if (GetWebServerPathsFromMetaBase(pAdminBase, sInstanceRootPath, L"", WebSitePaths) == false)
				continue;
		}
		else
		{
			if (GetWebServerPathsFromXML(pSite, WebSitePaths) == false)
				continue;
		}

		// has a cancel been requested?
		if (::WaitForSingleObject(m_SearchStop, 0) != WAIT_TIMEOUT)
		{
			bCancel = true;
			break;
		}

		// we know the port, so connect to the web server (the hostname is applied in the HTTP header)
		HINTERNET hConnect = ::InternetConnect(m_Inet, _T("127.0.0.1"), 
			(INTERNET_PORT) dwPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

		if (hConnect != NULL)
		{
			// create an object to store the search state (NB. it owns the connect)
			InstanceContentTypeSearch Search(hConnect);			

			// for each of the filesystem directories we have found
			for (map<wstring, wstring>::const_iterator iter = WebSitePaths.begin(); iter != WebSitePaths.end(); iter++)
			{						
				// has a cancel been requested?
				if (::WaitForSingleObject(m_SearchStop, 0) != WAIT_TIMEOUT)
				{
					bCancel = true;
					break;
				}

				CString sFileSystemPath(iter->first.c_str());
				CString sURL(iter->second.c_str());

				// get the content types from the file system path
				GetPathContentTypes(Search, sFileSystemPath, sHostname, sURL);				

				// store the content types
				KnownContentTypes.insert(Search.ContentTypes.begin(), Search.ContentTypes.end());				
			}			

			if (bCancel == true)
				break;
		}
	}

	::CoUninitialize();

	if (bCancel == false)
	{
		// we need an array to store the candidates
		CArray<CString, LPCTSTR> saCandidateContentTypes;
		saCandidateContentTypes.SetSize(KnownContentTypes.size());

		// add the candidates from the set to the array
		int nIndex = 0;
		for (
			set<CString, CStringCompareNoCase>::const_iterator iter = KnownContentTypes.begin();
			iter != KnownContentTypes.end(); 
			iter++)
		{
			LPCTSTR pszContentType = *iter;

			saCandidateContentTypes[nIndex++] = pszContentType;
		}

		// store the candidates in the sheet
		m_pSheet->SetCandidateContentTypes(saCandidateContentTypes);

		// enable the next button
		m_pSheet->SetWizardButtons(PSWIZB_NEXT);
	
		// ask the sheet to move to the next page
		m_pSheet->PostMessage(PSM_PRESSBUTTON, PSBTN_NEXT, 0);	
	}	

	return 0;
}

bool CAddContentTypeAutoAddSearchPage::GetWebServerPathsFromMetaBase(
												IMSAdminBase* pAdminBase,
												LPCWSTR pszMBPath, 
												LPCWSTR pszURLPath,
												map<wstring, wstring>& WebSitePaths)
{
	if (pAdminBase == NULL || pszMBPath == NULL)
		return false;	

	// get the path of the key
	CIISMetaBaseData VRPath;
	if (VRPath.ReadData(pAdminBase, pszMBPath, MD_VR_PATH) == true)
	{
		wstring sPath;
		if (VRPath.GetAsString(sPath) == true && sPath.size() > 0)
		{			
			// store the filesystem path			
			WebSitePaths[sPath] = pszURLPath;
		}
	}

	// enumerate the keys of the current MB item
	vector<wstring> Keys;
	CIISMetaBase MetaBase(pAdminBase);
	if (MetaBase.EnumKeys(CString(pszMBPath), Keys) == true)
	{
		// for each of the child keys
		for (int i = 0; i < (int) Keys.size(); i++)
		{			
			// make the path to the child key
			CStringW sMBPath = pszMBPath;
			sMBPath += '/';
			sMBPath += Keys[i].c_str();

			CStringW sURLPath = pszURLPath;
			sURLPath += '/';
			sURLPath += Keys[i].c_str();

			// get the child filesystem paths
			GetWebServerPathsFromMetaBase(pAdminBase, sMBPath, sURLPath, WebSitePaths);
		}
	}

	return true;
}

bool CAddContentTypeAutoAddSearchPage::GetWebServerPathsFromXML(IXMLDOMNode* pSite, map<wstring, wstring>& WebSitePaths)
{
	if (pSite == NULL)
		return false;

	// get the 'applications' of the site
	CComPtr<IXMLDOMNodeList> pApplications;
	HRESULT hr = pSite->selectNodes(CComBSTR(L"application"), &pApplications);
	if (hr != S_OK || pApplications == NULL)
		return false;

	long nApplications = 0;
	hr = pApplications->get_length(&nApplications);

	// loop thru the applications
	for (long i = 0; i < nApplications; i++)
	{
		CComPtr<IXMLDOMNode> pApplicationNode;
		hr = pApplications->get_item(i, &pApplicationNode);
		if (hr != S_OK || pApplicationNode == NULL)
			continue;

		CComPtr<IXMLDOMNamedNodeMap> pApplicationAttribs;
		hr = pApplicationNode->get_attributes(&pApplicationAttribs);
		if (hr != S_OK || pApplicationAttribs == NULL)
			continue;
		
		CComPtr<IXMLDOMNode> pApplicationPathAttr;
		hr = pApplicationAttribs->getNamedItem(CComBSTR(L"path"), &pApplicationPathAttr);
		if (hr != S_OK || pApplicationPathAttr == NULL)
			continue;

		CComBSTR bsApplicationURI;
		hr = pApplicationPathAttr->get_text(&bsApplicationURI);

		CComPtr<IXMLDOMNodeList> pVirtualDirectories;
		hr = pApplicationNode->selectNodes(CComBSTR(L"virtualDirectory"), &pVirtualDirectories);
		if (hr != S_OK || pVirtualDirectories == NULL)
			continue;

		long nVirtualDirectories = 0;
		hr = pVirtualDirectories->get_length(&nVirtualDirectories);

		for (long j = 0; j < nVirtualDirectories; j++)
		{
			CComPtr<IXMLDOMNode> pVirtualDirectoryNode;
			hr = pVirtualDirectories->get_item(j, &pVirtualDirectoryNode);
			if (hr != S_OK || pVirtualDirectoryNode == NULL)
				continue;

			CComPtr<IXMLDOMNamedNodeMap> pVirtualDirectoryAttribs;
			hr = pVirtualDirectoryNode->get_attributes(&pVirtualDirectoryAttribs);
			if (hr != S_OK || pVirtualDirectoryAttribs == NULL)
				continue;

			CComPtr<IXMLDOMNode> pVirtualDirectoryPathNode;
			hr = pVirtualDirectoryAttribs->getNamedItem(CComBSTR(L"path"), &pVirtualDirectoryPathNode);

			CComBSTR bsVirtualDirectoryURI;
			hr = pVirtualDirectoryPathNode->get_text(&bsVirtualDirectoryURI);

			CComPtr<IXMLDOMNode> pPhysicalPathNode;
			hr = pVirtualDirectoryAttribs->getNamedItem(CComBSTR(L"physicalPath"), &pPhysicalPathNode);

			CComBSTR bsPhysicalPath;
			hr = pPhysicalPathNode->get_text(&bsPhysicalPath);

			wstring sURI;

			// we need to figure out on the URI required to get to the item
			if (bsApplicationURI == L"/")
			{
				// we are at the root of the app/web site, so the URI is the virtual dir portion
				sURI = bsVirtualDirectoryURI;
			}
			else if (bsVirtualDirectoryURI == L"/")
			{				
				// we are at the root of the app/virtual dir, so the URI is the app dir portion
				sURI = bsApplicationURI;
			}
			else
			{
				// we need both app and virtual directory strings
				sURI = bsApplicationURI;
				sURI += bsVirtualDirectoryURI;
			}

			wstring sPhysicalPath;
			CIIS7XMLConfigHelper::ExpandEnvironmentStrings(bsPhysicalPath, sPhysicalPath);

			WebSitePaths[sPhysicalPath] = sURI;
		}		
	}

	return true;
}

bool CAddContentTypeAutoAddSearchPage::GetPathContentTypes(
												InstanceContentTypeSearch& Search,
												LPCTSTR pszFileSystemPath, 
												LPCTSTR pszHostname, 
												LPCTSTR pszURLPath)
{
	if (Search.Connect == NULL || pszFileSystemPath == NULL || pszHostname == NULL || pszURLPath == NULL)
		return false;

#ifdef _DEBUG
	CString sDebugMsg;
	sDebugMsg.Format(_T("Searching dir: '%s', host: '%s', uri: '%s'\n"), pszFileSystemPath, pszHostname, pszURLPath);
	ATLTRACE(sDebugMsg);
#endif

	// check that we haven't already been to this part of the file system
	if (Search.FileSystemPathsSearched.find(pszFileSystemPath) != Search.FileSystemPathsSearched.end())
	{
		#ifdef _DEBUG
		ATLTRACE(_T("^^^ Already searched\n"));
		#endif
		return true;
	}

	// update the list control
	int nItem = m_cDirectoriesSearched.InsertItem(m_cDirectoriesSearched.GetItemCount(), pszFileSystemPath);
	m_cDirectoriesSearched.EnsureVisible(nItem, FALSE);

	CPathT<CString> FileSystemPath(pszFileSystemPath);
	FileSystemPath.Append(_T("*.*"));

	// start the search
	WIN32_FIND_DATA FindData;	
	HANDLE hFind = ::FindFirstFile(FileSystemPath, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return true;

	CArray<CString, LPCTSTR> SubDirectories;

	// guess that we will have 32 sub-dirs (this can grow)
	SubDirectories.SetSize(32);

	// keep track of the number of sub-dirs
	int nSubDirs = 0;

	bool bCancel = false;
	do
	{
		// has a cancel been requested?
		if (::WaitForSingleObject(m_SearchStop, 0) != WAIT_TIMEOUT)
		{		
			bCancel = true;
			break;
		}		

		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			// ignore files beginning with '.' (like . and ..)
			if (FindData.cFileName[0] != '.')
			{
				SubDirectories.SetAtGrow(nSubDirs, FindData.cFileName);
				nSubDirs++;
			}
		}
		else
		{
			LPCTSTR pszExtension = _tcsrchr(FindData.cFileName, '.');
			if (pszExtension == NULL || 
				Search.FileExtensionsSearched.find(pszExtension) == Search.FileExtensionsSearched.end())
			{						
				CString sURL = pszURLPath;

				if (sURL.Right(1) != '/')
					sURL += '/';

				sURL += FindData.cFileName;			

				CString sContentType;
				if (GetFileContentType(Search.Connect, pszHostname, sURL, sContentType) == true)
				{
					Search.ContentTypes.insert(sContentType);				
				}

				if (pszExtension != NULL)
				{
					Search.FileExtensionsSearched.insert(pszExtension);
				}
			}
		}
	}
	while (::FindNextFile(hFind, &FindData) == TRUE);

	// get rid of the find handle
	::FindClose(hFind);
	hFind = NULL;

	if (bCancel == false)
	{
		// mark this directory as having been searched
		Search.FileSystemPathsSearched.insert(pszFileSystemPath);

		// for all the sub-dirs we encountered: get their content types too 
		for (int i = 0; i < nSubDirs; i++)
		{		
			// make the sub-dir URL
			CString sURLPath = pszURLPath;
			sURLPath += '/';
			sURLPath += SubDirectories[i];

			CPathT<CString> sDirectoryPath(pszFileSystemPath);
			sDirectoryPath.Append(SubDirectories[i]);

			GetPathContentTypes(Search, sDirectoryPath, pszHostname, sURLPath);
		}
	}

	return true;
}

bool CAddContentTypeAutoAddSearchPage::GetFileContentType(
												HINTERNET hConnect, 
												LPCTSTR pszHostname,
												LPCTSTR pszURL, 
												CString& sContentType)
{
	bool bStatus = false;

	if (hConnect == NULL || pszHostname == NULL || pszURL == NULL || m_Inet == NULL)
		return bStatus;

	CAutoHINTERNET Request = ::HttpOpenRequest(hConnect, _T("HEAD"), pszURL, 
		_T("HTTP/1.0"), NULL, NULL, INTERNET_FLAG_RELOAD, 0);	

	if (Request != NULL)
	{
		// we need to override the host header in order to get the correct site
		TCHAR szHostHeader[256] = _T("Host: ");
		_tcscat_s(szHostHeader, pszHostname);
		_tcscat_s(szHostHeader, _T("\r\n"));		

		if (::HttpSendRequest(Request, szHostHeader, (DWORD) _tcslen(szHostHeader), NULL, 0) == TRUE)
		{
			DWORD dwStatus = 0;
			DWORD dwStatusBufferSize = sizeof(dwStatus);
			DWORD dwStatusIndex = 0;

			TCHAR szContentType[128];
			DWORD dwContentTypeBufferSize = _countof(szContentType);
			DWORD dwContentTypeIndex = 0;

			if (::HttpQueryInfo(Request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwStatusBufferSize, &dwStatusIndex) == TRUE)
			{
				if (dwStatus == 200 &&
					::HttpQueryInfo(Request, HTTP_QUERY_CONTENT_TYPE, szContentType, &dwContentTypeBufferSize, &dwContentTypeIndex) == TRUE)
				{	
					// make sure the string is NULL terminated
					szContentType[dwContentTypeBufferSize] = '\0';

					// find the first space character, then terminate the string there
					TCHAR* pszSeperator = _tcschr(szContentType, ' ');
					if (pszSeperator != NULL)
					{
						*pszSeperator = '\0';
					}

					// find the first semi-color character, then terminate the string there
					pszSeperator = _tcschr(szContentType, ';');
					if (pszSeperator != NULL)
					{
						*pszSeperator = '\0';
					}

					sContentType = CString(szContentType);
					bStatus = true;
				}
			}
		}		
	}

	return bStatus;
}

void CAddContentTypeAutoAddSearchPage::OnCancel(void)
{
	// we want an hourglass
	CWaitCursor Cursor;

	// ask for a stop
	StopSearchThread();	

	// allow the cancel now
	CPropertyPage::OnCancel();
}

LRESULT CAddContentTypeAutoAddSearchPage::OnWizardBack(void)
{
	// we want an hourglass
	CWaitCursor Cursor;

	// ask for a stop
	StopSearchThread();

	// go to the previous page
	return CPropertyPage::OnWizardBack();
}

void CAddContentTypeAutoAddSearchPage::StopSearchThread(void)
{
	ASSERT(m_pSearchThread != NULL);
	ASSERT(m_SearchStop != NULL);

	// if a stop has already been requested then don't do anything
	if (::WaitForSingleObject(m_SearchStop, 0) != WAIT_TIMEOUT)
		return;

	HANDLE hThread = *m_pSearchThread;
	if (::WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT)
	{
		// ask for the search to be stopped		
		m_SearchStop.SetEvent();

		// wait until the thread exits - allow the message pump to run tho		
		while (::MsgWaitForMultipleObjects(1, &hThread, TRUE, 1, QS_ALLEVENTS) == WAIT_TIMEOUT)
		{			
			MSG msg;
			while (::PeekMessage(&msg, GetSafeHwnd(), 0, 0, TRUE) == TRUE)
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}
}

void CAddContentTypeAutoAddSearchPage::OnMouseMove(UINT nFlags, CPoint point)
{
	ASSERT(m_hAppStarting != NULL);
	::SetCursor(m_hAppStarting);
}

BOOL CAddContentTypeAutoAddSearchPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	ASSERT(m_hAppStarting != NULL);
	::SetCursor(m_hAppStarting);
	return TRUE;
}