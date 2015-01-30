// IISxpressLib.h : main header file for the IISxpressLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIISxpressLibApp
// See IISxpressLib.cpp for the implementation of this class
//

class CIISxpressLibApp : public CWinApp
{
public:
	CIISxpressLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
