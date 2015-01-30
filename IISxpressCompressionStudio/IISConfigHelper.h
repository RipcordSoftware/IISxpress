#pragma once

#include <atlstr.h>
#include <atlcoll.h>

#include <set>
#include <map>
#include <vector>
#include <string>

#include "IISMetabase.h"

struct IISWebSiteBindings
{
	IISWebSiteBindings() : dwPort(0) {}

	IISWebSiteBindings(LPCTSTR pszBinding)
	{
		if (pszBinding != NULL)
		{
			if (*pszBinding == '*')
			{
				pszBinding++;
			}

			while (*pszBinding != '\0')
			{
				if (*pszBinding != ':')
				{					
					sIP += *pszBinding;										

					pszBinding++;
				}
				else
				{
					pszBinding++;
					break;
				}
			}

			CAtlStringW sPort;
			while (*pszBinding != '\0')
			{
				if (*pszBinding != ':')
				{
					sPort += *pszBinding;
					pszBinding++;
				}
				else
				{
					pszBinding++;
					break;
				}
			}
			dwPort = (DWORD) _ttoi64(sPort);

			while (*pszBinding != '\0')
			{
				sHostname += *pszBinding;
				pszBinding++;
			}
		}
	}

	CAtlStringW		sIP;
	DWORD			dwPort;
	CAtlStringW		sHostname;
};

struct IISWebSite
{
	// default constructor
	IISWebSite(void) : eSource(Unknown) {}

	// copy constructor
	IISWebSite(const IISWebSite& other)
	{
		eSource = other.eSource;
		sInstance = other.sInstance;
		sMetaBasePath = other.sMetaBasePath;
		sDescription = other.sDescription;
		sFileSystemPath = other.sFileSystemPath;
		Ports.Copy(other.Ports);
		SecurePorts.Copy(other.SecurePorts);
	}

	IISWebSite& operator =(const IISWebSite& other)
	{
		eSource = other.eSource;
		sInstance = other.sInstance;
		sMetaBasePath = other.sMetaBasePath;
		sDescription = other.sDescription;
		sFileSystemPath = other.sFileSystemPath;
		Ports.Copy(other.Ports);
		SecurePorts.Copy(other.SecurePorts);

		return *this;
	}

	// allow instance to keep track of the soure of the data
	enum Source
	{
		Unknown,
		Metabase,
		XML
	};

	Source							eSource;			// the source of the data
	CAtlStringW						sInstance;			// the instance string
	CAtlStringW						sDescription;		// the web site description (comment)
	CAtlStringW						sMetaBasePath;		// the metabase path (if the data came from the metabase, empty otherwise)
	CAtlStringW						sFileSystemPath;	// the physical file system path of the URI
	CAtlArray<IISWebSiteBindings>	Ports;
	CAtlArray<IISWebSiteBindings>	SecurePorts;
};

struct IISURIItem
{
	enum URITreeItemType
	{
		Unknown = 0,
		WebServer = 1,
		VirtualRoot = 2,
		FileSystem = 3,
		IncompleteFileSystemPath = 4
	};

	IISURIItem()
	{
		Type = IISURIItem::Unknown;
		bExcluded = FALSE;
		dwExclusionType = RULEFLAGS_FOLDER_NOMATCH;
	}

	IISURIItem(const IISURIItem& other)
	{
		Type = other.Type;
		sMetaBasePath = other.sMetaBasePath;
		sURI = other.sURI;
		sFileSystemPath = other.sFileSystemPath;
		bExcluded = other.bExcluded;
		dwExclusionType = other.dwExclusionType;
	}

	URITreeItemType	Type;
	CStringW		sMetaBasePath;
	CStringW		sURI;
	CStringW		sFileSystemPath;
	BOOL			bExcluded;
	DWORD			dwExclusionType;
};

class CIISConfigHelper
{
public:

	CIISConfigHelper(void);
	~CIISConfigHelper(void);

	static BOOL EnumWebSitesFromMetaBase(std::vector<IISWebSite>& sites);
	static BOOL EnumWebSitesFromXML(std::vector<IISWebSite>& sites);

	static void PopulateTreeFromMetaBase(CTreeCtrl& cTree, HTREEITEM htParent, IISWebSite* pInstanceItem, const std::set<CStringW>& IgnoreDirNames, int nVirtualDirectoryIconIndex);
	
	static void PopulateTreeFromXML(CTreeCtrl& cTree, HTREEITEM htParent, int nVirtualDirectoryIconIndex, IISWebSite* pInstanceItem);
	
	static void RecurseTreeAndPopulateFromFileSystem(CTreeCtrl& cTree, HTREEITEM htParent, const std::set<CStringW>& IgnoreDirNames, int nMaxDepth = 65535);	
	static void PopulateTreeFromFileSystem(CTreeCtrl& cTree, HTREEITEM htParent, const std::set<CStringW>& IgnoreDirNames, int nMaxDepth);

	static void RecurseTreeAndSortItems(CTreeCtrl& cTree, HTREEITEM htParent);	

	const static CString EmptyNodeString;
	const static CStringA EmptyNodeStringA;
	const static CStringW EmptyNodeStringW;

private:

	static int PopulateTreeFromMetaBase(CTreeCtrl& cTree, HTREEITEM htParent, LPCWSTR pszMetaBasePath, LPCWSTR pszURI, IMSAdminBase* pAdminBase, const std::set<CStringW>& IgnoreDirNames, int nVirtualDirectoryIconIndex);
};
