#include "StdAfx.h"
#include "IIS7XMLConfigHelper.h"

CIIS7XMLConfigHelper::CIIS7XMLConfigHelper(void)
{
}

CIIS7XMLConfigHelper::~CIIS7XMLConfigHelper(void)
{
}

HRESULT CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(IXMLDOMDocument** pDoc)
{
	// we need a DOM
	CComPtr<IXMLDOMDocument> pDOM;
	HRESULT hr = pDOM.CoCreateInstance(L"MSXML.DOMDocument");
	if (hr != S_OK)
		return hr;

	// get the path to the config file
	CAtlString sApplicationHostPath;
	GetApplicationHostConfigPath(sApplicationHostPath);

	// ask the XML DOM to load the config file
	VARIANT_BOOL bSuccess;
	hr = pDOM->load(CComVariant((const TCHAR*) sApplicationHostPath), &bSuccess);
	if (hr != S_OK)
		return hr;
	
	if (bSuccess != VARIANT_TRUE)
		return E_FAIL;

	// return the pointer
	return pDOM.CopyTo(pDoc);	
}

bool CIIS7XMLConfigHelper::GetApplicationHostConfigPath(CAtlString& sPath)
{
	// get the system directory
	TCHAR szSystemDir[512] = _T("");
	if (::GetSystemDirectory(szSystemDir, _countof(szSystemDir)) == 0)
		return false;

	// append the offset of the config file
	CAtlString sApplicationHostPath(szSystemDir);
	sApplicationHostPath += _T("\\inetsrv\\config\\applicationHost.config");

	// return the file path to the caller
	sPath = sApplicationHostPath;
	return true;
}

HRESULT CIIS7XMLConfigHelper::GetSitesCollection(IXMLDOMDocument* pDoc, IXMLDOMNodeList** ppSites)
{
	if (pDoc == NULL || ppSites == NULL)
		return E_POINTER;

	// get the site collection
	CComPtr<IXMLDOMNodeList> pSites;
	HRESULT hr = pDoc->selectNodes(CComBSTR(L"/configuration/system.applicationHost/sites/site"), &pSites);
	if (hr != S_OK)
		return hr;

	return pSites.CopyTo(ppSites);	
}

HRESULT CIIS7XMLConfigHelper::GetSiteNode(IXMLDOMDocument* pDoc, LPCWSTR pszId, IXMLDOMNode** ppSite)
{
	if (pDoc == NULL || pszId == NULL || ppSite == NULL)
		return E_POINTER;

	// we want the site corresponding to the instance string
	CAtlString sSitesQuery;
	sSitesQuery.Format(_T("/configuration/system.applicationHost/sites/site[@id='%ls']"), pszId);

	CComPtr<IXMLDOMNode> pSite;
	HRESULT hr = pDoc->selectSingleNode(CComBSTR(sSitesQuery), &pSite);
	if (hr != S_OK)
		return hr;
		
	if (pSite == NULL)
		return E_FAIL;

	return pSite.CopyTo(ppSite);
}

HRESULT CIIS7XMLConfigHelper::GetSiteInfo(IXMLDOMNode* pSite, CAtlString& sId, CAtlString& sDesc, CAtlString& sPhysicalPath, CAtlArray<CAtlString>& Ports, CAtlArray<CAtlString>& SecurePorts)
{
	if (pSite == NULL)
		return E_POINTER;	

	CComPtr<IXMLDOMNamedNodeMap> pSiteAttribs;
	HRESULT hr = pSite->get_attributes(&pSiteAttribs);
	if (hr != S_OK)		
		return hr;
	
	if (pSiteAttribs == NULL)
		return E_FAIL;

	// get the site name
	CComPtr<IXMLDOMNode> pNameNode;
	hr = pSiteAttribs->getNamedItem(CComBSTR(L"name"), &pNameNode);		
	if (hr != S_OK)
		return hr;

	CComBSTR bsName;
	hr = pNameNode->get_text(&bsName);
	if (hr != S_OK)
		return hr;

	// get the site id
	CComPtr<IXMLDOMNode> pIdNode;
	hr = pSiteAttribs->getNamedItem(CComBSTR(L"id"), &pIdNode);
	if (hr != S_OK)
		return hr;

	CComBSTR bsId;
	hr = pIdNode->get_text(&bsId);		
	if (hr != S_OK)
		return hr;

	// get the physical path of the site
	CComPtr<IXMLDOMNode> pVirtualRootNode;
	hr = pSite->selectSingleNode(CComBSTR(L"application[@path='/']/virtualDirectory[@path='/']"), &pVirtualRootNode);
	if (hr != S_OK)
		return hr;

	CComPtr<IXMLDOMNamedNodeMap> pVRAttribs;
	hr = pVirtualRootNode->get_attributes(&pVRAttribs);
	if (hr != S_OK)
		return hr;

	CComPtr<IXMLDOMNode> pPhysicalPathNode;
	hr = pVRAttribs->getNamedItem(CComBSTR(L"physicalPath"), &pPhysicalPathNode);
	if (hr != S_OK)
		return hr;

	CComBSTR bsFileSystemPath;
	hr = pPhysicalPathNode->get_text(&bsFileSystemPath);		
	if (hr != S_OK)
		return hr;	

	CComPtr<IXMLDOMNodeList> pHTTPBindingNodes;
	hr = pSite->selectNodes(CComBSTR(L"bindings/binding[@protocol='http']/@bindingInformation"), &pHTTPBindingNodes);
	if (hr == S_OK)
	{
		long nodes = 0;
		pHTTPBindingNodes->get_length(&nodes);

		for (long i = 0; i < nodes; i++)
		{
			CComPtr<IXMLDOMNode> pBindingNode;
			pHTTPBindingNodes->get_item(i, &pBindingNode);

			if (pBindingNode == NULL)
				continue;

			CComBSTR bsBinding;
			hr = pBindingNode->get_text(&bsBinding);
			if (hr == S_OK)
			{
				Ports.Add(bsBinding);
			}
		}
	}

	CComPtr<IXMLDOMNodeList> pHTTPSBindingNodes;
	hr = pSite->selectNodes(CComBSTR(L"bindings/binding[@protocol='https']/@bindingInformation"), &pHTTPSBindingNodes);
	if (hr == S_OK)
	{
		long nodes = 0;
		pHTTPSBindingNodes->get_length(&nodes);

		for (long i = 0; i < nodes; i++)
		{
			CComPtr<IXMLDOMNode> pBindingNode;
			pHTTPSBindingNodes->get_item(i, &pBindingNode);

			if (pBindingNode == NULL)
				continue;

			CComBSTR bsBinding;
			hr = pBindingNode->get_text(&bsBinding);
			if (hr == S_OK)
			{
				SecurePorts.Add(bsBinding);
			}
		}
	}

	sId = bsId;
	sDesc = bsName;
	ExpandEnvironmentStrings(bsFileSystemPath, sPhysicalPath);	

	return hr;
}

HRESULT CIIS7XMLConfigHelper::AddNativeModuleNode(IXMLDOMDocument* pDoc, 
												  IXMLDOMNode* pModulesNode, 
												  LPCSTR pszModuleName,
												  LPCSTR pszModulePath,
												  int bitness)
{
	if (pDoc == NULL || pModulesNode == NULL || pszModuleName == NULL)
		return E_POINTER;

	CAtlStringW sQuery;
	sQuery.Format(L"add[@name='%hs']", pszModuleName);	

	CComBSTR bsQuery(sQuery);

	// find the module node, if it doesn't exist then add it
	CComPtr<IXMLDOMNode> pModuleNode;
	HRESULT hr = pModulesNode->selectSingleNode(bsQuery, &pModuleNode);
	if (hr != S_OK || pModuleNode == NULL)
	{		
		hr = pDoc->createNode(CComVariant(NODE_ELEMENT), L"add", L"", &pModuleNode);
		if (hr != S_OK || pModuleNode == NULL)
			return hr;

		CComPtr<IXMLDOMNamedNodeMap> pAttrs;
		hr = pModuleNode->get_attributes(&pAttrs);
		if (hr != S_OK || pAttrs == NULL)
			return hr;

		CComPtr<IXMLDOMAttribute> pNameAttr;
		hr = pDoc->createAttribute(L"name", &pNameAttr);
		if (hr != S_OK || pNameAttr == NULL)
			return hr;

		hr = pNameAttr->put_text(CComBSTR(pszModuleName));
		hr = pAttrs->setNamedItem(pNameAttr, NULL);

		if (pszModulePath != NULL)
		{
			CComPtr<IXMLDOMAttribute> pImageAttr;
			hr = pDoc->createAttribute(L"image", &pImageAttr);
			if (hr != S_OK || pNameAttr == NULL)
				return hr;

			hr = pImageAttr->put_text(CComBSTR(pszModulePath));
			hr = pAttrs->setNamedItem(pImageAttr, NULL);
		}

		// set the bitness precondition
		if (bitness > 0)
		{
			CComPtr<IXMLDOMAttribute> pPreconditionAttr;
			hr = pDoc->createAttribute(L"preCondition", &pPreconditionAttr);
			if (hr != S_OK || pPreconditionAttr == NULL)
				return hr;
			
			CAtlString sBitness;
			sBitness.Format(_T("bitness%d"), bitness);

			hr = pPreconditionAttr->put_text(CComBSTR(sBitness));
			hr = pAttrs->setNamedItem(pPreconditionAttr, NULL);
		}

		hr = pModulesNode->appendChild(pModuleNode, NULL);
	}

	return hr;
}

HRESULT CIIS7XMLConfigHelper::RemoveModuleNodes(IXMLDOMDocument* pDoc, LPCSTR pszModuleName)
{
	if (pDoc == NULL || pszModuleName == NULL)
		return E_POINTER;

	CAtlStringW sQuery;
	sQuery.Format(L"//add[@name='%hs']", pszModuleName);	

	CComBSTR bsQuery(sQuery);

	CComPtr<IXMLDOMNodeList> pModuleNodes;
	HRESULT hr = pDoc->selectNodes(bsQuery, &pModuleNodes);

	if (hr == S_OK)
	{
		LONG nModules = 0;
		hr = pModuleNodes->get_length(&nModules);

		for (LONG i = 0; i < nModules; i++)
		{
			CComPtr<IXMLDOMNode> pModuleNode;
			hr = pModuleNodes->get_item(i, &pModuleNode);
			if (hr != S_OK || pModuleNode == NULL)
				continue;

			CComPtr<IXMLDOMNode> pParentNode;
			hr = pModuleNode->get_parentNode(&pParentNode);
			if (hr != S_OK || pParentNode == NULL)
				continue;

			CComPtr<IXMLDOMNode> pTemp;
			hr = pParentNode->removeChild(pModuleNode, &pTemp);
			if (hr != S_OK)
				break;
		}
	}

	return hr;
}

HRESULT CIIS7XMLConfigHelper::GetWebServerHostnameAndPort(IXMLDOMNode* pSite, CString& sHostname, DWORD& dwPort)
{
	if (pSite == NULL)
		return E_POINTER;

	CComPtr<IXMLDOMNode> pBindingAttr;
	HRESULT hr = pSite->selectSingleNode(
		CComBSTR(L"bindings/binding[@protocol='http']/@bindingInformation"), &pBindingAttr);

	if (hr != S_OK)
		return hr;
	
	if (pBindingAttr == NULL)
		return E_FAIL;

	CComBSTR bsBinding;
	hr = pBindingAttr->get_text(&bsBinding);
	if (hr != S_OK)
		return hr;

	CString sPort(bsBinding);

	if (sPort.GetLength() == 0 || sPort[0] != '*' || sPort[1] != ':')
		return E_FAIL;	

	LPCTSTR pszToken = sPort;

	// skip the leading '*:'
	pszToken += 2;

	// get the port
	dwPort = _ttoi(pszToken);

	// skip the number characters
	while (*pszToken != '\0' && _istdigit(*pszToken) != 0)
		pszToken++;

	// we must find the ':' here
	if (*pszToken != ':')
		return E_FAIL;

	// skip the ':'
	pszToken++;

	// get the hostname
	sHostname = pszToken;	

	return S_OK;
}