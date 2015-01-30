// IISxpressCompressionStudio.h : main header file for the IISxpressCompressionStudio application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "IISxpressCompressionStudio_i.h"


// CIISxpressCompressionStudioApp:
// See IISxpressCompressionStudio.cpp for the implementation of this class
//

class CIISxpressCompressionStudioApp : public CWinApp
{
public:
	CIISxpressCompressionStudioApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);
};

extern CIISxpressCompressionStudioApp theApp;