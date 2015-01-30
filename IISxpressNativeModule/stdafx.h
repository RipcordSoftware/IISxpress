// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0600		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0600	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0600 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>
#include <atlcache.h>

// we need Vista/Longhorn decls
#include <HttpServ.h>

#include "resource.h"

// we need IISxpress interface decls
#include "..\IISxpressServer\IISxpressServer.h"

#include "RegistryKeys.h"
#include "IISxpressRegSettingsEx.h"

#include "IISxpressModuleFactory.h"
#include "IISxpressHttpModule.h"

#include "ResponseStream.h"

#include "AutoMemberCriticalSection.h"

#include "SetThreadName.h"

#include "AppLog.h"

// the module handle when the DLL is loaded
extern HMODULE g_hModule;

#include "HiResTimer.h"
extern HiResTimer		g_Timer;