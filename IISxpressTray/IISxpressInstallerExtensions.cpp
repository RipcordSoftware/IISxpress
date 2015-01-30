// IISxpressInstallerExtensions.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "IISxpressInstallerExtensions.h"

#include <objbase.h>
#include <atlstr.h>
#include <atlpath.h>

#include <Imagehlp.h>

#include "IISMetabase.h"
#include "ServiceNames.h"

#include "IIS7XMLConfigHelper.h"
#include "IStreamHelper.h"

#include "resource.h"

// ********************************************************************************************

static HANDLE AttemptExclusiveFileOpen(LPCTSTR pszFilename, DWORD dwDelay = 1000);
static HANDLE AttemptExclusiveFileOpenW(LPCWSTR pszFilename, DWORD dwDelay = 1000);

static bool AddGlobalIISxpressNativeModule(IXMLDOMDocument* pDoc, LPCSTR pszModulePath, int bitness);
static bool AddLocalIISxpressNativeModule(IXMLDOMDocument* pDoc, int bitness);
static bool AddIISxpressNativeModule(IXMLDOMDocument* pDoc, LPCWSTR pszModuleNodeName, LPCSTR pszModulePath, int bitness);

static bool AddIISxpressNativeModuleToRootLocation(IXMLDOMDocument* pDoc, int bitness);

static bool RemoveModuleFromRootLocation(IXMLDOMDocument* pDoc, LPCWSTR pszModuleName);

static int GetImageBitness(LPCSTR pszImagePath);

// ********************************************************************************************

#define IISXPRESS_FILTERNAMEW			L"IISxpress"

#define IISXPRESS_MODULE32NAMEW			L"IISxpressNativeModule32"
#define IISXPRESS_MODULE32NAMEA			"IISxpressNativeModule32"
#define IISXPRESS_MODULE64NAMEW			L"IISxpressNativeModule64"
#define IISXPRESS_MODULE64NAMEA			"IISxpressNativeModule64"

#define IIS7_STATICCOMPRESSION_MODULENAMEA	"StaticCompressionModule"
#define IIS7_DYNAMICCOMPRESSION_MODULENAMEA	"DynamicCompressionModule"
#define IIS7_STATICCOMPRESSION_MODULENAMEW	L"StaticCompressionModule"
#define IIS7_DYNAMICCOMPRESSION_MODULENAMEW	L"DynamicCompressionModule"

LONG Internal_InstallIISxpressFilter_IIS5_6(HWND hwnd, LONG IValue, LPCSTR lpszValue);
LONG Internal_InstallIISxpressFilter_IIS7(HWND hwnd, LONG IValue, LPCSTR lpszValue);

LONG Internal_RemoveIISxpressFilter_IIS5_6(HWND hwnd, LONG IValue);
LONG Internal_RemoveIISxpressFilter_IIS7(HWND hwnd, LONG IValue);

// Add the IISxpress filter to the IIS configuration
// 
// incoming:
//  lpIValue             : set to 0 to ignore service manager logic
//                         set to 1 to stop IIS
//                         set to 2 to stop then restart IIS if it was already running
//  lpszValue            : the full path to the filter DLL
//
// returns:
//  -1 : fatal
//   0 : failed but can continue
//   1 : succeeded
LONG Internal_InstallIISxpressFilter(HWND hwnd, LONG IValue, LPCSTR lpszValue)
{
	OSVERSIONINFO verinfo;
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	::GetVersionEx(&verinfo);
	if (verinfo.dwMajorVersion < 6)
	{
		return Internal_InstallIISxpressFilter_IIS5_6(hwnd, IValue, lpszValue);
	}
	else
	{
		// determine the bitness of the module we are being asked to load
		int bitness = GetImageBitness(lpszValue);
		if (bitness > 0)
		{
			return Internal_InstallIISxpressFilter_IIS7(hwnd, IValue, lpszValue, bitness);
		}
		else
		{
			return -1;
		}
	}
}

LONG Internal_InstallIISxpressFilter_IIS5_6(HWND hwnd, LONG IValue, LPCSTR lpszValue)
{
#ifndef QUIET_MODE
	// load the error box caption
	CAtlString sCaption;
	sCaption.LoadString(IDS_ERROR_CAPTION);
#endif

	SC_HANDLE hSM = NULL;
	SC_HANDLE hIIS = NULL;
	bool bIISWasRunnning = false;

	if (IValue > 0)
	{
		// attempt to connect to the service manager
		hSM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT | STANDARD_RIGHTS_READ);
		if (hSM == NULL)
		{		
#ifndef QUIET_MODE
			CAtlString sMsg;		
			sMsg.LoadString(IDS_ERROR_CONNECTSERVICEMANGER);
			if (::MessageBox(hwnd, sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) == IDNO)
				return -1;
			else
				return 0;
#else
			return -1;
#endif
		}

		// attempt to open the IIS service so we can determine that it is definitely installed
		hIIS = ::OpenService(hSM, IIS_SERVICENAME, GENERIC_READ | GENERIC_EXECUTE);
		if (hIIS == NULL)
		{
			::CloseServiceHandle(hSM);

#ifndef QUIET_MODE
			CAtlString sMsg;		
			sMsg.LoadString(IDS_ERROR_OPENIISSERVICE);
			if (::MessageBox(hwnd, sMsg, sCaption, MB_YESNO | MB_ICONQUESTION) == IDNO)
				return -1;
			else
				return 0;
#else
			return -1;
#endif
		}

		// stop the IIS service noting whether it was already running (so we can restart it)
		SERVICE_STATUS ServiceStatus;

		if (IValue == 2)
		{
			if (::QueryServiceStatus(hIIS, &ServiceStatus) == TRUE)
			{
				if (ServiceStatus.dwCurrentState != SERVICE_STOPPED)
				{
					bIISWasRunnning = true;				
				}
			}
		}

		// wait until it stops (up to 30s)
		if (::ControlService(hIIS, SERVICE_CONTROL_STOP, &ServiceStatus) == TRUE)
		{
			for (int nLoop = 0; nLoop < 30; nLoop++)
			{
				::Sleep(1000);

				if (::QueryServiceStatus(hIIS, &ServiceStatus) == TRUE &&
					ServiceStatus.dwCurrentState == SERVICE_STOPPED)
				{
					break;
				}
			}			
		}
	}

	LONG nStatus = 0;

	CIISMetaBase MetaBase;
	CComPtr<IMSAdminBase> pAdminBase;
	if (MetaBase.Init() == true && MetaBase.GetAdminBase(&pAdminBase) == true)
	{			
		CIISMetaBaseData LoadOrderData;
		if (LoadOrderData.ReadData(pAdminBase, L"/LM/W3SVC/Filters", MD_FILTER_LOAD_ORDER) == true)
		{
			std::wstring sLoadOrder;
			LoadOrderData.GetAsString(sLoadOrder);

			// remove the IISxpress entry(s) from the filter load order if already there						
			while (1)
			{
				std::wstring::size_type nPos = sLoadOrder.find(IISXPRESS_FILTERNAMEW);
				if (nPos == std::wstring::npos)
					break;
				
				sLoadOrder.replace(nPos, wcslen(IISXPRESS_FILTERNAMEW), L"");										
			}			

			// replace any double commas with singles (probably as a result of above)
			while (1)
			{
				std::wstring::size_type nPos = sLoadOrder.find(L",,");
				if (nPos == std::wstring::npos)
					break;
				
				sLoadOrder.replace(nPos, 2, L",");				
			}

			// remove leading ',' if there is one
			if (sLoadOrder.size() > 0 && sLoadOrder[0] == ',')
			{
				sLoadOrder = sLoadOrder.substr(1);
			}

			// remove trailing ',' if there is one
			if (sLoadOrder.size() > 0 && sLoadOrder[sLoadOrder.size() - 1] == ',')
			{
				sLoadOrder.resize(sLoadOrder.size() - 1);
			}

			// open the Filters metabase key for writing
			METADATA_HANDLE hFilters = NULL;
			if (pAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE, L"/LM/W3SVC/Filters", METADATA_PERMISSION_WRITE, METABASE_TIMEOUT, &hFilters) == S_OK)
			{
				// delete the key if it's already there since it's probably crap
				pAdminBase->DeleteKey(hFilters, IISXPRESS_FILTERNAMEW);

				// add the key
				pAdminBase->AddKey(hFilters, IISXPRESS_FILTERNAMEW);

				// don't need this anymore
				pAdminBase->CloseKey(hFilters);
				hFilters = NULL;
				
				METADATA_HANDLE hIISxpress = NULL;
				if (pAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE, L"/LM/W3SVC/Filters/" IISXPRESS_FILTERNAMEW, METADATA_PERMISSION_WRITE, METABASE_TIMEOUT, &hIISxpress) == S_OK)
				{
					METADATA_RECORD Record;
					Record.dwMDIdentifier = MD_FILTER_IMAGE_PATH;
					Record.dwMDAttributes = METADATA_NO_ATTRIBUTES;
					Record.dwMDUserType = IIS_MD_UT_SERVER;
					Record.dwMDDataType = STRING_METADATA;
					Record.dwMDDataLen = sizeof(WCHAR);
					Record.pbMDData = (BYTE*) L"";

					HRESULT hrTemp = pAdminBase->SetData(hIISxpress, L"", &Record);

					// add the MD_KEY_TYPE entry for this key
					WCHAR* pszFilterTypeString = L"IIsFilter";

					Record.dwMDIdentifier = MD_KEY_TYPE;
					Record.dwMDAttributes = METADATA_NO_ATTRIBUTES;
					Record.dwMDUserType = IIS_MD_UT_SERVER;
					Record.dwMDDataType = STRING_METADATA;
					Record.dwMDDataLen = (DWORD) (wcslen(pszFilterTypeString) + 1) * sizeof(WCHAR);
					Record.pbMDData = (BYTE*) pszFilterTypeString;

					hrTemp = pAdminBase->SetData(hIISxpress, L"", &Record);

					pAdminBase->CloseKey(hIISxpress);
					hIISxpress = NULL;
				}				
			}

			// now commit the data
			CIISMetaBaseData ImagePathData;
			if (ImagePathData.ReadData(pAdminBase, L"/LM/W3SVC/Filters/" IISXPRESS_FILTERNAMEW, MD_FILTER_IMAGE_PATH) == true)
			{					
				ImagePathData.SetAsString(std::wstring(CComBSTR(lpszValue)));
				ImagePathData.WriteData(pAdminBase);

				// only prefix with comma when there area already filters in the list
				if (sLoadOrder.size() > 0)
					sLoadOrder += L",";
				
				sLoadOrder += IISXPRESS_FILTERNAMEW;

				LoadOrderData.SetAsString(sLoadOrder);				
				LoadOrderData.WriteData(pAdminBase);

				nStatus = 1;
			}			
		}	
	
		// switch off static compression
		CIISMetaBaseData DoStaticCompression;
		DoStaticCompression.SetKeyPath(L"/LM/W3SVC/Filters/Compression/Parameters");
		DoStaticCompression.SetIdentifier(MD_HC_DO_STATIC_COMPRESSION);
		DoStaticCompression.SetDataType(DWORD_METADATA);
		DoStaticCompression.SetUserType(IIS_MD_UT_SERVER);
		DoStaticCompression.SetAsDWORD(0);
		DoStaticCompression.WriteData(pAdminBase);			

		// switch off dynamic compression
		CIISMetaBaseData DoDynamicCompression;
		DoDynamicCompression.SetKeyPath(L"/LM/W3SVC/Filters/Compression/Parameters");
		DoDynamicCompression.SetIdentifier(MD_HC_DO_DYNAMIC_COMPRESSION);
		DoDynamicCompression.SetDataType(DWORD_METADATA);
		DoDynamicCompression.SetUserType(IIS_MD_UT_SERVER);
		DoDynamicCompression.SetAsDWORD(0);
		DoDynamicCompression.WriteData(pAdminBase);			
	}

	// restart IIS if it was already running
	if (hIIS != NULL && bIISWasRunnning == true)
	{
		if (::StartService(hIIS, 0, NULL) == TRUE)
		{
			for (int nLoop = 0; nLoop < 30; nLoop++)
			{
				::Sleep(1000);

				SERVICE_STATUS ServiceStatus;
				if (::QueryServiceStatus(hIIS, &ServiceStatus) == TRUE &&
					ServiceStatus.dwCurrentState == SERVICE_RUNNING)
				{
					break;
				}
			}
		}
	}

	if (hIIS != NULL)
	{
		::CloseServiceHandle(hIIS);
	}

	if (hSM != NULL)
	{
		::CloseServiceHandle(hSM);
	}

	return nStatus;
}

LONG Internal_InstallIISxpressFilter_IIS7(HWND hwnd, LONG IValue, LPCSTR lpszValue, int bitness)
{
	if (lpszValue == NULL)
	{
		return -1;
	}

	// if the caller didn't specify the bitness of the module then we need to determine it now
	if (bitness <= 0)
	{
		bitness = GetImageBitness(lpszValue);
	}

	// if we can't determine the image bitness then we can't continue
	if (bitness <= 0)
	{
		return -1;
	}

	// get the path to the config file
	CAtlString sConfigPath;
	if (CIIS7XMLConfigHelper::GetApplicationHostConfigPath(sConfigPath) == false)
		return -1;

	// get an exclusive lock on the config file
	Ripcord::Types::AutoHandle::AutoFileHandle hFile(AttemptExclusiveFileOpen(sConfigPath));
	if (!hFile == true)
	{		
		return -1;
	}

	// read the XML file data into a stream
	CComPtr<IStream> pStream;
	if (CIStreamHelper::GetFileDataAsStream(hFile, &pStream) != S_OK)
	{
		return -1;
	}	

	// create an XML DOM
	CComPtr<IXMLDOMDocument> pDoc;
	HRESULT hr = pDoc.CoCreateInstance(L"MSXML.DOMDocument");
	if (hr != S_OK)
	{
		return -1;
	}

	// load the XML
	VARIANT_BOOL vbSuccess;
	hr = pDoc->load(CComVariant(pStream), &vbSuccess);
	if (hr != S_OK || vbSuccess != -1)
	{
		return -1;
	}

	// add the IISxressNativeModule to the global module list if it's not already there
	AddGlobalIISxpressNativeModule(pDoc, lpszValue, bitness);	

	// add the module to the location section
	AddIISxpressNativeModuleToRootLocation(pDoc, bitness);

	// remove the built in compression modules
	RemoveModuleFromRootLocation(pDoc, IIS7_DYNAMICCOMPRESSION_MODULENAMEW);
	RemoveModuleFromRootLocation(pDoc, IIS7_STATICCOMPRESSION_MODULENAMEW);

	// commit the XML document to the stream object
	DWORD dwStreamLength = 0;
	CIStreamHelper::CommitXMLToStream(pDoc, pStream, dwStreamLength);

	// flush the stream to disk
	CIStreamHelper::CommitStreamToFile(pStream, dwStreamLength, hFile);

	CComPtr<IXMLDOMNodeList> pSiteNodes;
	hr = CIIS7XMLConfigHelper::GetSitesCollection(pDoc, &pSiteNodes);
	if (hr == S_OK && pSiteNodes != NULL)
	{
		LONG nSites = 0;
		hr = pSiteNodes->get_length(&nSites);

		for (LONG i = 0; i < nSites; i++)
		{
			CComPtr<IXMLDOMNode> pSiteNode;
			hr = pSiteNodes->get_item(i, &pSiteNode);
			if (hr != S_OK || pSiteNode == NULL)
				continue;

			CAtlStringW sId;
			CAtlStringW sDesc;
			CAtlStringW sPhysicalPath;
			CAtlArray<CAtlString> Ports;
			CAtlArray<CAtlString> SecurePorts;
			CIIS7XMLConfigHelper::GetSiteInfo(pSiteNode, sId, sDesc, sPhysicalPath, Ports, SecurePorts);			

			CPathW ConfigPath(sPhysicalPath);
			ConfigPath.Append(L"web.config");

			// get an exclusive lock on the config file
			Ripcord::Types::AutoHandle::AutoFileHandle hFile(AttemptExclusiveFileOpenW(ConfigPath));
			if (!hFile == true)
			{				
				continue;
			}

			// read the XML file data into a stream
			CComPtr<IStream> pStream;
			if (CIStreamHelper::GetFileDataAsStream(hFile, &pStream) != S_OK)
			{
				continue;
			}	

			// create an XML DOM
			CComPtr<IXMLDOMDocument> pDoc;
			HRESULT hr = pDoc.CoCreateInstance(L"MSXML.DOMDocument");
			if (hr != S_OK)
			{
				continue;
			}

			// load the XML
			VARIANT_BOOL vbSuccess;
			hr = pDoc->load(CComVariant(pStream), &vbSuccess);
			if (hr != S_OK || vbSuccess != -1)
			{
				continue;
			}

			AddLocalIISxpressNativeModule(pDoc, bitness);

			// remove the static and dynamic modules
			CIIS7XMLConfigHelper::RemoveModuleNodes(pDoc, IIS7_STATICCOMPRESSION_MODULENAMEA);
			CIIS7XMLConfigHelper::RemoveModuleNodes(pDoc, IIS7_DYNAMICCOMPRESSION_MODULENAMEA);
	
			// commit the XML document to the stream object
			DWORD dwStreamLength = 0;
			CIStreamHelper::CommitXMLToStream(pDoc, pStream, dwStreamLength);

			// flush the stream to disk
			CIStreamHelper::CommitStreamToFile(pStream, dwStreamLength, hFile);
		}
	}

	return 1;
}

bool AddGlobalIISxpressNativeModule(IXMLDOMDocument* pDoc, LPCSTR pszModulePath, int bitness)
{
	return AddIISxpressNativeModule(pDoc, L"globalModules", pszModulePath, bitness);
}

bool AddLocalIISxpressNativeModule(IXMLDOMDocument* pDoc, int bitness)
{
	return AddIISxpressNativeModule(pDoc, L"modules", NULL, bitness);
}

bool AddIISxpressNativeModule(IXMLDOMDocument* pDoc, LPCWSTR pszModuleNodeName, LPCSTR pszModulePath, int bitness)
{
	if (pDoc == NULL || pszModuleNodeName == NULL || bitness == 0)
		return false;

	CComBSTR bsModuleQueryPath(L"/configuration/system.webServer/");
	bsModuleQueryPath += pszModuleNodeName;

	// find the modules node
	CComPtr<IXMLDOMNode> pModulesNode;
	HRESULT hr = pDoc->selectSingleNode(bsModuleQueryPath, &pModulesNode);
	if (hr != S_OK || pModulesNode == NULL)
	{
		return false;
	}

	const char* pszModuleName = IISXPRESS_MODULE32NAMEA;
	if (bitness == 64)
	{
		pszModuleName = IISXPRESS_MODULE64NAMEA;
	}
	
	CIIS7XMLConfigHelper::AddNativeModuleNode(pDoc, pModulesNode, pszModuleName, pszModulePath, bitness);

	return true;
}

bool AddIISxpressNativeModuleToRootLocation(IXMLDOMDocument* pDoc, int bitness)
{
	if (pDoc == NULL || bitness == 0)
		return false;

	CComPtr<IXMLDOMNode> pLocationModulesNode;
	HRESULT hr = pDoc->selectSingleNode(L"/configuration/location[@path='']/system.webServer/modules", &pLocationModulesNode);
	if (hr != S_OK || pLocationModulesNode == NULL)
		return false;

	// determine the name of the module being installed
	const char* pszModuleName = IISXPRESS_MODULE32NAMEA;
	if (bitness == 64)
	{
		pszModuleName = IISXPRESS_MODULE64NAMEA;
	}
	
	CIIS7XMLConfigHelper::AddNativeModuleNode(pDoc, pLocationModulesNode, pszModuleName, NULL, bitness);

	return true;
}

bool RemoveModuleFromRootLocation(IXMLDOMDocument* pDoc, LPCWSTR pszModuleName)
{
	if (pDoc == NULL)
		return false;

	CComPtr<IXMLDOMNode> pLocationModulesNode;
	HRESULT hr = pDoc->selectSingleNode(L"/configuration/location[@path='']/system.webServer/modules", &pLocationModulesNode);
	if (hr != S_OK || pLocationModulesNode == NULL)
		return false;

	CAtlStringW sQuery;
	sQuery.Format(L"add[@name='%s']", pszModuleName);
	
	CComPtr<IXMLDOMNode> pModuleNode;
	hr = pLocationModulesNode->selectSingleNode(CComBSTR(sQuery), &pModuleNode);
	if (hr != S_OK || pModuleNode == NULL)
		return false;

	CComPtr<IXMLDOMNode> pTemp;
	hr = pLocationModulesNode->removeChild(pModuleNode, &pTemp);

	return hr == S_OK ? true : false;
}

HANDLE AttemptExclusiveFileOpen(LPCTSTR pszFilename, DWORD dwDelay)
{
	// get an exclusive lock on the config file
	HANDLE hFile = ::CreateFile(pszFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		::Sleep(dwDelay);

		hFile = ::CreateFile(pszFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);		
	}

	return hFile;
}

HANDLE AttemptExclusiveFileOpenW(LPCWSTR pszFilename, DWORD dwDelay)
{
	// get an exclusive lock on the config file
	HANDLE hFile = ::CreateFileW(pszFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		::Sleep(dwDelay);

		hFile = ::CreateFileW(pszFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);		
	}

	return hFile;
}

// Remove the IISxpress filter from the IIS configuration
//
// incoming:
//  lpIValue             : set to 0 to ignore service manager logic
//                         set to 1 to stop IIS
//                         set to 2 to stop then restart IIS if it was already running
//  lpszValue            : not used
//
// returns:
//  -1 : fatal
//   0 : failed but can continue
//   1 : succeeded
LONG Internal_RemoveIISxpressFilter(HWND hwnd, LONG IValue)
{
	OSVERSIONINFO verinfo;
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	::GetVersionEx(&verinfo);
	if (verinfo.dwMajorVersion < 6)
	{
		return Internal_RemoveIISxpressFilter_IIS5_6(hwnd, IValue);
	}
	else
	{
		return Internal_RemoveIISxpressFilter_IIS7(hwnd, IValue);
	}
}

LONG Internal_RemoveIISxpressFilter_IIS5_6(HWND hwnd, LONG IValue)
{
	SC_HANDLE hSM = NULL;
	SC_HANDLE hIIS = NULL;
	bool bIISWasRunnning = false;

	if (IValue > 0)
	{
		// attempt to connect to the service manager
		hSM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT | STANDARD_RIGHTS_READ);
		if (hSM == NULL)
		{					
			return 0;
		}

		// attempt to open the IIS service so we can determine that it is definitely installed
		hIIS = ::OpenService(hSM, IIS_SERVICENAME, GENERIC_READ | GENERIC_EXECUTE);
		if (hIIS == NULL)
		{
			::CloseServiceHandle(hSM);
			return 0;
		}

		// stop the IIS service noting whether it was already running (so we can restart it)
		SERVICE_STATUS ServiceStatus;

		if (IValue == 2)
		{
			if (::QueryServiceStatus(hIIS, &ServiceStatus) == TRUE)
			{
				if (ServiceStatus.dwCurrentState != SERVICE_STOPPED)
				{
					bIISWasRunnning = true;				
				}
			}
		}

		::ControlService(hIIS, SERVICE_CONTROL_STOP, &ServiceStatus);
	}

	int nStatus = 0;

	CIISMetaBase MetaBase;
	if (MetaBase.Init() == true)
	{
		CComPtr<IMSAdminBase> pAdminBase;
		MetaBase.GetAdminBase(&pAdminBase);

		CIISMetaBaseData LoadOrderData;
		if (pAdminBase != NULL &&
			LoadOrderData.ReadData(pAdminBase, L"/LM/W3SVC/Filters", MD_FILTER_LOAD_ORDER) == true)
		{
			std::wstring sLoadOrder;
			LoadOrderData.GetAsString(sLoadOrder);

			// remove the IISxpress entry(s) from the filter load order if already there						
			while (1)
			{
				std::wstring::size_type nPos = sLoadOrder.find(IISXPRESS_FILTERNAMEW);
				if (nPos == std::wstring::npos)
					break;
				
				sLoadOrder.replace(nPos, wcslen(IISXPRESS_FILTERNAMEW), L"");										
			}			

			// replace any double commas with singles (probably as a result of above)
			while (1)
			{
				std::wstring::size_type nPos = sLoadOrder.find(L",,");
				if (nPos == std::wstring::npos)
					break;
				
				sLoadOrder.replace(nPos, 2, L",");				
			}

			// remove leading ',' if there is one
			if (sLoadOrder.size() > 0 && sLoadOrder[0] == ',')
			{
				sLoadOrder = sLoadOrder.substr(1);
			}

			// remove trailing ',' if there is one
			if (sLoadOrder.size() > 0 && sLoadOrder[sLoadOrder.size() - 1] == ',')
			{
				sLoadOrder.resize(sLoadOrder.size() - 1);
			}

			// open the Filters metabase key for writing
			METADATA_HANDLE hFilters = NULL;
			if (pAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE, L"/LM/W3SVC/Filters", METADATA_PERMISSION_WRITE, METABASE_TIMEOUT, &hFilters) == S_OK)
			{
				// delete the key if it's already there since it's probably crap
				pAdminBase->DeleteKey(hFilters, IISXPRESS_FILTERNAMEW);

				// don't need this anymore
				pAdminBase->CloseKey(hFilters);
				hFilters = NULL;					

				LoadOrderData.SetAsString(sLoadOrder);				
				LoadOrderData.WriteData(pAdminBase);

				nStatus = 1;
			}			
		}
	}

	// restart IIS if it was already running
	if (hIIS != NULL && bIISWasRunnning == true)
	{
		::StartService(hIIS, 0, NULL);
	}

	if (hIIS != NULL)
	{
		::CloseServiceHandle(hIIS);
	}

	if (hSM != NULL)
	{
		::CloseServiceHandle(hSM);
	}

	return nStatus;
}

LONG Internal_RemoveIISxpressFilter_IIS7(HWND hwnd, LONG IValue)
{
	// get the path to the config file
	CAtlString sConfigPath;
	if (CIIS7XMLConfigHelper::GetApplicationHostConfigPath(sConfigPath) == false)
		return -1;

	// get an exclusive lock on the config file
	Ripcord::Types::AutoHandle::AutoFileHandle hFile(AttemptExclusiveFileOpen(sConfigPath));
	if (!hFile == true)
	{		
		return -1;
	}

	// read the XML file data into a stream
	CComPtr<IStream> pStream;
	if (CIStreamHelper::GetFileDataAsStream(hFile, &pStream) != S_OK)
	{
		return -1;
	}	

	// create an XML DOM
	CComPtr<IXMLDOMDocument> pDoc;
	HRESULT hr = pDoc.CoCreateInstance(L"MSXML.DOMDocument");
	if (hr != S_OK)
	{
		return -1;
	}

	// load the XML
	VARIANT_BOOL vbSuccess;
	hr = pDoc->load(CComVariant(pStream), &vbSuccess);
	if (hr != S_OK || vbSuccess != -1)
	{
		return -1;
	}

	// do the site 'web.config' files first
	CComPtr<IXMLDOMNodeList> pSiteNodes;
	hr = CIIS7XMLConfigHelper::GetSitesCollection(pDoc, &pSiteNodes);
	if (hr == S_OK && pSiteNodes != NULL)
	{
		LONG nSites = 0;
		hr = pSiteNodes->get_length(&nSites);

		for (LONG i = 0; i < nSites; i++)
		{
			CComPtr<IXMLDOMNode> pSiteNode;
			hr = pSiteNodes->get_item(i, &pSiteNode);
			if (hr != S_OK || pSiteNode == NULL)
				continue;

			CAtlStringW sId;
			CAtlStringW sDesc;
			CAtlStringW sPhysicalPath;
			CAtlArray<CAtlString> Ports;
			CAtlArray<CAtlString> SecurePorts;
			CIIS7XMLConfigHelper::GetSiteInfo(pSiteNode, sId, sDesc, sPhysicalPath, Ports, SecurePorts);			

			CPathW ConfigPath(sPhysicalPath);
			ConfigPath.Append(L"web.config");

			// get an exclusive lock on the config file
			Ripcord::Types::AutoHandle::AutoFileHandle hFile(AttemptExclusiveFileOpenW(ConfigPath));
			if (!hFile == true)
			{				
				continue;
			}

			// read the XML file data into a stream
			CComPtr<IStream> pStream;
			if (CIStreamHelper::GetFileDataAsStream(hFile, &pStream) != S_OK)
			{
				continue;
			}	

			// create an XML DOM
			CComPtr<IXMLDOMDocument> pDoc;
			HRESULT hr = pDoc.CoCreateInstance(L"MSXML.DOMDocument");
			if (hr != S_OK)
			{
				continue;
			}

			// load the XML
			VARIANT_BOOL vbSuccess;
			hr = pDoc->load(CComVariant(pStream), &vbSuccess);
			if (hr != S_OK || vbSuccess != -1)
			{
				continue;
			}

			// remove is both 32bit and 64bit modules
			CIIS7XMLConfigHelper::RemoveModuleNodes(pDoc, IISXPRESS_MODULE32NAMEA);
			CIIS7XMLConfigHelper::RemoveModuleNodes(pDoc, IISXPRESS_MODULE64NAMEA);
	
			// commit the XML document to the stream object
			DWORD dwStreamLength = 0;
			CIStreamHelper::CommitXMLToStream(pDoc, pStream, dwStreamLength);

			// flush the stream to disk
			CIStreamHelper::CommitStreamToFile(pStream, dwStreamLength, hFile);
		}
	}

	// remove is both 32bit and 64bit modules
	CIIS7XMLConfigHelper::RemoveModuleNodes(pDoc, IISXPRESS_MODULE32NAMEA);
	CIIS7XMLConfigHelper::RemoveModuleNodes(pDoc, IISXPRESS_MODULE64NAMEA);

	// commit the XML document to the stream object
	DWORD dwStreamLength = 0;
	CIStreamHelper::CommitXMLToStream(pDoc, pStream, dwStreamLength);

	// flush the stream to disk
	CIStreamHelper::CommitStreamToFile(pStream, dwStreamLength, hFile);

	return hr == S_OK ? 1 : -1;
}

// Determines whether the user running the current process is admin or not
//
// incoming:		none
//
// returns:
//  -1 : failed with an error condition
//   0 : not admin
//   1 : admin
LONG APIENTRY IsUserAdmin(void)
{
	HANDLE hProcessHandle = GetCurrentProcess();
	HANDLE hProcessToken = NULL;
	if (::OpenProcessToken(hProcessHandle, TOKEN_READ, &hProcessToken) == FALSE)
	{
		return -1;
	}

	LONG nAdmin = 0;
	
	// create the admin group sid
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup = NULL;
	if (::AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, 
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup) == TRUE)
	{
		// get the group membership from the process token (we need the size first)
		DWORD dwGroupsSize = 0;
		::GetTokenInformation(hProcessToken, TokenGroups, NULL, 0, &dwGroupsSize);
		if (dwGroupsSize > 0)
		{
			BYTE* pbyBuffer = (BYTE*) alloca(dwGroupsSize);
			if (::GetTokenInformation(hProcessToken, TokenGroups, pbyBuffer, dwGroupsSize, &dwGroupsSize) == TRUE)
			{
				TOKEN_GROUPS* pGroups = (TOKEN_GROUPS*) pbyBuffer;

				for (int i = 0; i < (int) pGroups->GroupCount; i++)
				{
					// check that the group is enabled, then that the sid matches the admin group sid
					if ((pGroups->Groups[i].Attributes & SE_GROUP_ENABLED) != 0 &&
						::EqualSid(AdministratorsGroup, pGroups->Groups[i].Sid) == TRUE)
					{
						nAdmin = 1;
						break;
					}
				}
			}
		}

		::FreeSid(AdministratorsGroup);
	}
	else
	{
		nAdmin = -1;
	}

	::CloseHandle(hProcessToken);

	return nAdmin;
}

// This version of ImageLoad deals with the const problem that Microsoft introduced with newer versions of ImageLoad
PLOADED_IMAGE MyImageLoad(LPCSTR pszFilename, LPCSTR pszPath)
{
	PLOADED_IMAGE pImage = ::ImageLoad(const_cast<PSTR>(pszFilename), const_cast<PSTR>(pszPath));
	return pImage;
}

// Gets the bitness of an image
//
// incoming:		the full path to the image
//
// returns:
//  -1 : failed (image does not exist, etc)
//   32: it was a 32bit image
//   64: it was a 64bit image
int GetImageBitness(LPCSTR pszImagePath)
{
	if (pszImagePath == NULL)
	{
		return -1;
	}

	// we need to chop up the path to seperate the filename
	CAtlStringA sPath(pszImagePath);
	CPathT<CAtlStringA> path(sPath);
	if (path.FileExists() == FALSE)
	{
		return -1;
	}

	int filePos = path.FindFileName();
	if (filePos < 0)
	{
		return -1;
	}

	LPCSTR pszFilename = path + filePos;
	CAtlStringA sFilename(pszFilename);
	pszFilename = NULL;

	path.RemoveFileSpec();

	// attempt to load the image
	PLOADED_IMAGE pImage = MyImageLoad(sFilename, path);
	if (pImage == NULL)
	{
		return -1;
	}

	// get the bitness from the header
	int bitness = 32;
	if (pImage->FileHeader != NULL && pImage->FileHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
	{
		bitness = 64;
	}

	::ImageUnload(pImage);
	pImage = NULL;

	return bitness;
}