#pragma once

#include <atlstr.h>
#include <atlcoll.h>

class CIIS7XMLConfigHelper
{
public:
	CIIS7XMLConfigHelper(void);
	~CIIS7XMLConfigHelper(void);

	static bool GetApplicationHostConfigPath(CAtlString& sPath);
	static HRESULT GetApplicationHostConfigDocument(IXMLDOMDocument** pDoc);
	static HRESULT GetSitesCollection(IXMLDOMDocument* pDoc, IXMLDOMNodeList** ppSites);
	static HRESULT GetSiteNode(IXMLDOMDocument* pDoc, LPCWSTR pszId, IXMLDOMNode** ppSite);
	static HRESULT GetSiteInfo(IXMLDOMNode* pSite, CAtlString& sId, CAtlString& sDesc, CAtlString& sPhysicalPath, CAtlArray<CAtlString>& Ports, CAtlArray<CAtlString>& SecurePorts);

	static HRESULT AddNativeModuleNode(IXMLDOMDocument* pDoc, IXMLDOMNode* pModulesNode, LPCSTR pszModuleName, LPCSTR pszModulePath, int bitness);
	static HRESULT RemoveModuleNodes(IXMLDOMDocument* pDoc, LPCSTR pszModuleName);

	static HRESULT GetWebServerHostnameAndPort(IXMLDOMNode* pSite, CString& sHostname, DWORD& dwPort);

	template<typename T> static HRESULT ExpandEnvironmentStrings(LPCTSTR pszSrc, T& sDest);
};

template<typename T> HRESULT CIIS7XMLConfigHelper::ExpandEnvironmentStrings(LPCTSTR pszSrc, T& sDest)
{
	// expand the physical path environment strings
	TCHAR szFullFileSystemPath[2048];	
	::ExpandEnvironmentStrings(pszSrc, szFullFileSystemPath, _countof(szFullFileSystemPath));		

	sDest = szFullFileSystemPath;
	return S_OK;
}