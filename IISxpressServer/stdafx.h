// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

//#define _ATL_APARTMENT_THREADED
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

// because of a problem in ATLBASE.H in release mode include these two includes here instead
#ifndef _DEBUG
#include <stdio.h>
#include <stdarg.h>
#endif

#include <pdh.h>

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <atlpath.h>

using namespace ATL;

#include "DebugOutput.h"

#include "RegistryKeys.h"
#include "ComIISxpressRegSettings.h"


#include "AppLog.h"
extern CAppLog							g_Log;

// store the OS version globally
extern OSVERSIONINFOEX					g_VerInfo;

#define IISXPRESS_STORAGEFILENAME		_T("IISxpress.config")

#define THROW_HR_ON_NOTOK(hr)					{ if (hr != S_OK) throw hr; }
#define THROW_HR_ON_FAIL(hr)					{ if (FAILED(hr) == TRUE) throw hr; }
#define THROW_HR_ON_NULL(p, hr, code)			{ if (p == NULL) { hr = code; throw hr; } }

void MyOutputDebugString(BOOL bDebugEnabled, const TCHAR* pszMsg, ...);
void MyOutputDebugString(IComIISxpressRegSettings* pRegSettings, const TCHAR* pszMsg, ...);
void MyOutputLoggingString(DWORD dwCurrentLevel, DWORD dwRequiredLevel, const TCHAR* pszMsg, ...);
void MyOutputLoggingString(IComIISxpressRegSettings* pRegSettings, DWORD dwRequiredLevel, const TCHAR* pszMsg, ...);

#include "AutoMemberCriticalSection.h"

#include "Toolhelp32Helper.h"

#include "SetThreadName.h"