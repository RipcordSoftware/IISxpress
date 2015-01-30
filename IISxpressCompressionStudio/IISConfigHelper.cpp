#include "StdAfx.h"
#include "IISConfigHelper.h"

#include <atlpath.h>

#include "IIS7XMLConfigHelper.h"

const CString CIISConfigHelper::EmptyNodeString(_T("."));
const CStringA CIISConfigHelper::EmptyNodeStringA(".");
const CStringW CIISConfigHelper::EmptyNodeStringW(L".");

CIISConfigHelper::CIISConfigHelper(void)
{
}

CIISConfigHelper::~CIISConfigHelper(void)
{
}

BOOL CIISConfigHelper::EnumWebSitesFromMetaBase(std::vector<IISWebSite>& sites)
{
	sites.clear();

	CIISMetaBase MetaBase;
	CComPtr<IMSAdminBase> pAdminBase;
	std::vector<std::wstring> Keys;

	// connect to the metabase
	if (MetaBase.Init() == false ||
		MetaBase.GetAdminBase(&pAdminBase) == false ||
		MetaBase.EnumKeys(_T("/LM/W3SVC"), Keys) == false)
	{
		return FALSE;
	}

	// loop thru the metabase keys
	int nKeys = (int) Keys.size();
	for (int i = 0; i < nKeys; i++)
	{
		std::wstring sKey = Keys[i];

		std::wstring sKeyPath = L"/LM/W3SVC/";
		sKeyPath += sKey;

		CIISMetaBaseData KeyType;
		if (KeyType.ReadData(pAdminBase, sKeyPath.c_str(), MD_KEY_TYPE) == false)
			continue;

		std::wstring sKeyType;
		if (KeyType.GetAsString(sKeyType) == false)
			continue;

		// we are looking for web servers
		if (sKeyType.compare(L"IIsWebServer") != 0)
			continue;

		CString sDescription = CString(sKey.c_str());

		std::wstring sComment;
		CIISMetaBaseData Description;
		if (Description.ReadData(pAdminBase, sKeyPath.c_str(), MD_SERVER_COMMENT) == true)
		{			
			Description.GetAsString(sComment);			
		}

		std::wstring sKeyPathForVR = sKeyPath + L"/ROOT";		

		CIISMetaBaseData VRPath;
		if (VRPath.ReadData(pAdminBase, sKeyPathForVR.c_str(), MD_VR_PATH) == false)
			continue;

		std::wstring sVRPath;
		if (VRPath.GetAsString(sVRPath) != true)
			continue;		

		IISWebSite item;
		item.eSource = IISWebSite::Metabase;
		item.sInstance = sKey.c_str();
		item.sMetaBasePath = sKeyPath.c_str();		
		item.sDescription = sComment.c_str();
		item.sFileSystemPath = sVRPath.c_str();			

		CIISMetaBaseData ServerBindings;
		if (ServerBindings.ReadData(pAdminBase, sKeyPath.c_str(), MD_SERVER_BINDINGS) == true)
		{
			std::vector<std::wstring> Ports;
			if (ServerBindings.GetAsStringVector(Ports) == true)			
			{
				size_t nPorts = Ports.size();
				for (size_t i = 0; i < nPorts; i++)
				{
					if (Ports[i].size() == 0)
						continue;

					IISWebSiteBindings bindings(Ports[i].c_str());			
					item.Ports.Add(bindings);			
				}
			}
		}

		CIISMetaBaseData SecureBindings;
		if (SecureBindings.ReadData(pAdminBase, sKeyPath.c_str(), MD_SECURE_BINDINGS) == true)
		{
			std::vector<std::wstring> SecurePorts;
			if (SecureBindings.GetAsStringVector(SecurePorts) == true)
			{
				size_t nSecurePorts = SecurePorts.size();
				for (size_t i = 0; i < nSecurePorts; i++)
				{
					if (SecurePorts[i].size() == 0)
						continue;			

					IISWebSiteBindings bindings(SecurePorts[i].c_str());
					item.SecurePorts.Add(bindings);
				}				
			}
		}

		sites.push_back(item);
	}	

	return TRUE;
}

BOOL CIISConfigHelper::EnumWebSitesFromXML(std::vector<IISWebSite>& sites)
{
	sites.clear();

	// connect to the config file
	CComPtr<IXMLDOMDocument> pDOM;
	HRESULT hr = CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDOM);
	if (hr != S_OK || pDOM == NULL)
		return FALSE;

	// get the site collection
	CComPtr<IXMLDOMNodeList> pSites;
	hr = CIIS7XMLConfigHelper::GetSitesCollection(pDOM, &pSites);
	if (hr != S_OK)
		return FALSE;

	long nSites = 0;
	pSites->get_length(&nSites);

	// loop thru the available sites
	for (long i = 0; i < nSites; i++)
	{
		CComPtr<IXMLDOMNode> pSite;
		hr = pSites->get_item(i, &pSite);
		if (hr != S_OK || pSite == NULL)
			continue;

		IISWebSite item;		
		item.eSource = IISWebSite::XML;

		CAtlArray<CAtlString> Ports;
		CAtlArray<CAtlString> SecurePorts;

		hr = CIIS7XMLConfigHelper::GetSiteInfo(pSite, item.sInstance, item.sDescription, item.sFileSystemPath, Ports, SecurePorts);
		if (hr != S_OK)
			continue;

		const int nPorts = (int) Ports.GetCount();
		for (int i = 0; i < nPorts; i++)
		{
			IISWebSiteBindings binding(Ports[i]);
			item.Ports.Add(binding);
		}

		const int nSecurePorts = (int) SecurePorts.GetCount();
		for (int i = 0; i < nSecurePorts; i++)
		{
			IISWebSiteBindings binding(SecurePorts[i]);
			item.Ports.Add(binding);
		}

		sites.push_back(item);
	}

	return TRUE;
}

void CIISConfigHelper::PopulateTreeFromXML(CTreeCtrl& cTree, HTREEITEM htParent, int nVirtualDirectoryIconIndex, IISWebSite* pInstanceItem)
{
	if (pInstanceItem == NULL)
		return;

	// get the config file
	CComPtr<IXMLDOMDocument> pDoc;
	if (CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDoc) != S_OK || pDoc == NULL)
		return;	

	CComPtr<IXMLDOMNode> pSite;
	HRESULT hr = CIIS7XMLConfigHelper::GetSiteNode(pDoc, pInstanceItem->sInstance, &pSite);
	if (hr != S_OK || pSite == NULL)
		return;		

	// get the 'applications' of the site
	CComPtr<IXMLDOMNodeList> pApplications;
	hr = pSite->selectNodes(CComBSTR(L"application"), &pApplications);
	if (hr != S_OK || pApplications == NULL)
		return;

	long nApplications = 0;
	hr = pApplications->get_length(&nApplications);

	// we are going to turn the XML into a more usable form, so we need map for this
	std::map<std::wstring, IISURIItem> uriitems;

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

			IISURIItem item;			

			// figure out the type of the item
			if (bsApplicationURI == L"/" && bsVirtualDirectoryURI == L"/")
			{
				item.Type = IISURIItem::WebServer;
			}
			else
			{
				item.Type = IISURIItem::VirtualRoot;
			}			

			// we need to figure out on the URI required to get to the item
			if (bsApplicationURI == L"/")
			{
				// we are at the root of the app/web site, so the URI is the virtual dir portion
				item.sURI = bsVirtualDirectoryURI;
			}
			else if (bsVirtualDirectoryURI == L"/")
			{				
				// we are at the root of the app/virtual dir, so the URI is the app dir portion
				item.sURI = bsApplicationURI;
			}
			else
			{
				// we need both app and virtual directory strings
				item.sURI = bsApplicationURI;
				item.sURI += bsVirtualDirectoryURI;
			}
			 
			CIIS7XMLConfigHelper::ExpandEnvironmentStrings(bsPhysicalPath, item.sFileSystemPath);

			// add the item to the list
			uriitems[std::wstring(item.sURI)] = item;
		}		
	}

	// as we add the items to the tree keep track of them since we need to establish the
	// parent/child relationship
	std::map<std::wstring, HTREEITEM> uritreemap;

	// add the root to the map since it's already in the tree
	uritreemap[L"/"] = htParent;

	// loop thru the URI items
	std::map<std::wstring, IISURIItem>::const_iterator iter = uriitems.begin();
	for (; iter != uriitems.end(); ++iter)
	{
		// ignore the web server node since it has already been added
		if (iter->first == L"/")
			continue;

		// look for the last '/' in the URI string
		size_t nURIPrefixLength = iter->first.rfind('/');
		if (nURIPrefixLength == std::wstring::npos)
			continue;

		// get the URI prefix
		std::wstring sURIPrefix(iter->first.c_str(), nURIPrefixLength == 0 ? 1 : nURIPrefixLength);

		// get the URI tail (the bit we are going use for the tree node text)
		LPCWSTR pszURITail = iter->first.c_str() + nURIPrefixLength + 1;		

		// find the parent item
		std::map<std::wstring, HTREEITEM>::const_iterator uriiter = uritreemap.find(sURIPrefix);
		if (uriiter == uritreemap.end())
			continue;		
		
		// add the URI item to the tree
		HTREEITEM htItem = cTree.InsertItem(pszURITail, nVirtualDirectoryIconIndex, nVirtualDirectoryIconIndex, uriiter->second);

		// we need an item data for this tree item
		IISURIItem* pItem = new IISURIItem;
		pItem->Type = IISURIItem::VirtualRoot;
		pItem->sMetaBasePath = iter->second.sMetaBasePath;
		pItem->sFileSystemPath = iter->second.sFileSystemPath;
		pItem->sURI = iter->second.sURI;

		cTree.SetItemData(htItem, (DWORD_PTR) pItem);	

		// add the new item to the map (so children can find it on subseqent passes)
		uritreemap[iter->first] = htItem;
	}
}

void CIISConfigHelper::RecurseTreeAndPopulateFromFileSystem(CTreeCtrl& cTree, HTREEITEM htParent, const std::set<CStringW>& IgnoreDirNames, int nMaxDepth)
{
	if (htParent == NULL)
		return;

	std::vector<HTREEITEM> Children;

	HTREEITEM htChild = cTree.GetChildItem(htParent);
	while (htChild != NULL)
	{		
		Children.push_back(htChild);

		htChild = cTree.GetNextSiblingItem(htChild);		
	}		

	CIISConfigHelper::PopulateTreeFromFileSystem(cTree, htParent, IgnoreDirNames, nMaxDepth);

	for (int i = 0; i < (int) Children.size(); i++)
	{
		RecurseTreeAndPopulateFromFileSystem(cTree, Children[i], IgnoreDirNames, nMaxDepth - 1);	
	}
}

void CIISConfigHelper::PopulateTreeFromFileSystem(CTreeCtrl& cTree, HTREEITEM htParent, const std::set<CStringW>& IgnoreDirNames, int nMaxDepth)
{
	if (htParent == NULL)
		return;	

	// get the item data from the tree item
	IISURIItem* pParentItem = (IISURIItem*) cTree.GetItemData(htParent);
	if (pParentItem == NULL)
	{
		ASSERT(pParentItem != NULL);
		return;
	}

	// if the item is incomplete then we need to get it's file system path
	if (pParentItem->Type == IISURIItem::IncompleteFileSystemPath && pParentItem->sFileSystemPath.GetLength() <= 0)
	{
		// get the grand parent item
		HTREEITEM htGrandParent = cTree.GetParentItem(htParent);
		if (htGrandParent == NULL)
		{
			ASSERT(htGrandParent != NULL);
			return;
		}		

		// get the grand parent item data
		IISURIItem* pGrandParentItem = (IISURIItem*) cTree.GetItemData(htGrandParent);
		if (pGrandParentItem == NULL)
		{
			ASSERT(pGrandParentItem != NULL);
			return;
		}

		// the grand parent MUST not be incomplete
		if (pGrandParentItem->Type == IISURIItem::IncompleteFileSystemPath)
		{
			ASSERT(pGrandParentItem->Type != IISURIItem::IncompleteFileSystemPath);
			return;
		}

		// get the item name
		CStringW sName = CStringW(cTree.GetItemText(htParent));
		if (sName.GetLength() <= 0)
		{
			ASSERT(sName.GetLength() > 0);
			return;
		}

		// make the path to the parent item
		CPathT<CStringW> ItemFileSystemPath(pGrandParentItem->sFileSystemPath);
		ItemFileSystemPath.Append(sName);

		// assign the new file system path and set the type
		pParentItem->sFileSystemPath = (LPCWSTR) ItemFileSystemPath;
		pParentItem->Type = IISURIItem::FileSystem;
	}

	// if the item already has children then we need to build up a list of their names
	std::set<CStringW> KnownDirs;
	if (cTree.ItemHasChildren(htParent) == TRUE)
	{
		// loop through all the children
		HTREEITEM htChild = cTree.GetChildItem(htParent);
		while (htChild != NULL)
		{
			// get the child name
			CStringW sName = CStringW(cTree.GetItemText(htChild));

			// we need lower case so we can compare easier
			sName.MakeLower();

			// add the name to the known list
			KnownDirs.insert(sName);

			// move on to the next child item
			htChild = cTree.GetNextSiblingItem(htChild);
		}
	}

#ifdef _DEBUG
	ATLTRACE2(L"CIISConfigHelper::PopulateTreeFromFileSystem() : searching '%s'\n", pParentItem->sFileSystemPath);
#endif

	// create a search string
	CPathT<CStringW> FileSystemPath(pParentItem->sFileSystemPath);
	FileSystemPath.Append(L"*.*");
	CStringW sSearchPath = (LPCWSTR) FileSystemPath;	

	// start the search
	WIN32_FIND_DATAW FindData;	
	HANDLE hFind = ::FindFirstFileW(sSearchPath, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// TODO: empty
		return;
	}

	CArray<CStringW, LPCWSTR> saDirs;	

	// guess that we will have 32 sub dirs (the array will grow if we have more)
	saDirs.SetSize(32);

	// keep track of the number of dirs we have actually found
	int nDirsFound = 0;

	do
	{		
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			continue;		

		// eliminate . and .. from the search
		if (FindData.cFileName[0] == '.')
		{
			if (FindData.cFileName[1] == '\0')
			{
				continue;
			}
			else if (FindData.cFileName[1] == '.' && FindData.cFileName[2] == '\0')
			{
				continue;
			}
		}		
			
		// convert to lower case
		CStringW sTempFileName = FindData.cFileName;
		sTempFileName.MakeLower();

		// only add to the collection if we don't already know about it
		if (KnownDirs.find(sTempFileName) != KnownDirs.end())
			continue;

		// if it's a known ignore dir name - then ignore it
		if (IgnoreDirNames.find(sTempFileName) != IgnoreDirNames.end())
			continue;
		
#ifdef _DEBUG
		ATLTRACE2(L"CIISConfigHelper::PopulateTreeFromFileSystem() : found '%s'\n", FindData.cFileName);
#endif

		saDirs.SetAtGrow(nDirsFound, FindData.cFileName);		

		// we have found one
		nDirsFound++;
	}
	while (::FindNextFileW(hFind, &FindData) == TRUE);	

	// get rid of the find handle
	::FindClose(hFind);
	hFind = NULL;	
	
	if (nDirsFound <= 0)
	{		
		return;
	}

	if (nMaxDepth <= 0)
	{
		cTree.InsertItem(EmptyNodeString, htParent);
		return;
	}

	for (int i = 0; i < nDirsFound; i++)
	{
		CString sDir(saDirs[i]);
		HTREEITEM htChild = cTree.InsertItem(sDir, htParent);		

		// create the child file system path
		CPathT<CStringW> FileSystemPath(pParentItem->sFileSystemPath);
		FileSystemPath.Append(saDirs[i]);

		// we need an item data for this tree item
		IISURIItem* pChildItem = new IISURIItem;
		pChildItem->Type = IISURIItem::FileSystem;
		pChildItem->sFileSystemPath = (LPCWSTR) FileSystemPath;

		// make the child URI
		CStringW sChildURI = pParentItem->sURI;
		int nChildURILength = sChildURI.GetLength();
		if (nChildURILength > 0 && sChildURI[nChildURILength - 1] != '/')
			sChildURI += '/';

		// append the dir name
		sChildURI += saDirs[i];

		// assign the URI to the item data
		pChildItem->sURI = sChildURI;

		// store the item data
		cTree.SetItemData(htChild, (DWORD_PTR) pChildItem);

		PopulateTreeFromFileSystem(cTree, htChild, IgnoreDirNames, nMaxDepth - 1);
	}
}

void CIISConfigHelper::PopulateTreeFromMetaBase(CTreeCtrl& cTree, HTREEITEM htParent, IISWebSite* pInstanceItem, const std::set<CStringW>& IgnoreDirNames, int nVirtualDirectoryIconIndex)
{
	if (htParent == NULL || pInstanceItem == NULL)
	{
		return;
	}

	CIISMetaBase MetaBase;
	CComPtr<IMSAdminBase> pAdminBase;

	if (MetaBase.Init() == true &&
		MetaBase.GetAdminBase(&pAdminBase) == true)
	{		
		CIISConfigHelper::PopulateTreeFromMetaBase(cTree, htParent, pInstanceItem->sMetaBasePath + _T("/ROOT"), L"/", pAdminBase, IgnoreDirNames, nVirtualDirectoryIconIndex);
	}
}

int CIISConfigHelper::PopulateTreeFromMetaBase(CTreeCtrl& cTree, HTREEITEM htParent, LPCWSTR pszMetaBasePath, LPCWSTR pszURI, IMSAdminBase* pAdminBase, const std::set<CStringW>& IgnoreDirNames, int nVirtualDirectoryIconIndex)
{
	if (htParent == NULL || pszMetaBasePath == NULL || pszURI == NULL || pAdminBase == NULL)
		return 0;	

	// we are going to enum the metabase...
	CIISMetaBase MetaBase(pAdminBase);
	
	// enum the given metabase path for child keys
	std::vector<std::wstring> saKeys;
	if (MetaBase.EnumKeys(CString(pszMetaBasePath), saKeys) == false || saKeys.size() == 0)
		return 0;

	// keep track of the number of child VRs we find
	int nTotalVRs = 0;

	// loop through all the child keys
	for (int i = 0; i < (int) saKeys.size(); i++)
	{
		// check to see whether we should ignore this dir
		if (IgnoreDirNames.find(saKeys[i].c_str()) != IgnoreDirNames.end())
			continue;

		CStringW sChildURI = pszURI;

		int nChildURILength = sChildURI.GetLength();
		if (nChildURILength > 0 && sChildURI[nChildURILength - 1] != '/')
			sChildURI += '/';

		sChildURI += saKeys[i].c_str();

		CStringW sChildMetaBasePath = pszMetaBasePath;
		sChildMetaBasePath += '/';
		sChildMetaBasePath += saKeys[i].c_str();

		// add the key to the tree (since children may need it)
		HTREEITEM htVR = cTree.InsertItem(CString(saKeys[i].c_str()), nVirtualDirectoryIconIndex, nVirtualDirectoryIconIndex, htParent);

		// drop down to enumerate the child key
		int nChildVRs = PopulateTreeFromMetaBase(cTree, htVR, sChildMetaBasePath, sChildURI, pAdminBase, IgnoreDirNames, nVirtualDirectoryIconIndex);

		// if the key doesn't have a VR itself or it didn't have any children with VRs then it's nothing
		// of interest. Delete the tree item and move to the next key.
		CIISMetaBaseData VRPath;		
		if (VRPath.ReadData(pAdminBase, sChildMetaBasePath, MD_VR_PATH) == false && nChildVRs <= 0)
		{
			cTree.DeleteItem(htVR);
			continue;		
		}		

		// we need an item data for this tree item
		IISURIItem* pItem = new IISURIItem;
		pItem->sMetaBasePath = sChildMetaBasePath;
		pItem->sURI = sChildURI;

		// get the VR_PATH (there may not be one)
		std::wstring sVRPath;
		if (VRPath.GetAsString(sVRPath) == true)
		{
			pItem->Type = IISURIItem::VirtualRoot;
			pItem->sFileSystemPath = sVRPath.c_str();
		}
		else
		{
			pItem->Type = IISURIItem::IncompleteFileSystemPath;
		}

		cTree.SetItemData(htVR, (DWORD_PTR) pItem);

		// keep track of the number of VRs in this branch
		nTotalVRs += nChildVRs;
		nTotalVRs++;
	}

	return nTotalVRs;
}

void CIISConfigHelper::RecurseTreeAndSortItems(CTreeCtrl& cTree, HTREEITEM htParent)
{
	if (htParent == NULL)
		return;

	cTree.SortChildren(htParent);

	HTREEITEM htChild = cTree.GetChildItem(htParent);
	while (htChild != NULL)
	{		
		RecurseTreeAndSortItems(cTree, htChild);

		htChild = cTree.GetNextSiblingItem(htChild);		
	}			
}