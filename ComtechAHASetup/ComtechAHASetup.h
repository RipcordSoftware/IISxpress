// ComtechAHASetup.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "ComtechAHASetupSheet.h"
#include "WelcomePage.h"

// CComtechAHASetupApp:
// See ComtechAHASetup.cpp for the implementation of this class
//

class CComtechAHASetupApp : public CWinApp
{
public:
	CComtechAHASetupApp();
	~CComtechAHASetupApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:

	CComtechAHASetupSheet* m_pSheet;
	CWelcomePage* m_pWelcomePage;
};

extern CComtechAHASetupApp theApp;