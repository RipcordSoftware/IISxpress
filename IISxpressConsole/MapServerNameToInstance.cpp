#include "stdafx.h"

#include "MapServerNameToInstance.h"

#include "IISMetaBase.h"

#include <vector>
using namespace std;

bool MapServerNameToInstance(const CString& sName, CString& sInstance)
{
	// connect to the metabase
	CIISMetaBase MetaBase;
	if (MetaBase.Init() == false)
	{
		// TODO: handle error
		return false;
	}

	// enumerate the web sites
	vector<wstring> Keys;
	if (MetaBase.EnumKeys(_T("/LM/W3SVC"), Keys) == false)
	{
		// TODO: handle error
		return false;
	}

	CComPtr<IMSAdminBase> pAdminBase;
	MetaBase.GetAdminBase(&pAdminBase);

	// loop round the candiate web sites
	for (int i = 0; i < (int) Keys.size(); i++)
	{
		// create the MB path to the candiate server
		CStringW sServerPath = L"/LM/W3SVC/";
		sServerPath += Keys[i].c_str();

		// ask for the key type
		CIISMetaBaseData KeyType;
		if (KeyType.ReadData(pAdminBase, sServerPath, MD_KEY_TYPE) == false)
			continue;

		// read the key type
		wstring sKeyType;
		if (KeyType.GetAsString(sKeyType) == false)
			continue;

		// we must have a web server
		if (sKeyType != IIS_CLASS_WEB_SERVER_W)
			continue;

		// get the server name (comment)
		CIISMetaBaseData ServerName;
		if (ServerName.ReadData(pAdminBase, sServerPath, MD_SERVER_COMMENT) == false)
			continue;

		// get the server name data
		wstring sServerName;
		if (ServerName.GetAsString(sServerName) == false)
			continue;

		// have we got a match?
		if (sName.CompareNoCase(sServerName.c_str()) == 0)
		{
			sInstance = Keys[i].c_str();
			return true;
		}
	}

	return false;
}

bool MapInstanceToServerName(const CString& sInstance, CString& sName)
{
	// connect to the metabase
	CIISMetaBase MetaBase;
	if (MetaBase.Init() == false)
	{
		// TODO: handle error
		return false;
	}

	CComPtr<IMSAdminBase> pAdminBase;
	MetaBase.GetAdminBase(&pAdminBase);

	// create the MB path to the candiate server
	CStringW sServerPath = L"/LM/W3SVC/";
	sServerPath += sInstance;

	// ask for the key type
	CIISMetaBaseData KeyType;
	if (KeyType.ReadData(pAdminBase, sServerPath, MD_KEY_TYPE) == false)
		return false;

	// read the key type
	wstring sKeyType;
	if (KeyType.GetAsString(sKeyType) == false)
		return false;

	// we must have a web server
	if (sKeyType != IIS_CLASS_WEB_SERVER_W)
		return false;

	// get the server name (comment)
	CIISMetaBaseData ServerName;
	if (ServerName.ReadData(pAdminBase, sServerPath, MD_SERVER_COMMENT) == false)
		return false;

	// get the server name data
	wstring sServerName;
	if (ServerName.GetAsString(sServerName) == false)
		return false;

	sName = sServerName.c_str();
	return true;
}

bool MapServerNameToInstance(IXMLDOMDocument* pDoc, const CString& sName, CString& sInstance)
{
	if (pDoc == NULL)
		return false;

	// get the site collection
	CComPtr<IXMLDOMNodeList> pSites;
	if (CIIS7XMLConfigHelper::GetSitesCollection(pDoc, &pSites) != S_OK)
		return false;

	// get the number of sites
	long nSites = 0;
	HRESULT hr = pSites->get_length(&nSites);

	// enum the sites looking for a match
	for (long i = 0; i < nSites; i++)
	{
		CComPtr<IXMLDOMNode> pSite;
		hr = pSites->get_item(i, &pSite);
		if (hr != S_OK)
			continue;

		CAtlString sId;
		CAtlString sDesc;
		CAtlString sPhysicalPath;
		CAtlArray<CAtlString> Ports;
		CAtlArray<CAtlString> SecurePorts;

		// get the site info
		hr = CIIS7XMLConfigHelper::GetSiteInfo(pSite, sId, sDesc, sPhysicalPath, Ports, SecurePorts);
		if (hr != S_OK)
			continue;

		// try to match the name to the description
		if (sName.CompareNoCase(sDesc) == 0)
		{
			// we found it
			sInstance = sId;
			return true;
		}
	}

	// we failed to match the name
	return false;
}