#pragma once

#include "IISxpressServer.h"

#include "UserAgent.h"

#include <string>
#include <map>
#include <set>
using namespace std;

class INeverCompressRulesImpl : public INeverCompressRules
{

protected:

	INeverCompressRulesImpl(void);
	~INeverCompressRulesImpl(void);

	STDMETHOD(Init)(IStorage* pStorage);

	STDMETHOD(AddExtensionRule)(LPCSTR pszExtension, LPCSTR pszUserAgent, LPCSTR pszTypeString, DWORD dwUpdateCookie);
	STDMETHOD(AddContentTypeRule)(LPCSTR pszContentType, LPCSTR pszUserAgent, LPCSTR pszTypeString, DWORD dwUpdateCookie);
	STDMETHOD(AddFolderRule)(LPCSTR pszInstance, LPCSTR pszFolder, DWORD dwFlags, DWORD dwUpdateCookie);	
	STDMETHOD(AddIPRule)(LPCSTR pszIPAddress, DWORD dwUpdateCookie);

	STDMETHOD(RemoveExtensionRule)(LPCSTR pszExtension, LPCSTR pszUserAgent, DWORD dwUpdateCookie);
	STDMETHOD(RemoveContentTypeRule)(LPCSTR pszContentType, LPCSTR pszUserAgent, DWORD dwUpdateCookie);
	STDMETHOD(RemoveFolderRule)(LPCSTR pszInstance, LPCSTR pszLocation, DWORD dwUpdateCookie);
	STDMETHOD(RemoveIPRule)(LPCSTR pszIPAddress, DWORD dwUpdateCookie);

	STDMETHOD(MatchRule)(LPCSTR pszExtension, LPCSTR pszContentType, LPCSTR pszInstance, LPCSTR pszFolder, LPCSTR pszUserAgent, LPCSTR pszIPAddress);

	STDMETHOD(EnumExtensions)(IEnumExclusionRule** ppEnum);
	STDMETHOD(EnumContentTypes)(IEnumExclusionRule** ppEnum);
	STDMETHOD(EnumFolders)(IEnumExclusionRule** ppEnum);
	STDMETHOD(EnumIPAddresses)(IEnumExclusionRule** ppEnum);

protected:

	STDMETHOD(Fire_OnRuleChanged)(NeverRuleChangeHint hint, DWORD dwChangeSource);

private:

	HRESULT Load(void);
	HRESULT Save(void);

	HRESULT LoadNeverExtensions(void);
	HRESULT LoadNeverContentTypes(void);
	HRESULT LoadNeverFolders(void);
	HRESULT LoadNeverIPAddresses(void);

	HRESULT MatchFolderRule(LPCSTR pszInstance, LPCSTR pszFolder);

	HRESULT GetUserAgentProducts(IXMLDOMNode* pNode, UserAgentProducts& Products);
	HRESULT GetProductFromNode(IXMLDOMNode* pProductNode, UserAgentProduct& Product);	

	CComPtr<IStorage>			m_pConfigStorage;	

	CComAutoCriticalSection		m_csDOM;
	CComPtr<IXMLDOMDocument>	m_pDOM;	

	CComAutoCriticalSection						m_csConfig;
	map<string, RuleUserAgents>					m_NeverExtensions;	
	map<string, RuleUserAgents>					m_NeverContentTypes;
	map<string, RuleUserAgents>					m_NeverContentTypeClass;
	map<string, map<string, DWORD> >			m_NeverFolders;
	set<string>									m_NeverIPAddresses;
};
