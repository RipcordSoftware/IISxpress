// ComtechAHASetup.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ComtechAHASetup.h"
#include "ComtechAHASetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CComtechAHASetupApp

BEGIN_MESSAGE_MAP(CComtechAHASetupApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CComtechAHASetupApp construction

CComtechAHASetupApp::CComtechAHASetupApp() : m_pSheet(NULL), m_pWelcomePage(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CComtechAHASetupApp::~CComtechAHASetupApp()
{
	delete m_pWelcomePage;
	delete m_pSheet;
}

// The one and only CComtechAHASetupApp object

CComtechAHASetupApp theApp;

// CComtechAHASetupApp initialization

BOOL CComtechAHASetupApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//CComtechAHASetupDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with OK
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with Cancel
	//}

	CBitmap bmpWelcome;
	bmpWelcome.LoadBitmap(IDB_BITMAP_WELCOME);

	m_pSheet = new CComtechAHASetupSheet(IDS_CAPTION, NULL, 0, bmpWelcome);

	m_pSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_WIZARD97;
	m_pSheet->m_psh.dwFlags &= (~PSH_HASHELP);

	m_pWelcomePage = new CWelcomePage();
	m_pSheet->AddPage(m_pWelcomePage);

	m_pSheet->Create(NULL, WS_VISIBLE | DS_MODALFRAME | DS_3DLOOK | DS_CONTEXTHELP | DS_SETFONT | 
		WS_POPUP | WS_CAPTION | WS_SYSMENU);	

	m_pMainWnd = m_pSheet;	

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}
