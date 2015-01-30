// RipcordSoftwareLib.h : main header file for the RipcordSoftwareLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRipcordSoftwareLibApp
// See RipcordSoftwareLib.cpp for the implementation of this class
//

class CRipcordSoftwareLibApp : public CWinApp
{
public:
	CRipcordSoftwareLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
