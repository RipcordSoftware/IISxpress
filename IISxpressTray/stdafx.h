// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _ATL_APARTMENT_THREADED 
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0501	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdlgs.h>

#include <afxtempl.h>		// MFC templates

#include <afxmt.h>			// MFC multi-threaded

#include <atlbase.h>
#include <atlcom.h>
#include <atlpath.h>
#include <atlutil.h>

#include <atlrx.h>			// regular expressions

#include <memory>

#include "resource.h"

#include "..\IISxpressServer\IISxpressServer.h"

#include "..\Libs\RipcordSoftwareLib\import.h"

#include "InternationalHelper.h"

#include "IISxpressPageBase.h"
#include "IISxpressTrayPropertySheet.h"
#include "GeneralPropertyPage.h"
#include "HistoryPage.h"
#include "ExclusionsPage.h"
#include "ConfigPage.h"
#include "SystemPage.h"
#include "AboutPage.h"

extern OSVERSIONINFOEX g_VerInfo;

extern Ripcord::Utils::HiResTimer g_Timer;

#include "UserAgentLookup.h"
extern CUserAgentLookup g_UserAgentLookup;

#include "Toolhelp32Helper.h"

#include "ServiceNames.h"

#include "IISMetabase.h"

#include "RegistryKeys.h"

// we need to dynamically bind to GetProcessImageFileName from PSAPI.DLL
typedef DWORD WINAPI FNGETPROCESSIMAGEFILENAME(HANDLE, LPTSTR, DWORD);
extern FNGETPROCESSIMAGEFILENAME* g_fnGetProcessImageFileName;

#include "ShellImageList.h"
extern CShellImageList g_ShellImageList;

#include "AddExtnWizardSheet.h"
#include "AddExtnWelcomePage.h"
#include "AddExtnManualAddPage.h"
#include "AddExtnAutoAddSelectWebSitesPage.h"
#include "AddExtnAutoAddSelectExtnsPage.h"
#include "AddExtnConfirmPage.h"
#include "AddExtnFinishPage.h"

#include "AddContentTypeWizardSheet.h"
#include "AddContentTypeWelcomePage.h"
#include "AddContentTypeManualAddPage.h"
#include "AddContentTypeSelectWebSitesPage.h"
#include "AddContentTypeAutoAddSearchPage.h"
#include "AddContentTypeSelectContentTypesPage.h"
#include "AddContentTypeConfirmPage.h"
#include "AddContentTypeFinishPage.h"

#include <atlimage.h>
bool LoadPNG(LPCTSTR pszImageName, CImage& Image);

bool LoadRTF(LPCTSTR pszRTFName, CStringA& sRTF);

#include "IIS7XMLConfigHelper.h"
#include "IIS6ConfigHelper.h"

#include "NeverCompressRulesNotify.h"

#include "geoipdata.h"

#include "IISConfigHelper.h"