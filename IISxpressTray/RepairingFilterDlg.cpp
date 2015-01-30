// RepairingFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "RepairingFilterDlg.h"

#include "IISxpressInstallerExtensions.h"


// CRepairingFilterDlg dialog

IMPLEMENT_DYNAMIC(CRepairingFilterDlg, CDialog)
CRepairingFilterDlg::CRepairingFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRepairingFilterDlg::IDD, pParent)
{
	m_hRepairThread = NULL;
}

CRepairingFilterDlg::~CRepairingFilterDlg()
{
}

void CRepairingFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRepairingFilterDlg, CDialog)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CRepairingFilterDlg message handlers

BOOL CRepairingFilterDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();	

	// setup so we can guarantee the message loop will be kicked fairly regularily
	SetTimer(::GetTickCount(), 1000, NULL);

	// setup the data we will need in the repair thread
	m_RepairProcData.sPath = m_sPath;
	m_RepairProcData.hwndDlg = GetSafeHwnd();

	// start the repair thread
	unsigned nThreadId = 0;
	m_hRepairThread = (HANDLE) ::_beginthreadex(NULL, 0, RepairProc, &m_RepairProcData, 0, &nThreadId);

	return TRUE;
}

unsigned __stdcall CRepairingFilterDlg::RepairProc(void* pVoid)
{
	if (pVoid == NULL)
		return 0;

	RepairProcData* pData = (RepairProcData*) pVoid;

	// we are going to use COM so...
	::CoInitialize(NULL);

	// ask for the repair, mode=2 means restart IIS if it was already running
	LONG nMode = 2;
	LONG nStatus = Internal_InstallIISxpressFilter(pData->hwndDlg, nMode, pData->sPath);	

	::CoUninitialize();	

	// ok, bodge the UI a bit, wait for one sheet interval because then we can guarantee the
	// UI will be showing the correct status info for the IIS service and filter
	::Sleep(SHEET_TIMER_INTERVAL);

	if (nStatus == 1)
		return IDOK;
	else
		return IDCANCEL;	
}

BOOL CRepairingFilterDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hWait = ::LoadCursor(NULL, IDC_WAIT);
	::SetCursor(hWait);

	return 1;
}

BOOL CRepairingFilterDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hRepairThread != NULL)
	{
		// test to see if the thread is still running
		if (::WaitForSingleObject(m_hRepairThread, 0) == WAIT_OBJECT_0)
		{
			// get the thread exit code
			DWORD dwExitCode = 0;
			::GetExitCodeThread(m_hRepairThread, &dwExitCode);

			// end the dialog
			EndDialog(dwExitCode);
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}