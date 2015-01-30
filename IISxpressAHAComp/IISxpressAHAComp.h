// IISxpressAHAComp.h : main header file for the IISxpressAHAComp DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIISxpressAHACompApp
// See IISxpressAHAComp.cpp for the implementation of this class
//

class CIISxpressAHACompApp : public CWinApp
{
public:
	CIISxpressAHACompApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
