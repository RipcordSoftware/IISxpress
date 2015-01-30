// IISxpressTray.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "IISxpressTray_h.h"


// CIISxpressTrayApp:
// See IISxpressTray.cpp for the implementation of this class
//

class CIISxpressTrayApp : public CWinApp
{
public:
	CIISxpressTrayApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);

private:

	bool IsAlreadyRunning(void);
	bool IsAlreadyRunning_TH(void);
	bool IsAlreadyRunning_EP(void);

	void SetLanguage(void);

	CIISxpressTrayPropertySheet*	m_pSheet;
	CGeneralPropertyPage*			m_pGeneralPage;
	CHistoryPage*					m_pHistoryPage;
	CExclusionsPage*				m_pExclusionsPage;
	CConfigPage*					m_pConfigPage;
	CSystemPage*					m_pSystemPage;
	CAboutPage*						m_pAboutPage;
};

extern CIISxpressTrayApp theApp;