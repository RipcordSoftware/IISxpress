// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

//#define _ATL_APARTMENT_THREADED 

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afx.h>
#include <afxwin.h>
#include <afxmt.h>		// for synchronization objects
#include <afxext.h>

#include <atlbase.h>
#include <atlstr.h>
#include <atlcoll.h>

#include <HttpFilt.h>

#include "..\IISxpressServer\IISxpressServer.h"
#include "..\IISxpressServer\IISxpressHTTPRequestHResults.h"
#include "..\IISxpressPerf\IISxpressISAPIPerf.h"

EXTERN_C const CLSID CLSID_IISxpressHTTPRequest;
EXTERN_C const IID IID_IIISxpressHTTPRequest;

#include "ResponseStream.h"

#include "..\Libs\RipcordSoftwareLib\import.h"
#include "..\Libs\IISxpressLib\import.h"

extern Ripcord::Utils::HiResTimer g_Timer;
extern Ripcord::Utils::Interlocked64 g_interlock;