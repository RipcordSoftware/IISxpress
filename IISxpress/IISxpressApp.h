#pragma once

// The one and only CWinApp object
// NOTE: You may remove this object if you alter your project to no
// longer use MFC in a DLL.

#include "stdafx.h"
#include "IISxpress.h"

class IISxpressHTTPApp : public CWinApp
{

public:

	IISxpressHTTPApp(LPCTSTR lpszAppName = NULL) : CWinApp(lpszAppName)
	{		
	}

	virtual ~IISxpressHTTPApp(void) 
	{
	}

protected:

	BOOL InitInstance(void);
	int ExitInstance();

};

extern IISxpressHTTPApp theApp;