// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"

#include "MainFrm.h"

#include <atlpath.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define KB_MB_THRESHOLD		(50 * 1024)

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_TOOLS_LOCATEIP, OnLocateIP)
	ON_COMMAND(ID_TOOLS_IISADMINISTRATION, OnAdminTool)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MENUXP_MESSAGES()
	ON_COMMAND(ID_TOOLBARS_LOCATE, OnToolbarsLocate)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARS_LOCATE, OnUpdateUIToolbarsLocate)
	ON_COMMAND(ID_TOOLBARS_BROWSERS, OnToolbarsBrowsers)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARS_BROWSERS, OnUpdateUIToolbarsBrowsers)
	ON_COMMAND(ID_TOOLBARS_EXCLUSIONRULES, OnToolbarsExclusionRules)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARS_EXCLUSIONRULES, OnUpdateUIToolbarsExclusionRules)
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CMainFrame, CFrameWnd);

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_STATUSBAR_NUMRESPONSES,
	IDS_STATUSBAR_CACHEHITS,
	IDS_STATUSBAR_DATAIN,
	IDS_STATUSBAR_DATAOUT,
	IDS_STATUSBAR_COMPRESSIONRATIO
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_bTimerRunning(false), m_dwLastTimerEvent(0)
{
	m_sKBUnits.LoadString(IDS_KB_UNITS);		
	m_sMBUnits.LoadString(IDS_MB_UNITS);
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: replace with custom toolbar code
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	//if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
	//	CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	//{
	//	TRACE0("Failed to create dialogbar\n");
	//	return -1;		// fail to create
	//}

	EnableDocking(CBRS_ALIGN_TOP);

	if (!m_wndExclusionRuleToolbar.Create(this, CExclusionRuleToolbar::IDD, CBRS_GRIPPER | CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR + 1))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}	

	m_wndExclusionRuleToolbar.EnableDocking(CBRS_ALIGN_TOP);	
	DockControlBar(&m_wndExclusionRuleToolbar);

	if (!m_wndLocateIPDlg.Create(this, CLocateIPToolbarDlg::IDD, CBRS_GRIPPER | CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR + 2))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}	

	m_wndLocateIPDlg.EnableDocking(CBRS_ALIGN_TOP);	
	DockControlBar(&m_wndLocateIPDlg);

	if (!m_wndBrowserDlg.Create(this, CBrowsersToolbarDlg::IDD, CBRS_GRIPPER | CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR + 3))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}	

	m_wndBrowserDlg.EnableDocking(CBRS_ALIGN_TOP);	
	DockControlBar(&m_wndBrowserDlg);

	//if (!m_wndReBar.Create(this) ||
	//	//!m_wndReBar.AddBar(&m_wndToolBar) ||		
	//	//!m_wndReBar.AddBar(&m_wndDlgBar) || 
	//	!m_wndReBar.AddBar(&m_wndLocateIPDlg) ||
	//	!m_wndReBar.AddBar(&m_wndBrowserDlg))
	//{
	//	TRACE0("Failed to create rebar\n");
	//	return -1;      // fail to create
	//}	

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips
	//m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
	//	CBRS_TOOLTIPS | CBRS_FLYBY);	

	// TODO: add menu bitmaps
	CMenuXP::SetXPLookNFeel(this);

	RestoreWindowPosition();
	LoadBarState(IISXPRESSCS_SETTINGS_TOOLBARS);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CContextSelectionView), CSize(140, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CStackedView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	StartTimer();	

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// switch off the unwanted window title decoration
	cs.style &= (~FWS_ADDTOTITLE);

	return TRUE;
}

void CMainFrame::OnDestroy()
{
	StopTimer();

	SaveBarState(IISXPRESSCS_SETTINGS_TOOLBARS);
	SaveWindowPosition();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::StartTimer(void)
{
	if (m_bTimerRunning == false)
	{
		SetTimer(MAINFRAME_TIMER_ID, MAINFRAME_TIMER_INTERVAL, NULL);
		m_bTimerRunning = true;
	}
}

void CMainFrame::StopTimer(void)
{
	if (m_bTimerRunning == true)
	{
		KillTimer(MAINFRAME_TIMER_ID);
		m_bTimerRunning = false;
	}
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	DWORD dwNow = ::GetTickCount();

	// only handle timer if more than the defined number of ms have elapsed
	if (nIDEvent == MAINFRAME_TIMER_ID && dwNow >= (m_dwLastTimerEvent + MAINFRAME_TIMER_INTERVAL))
	{
		m_dwLastTimerEvent = ::GetTickCount();

		// attempt to connect to the IISxpress server
		ConnectToServer();

		// update the status bar
		UpdateStatusBarActivity();
	}
}

bool CMainFrame::ConnectToServer()
{
	static bool bConnected = false;

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetActiveDocument());
	if (pDoc != NULL && pDoc->GetIISxpressInterfaces() == S_OK)
	{
		if (bConnected == false)
		{
			bConnected = true;

			CNotifyServerState state(bConnected);
			GetActiveDocument()->UpdateAllViews(NULL, 0, &state);			
		}
	}
	else
	{
		if (bConnected == true)
		{
			bConnected = false;

			CNotifyServerState state(bConnected);
			GetActiveDocument()->UpdateAllViews(NULL, 0, &state);
		}

		bConnected = false;
	}

	return bConnected;
}

void CMainFrame::UpdateStatusBarActivity(void)
{
	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetActiveDocument());

	CString sCompressedResponses;
	sCompressedResponses.LoadString(IDS_STATUSBAR_NUMRESPONSES);

	CString sCacheHits;
	sCacheHits.LoadString(IDS_STATUSBAR_CACHEHITS);

	CString sDataIn;
	sDataIn.LoadString(IDS_STATUSBAR_DATAIN);

	CString sDataOut;
	sDataOut.LoadString(IDS_STATUSBAR_DATAOUT);

	CString sCompressionRatio;
	sCompressionRatio.LoadString(IDS_STATUSBAR_COMPRESSIONRATIO);

	CString sDataInUnits = m_sKBUnits;	
	CString sDataOutUnits = m_sKBUnits;	

	if (pDoc != NULL)
	{
		CComPtr<ICompressionRuleManager> pCompressionManager;
		pDoc->GetCompressionManager(&pCompressionManager);
		if (pCompressionManager != NULL)
		{
			CComQIPtr<ICompressionStats> pStats = pCompressionManager;
			if (pStats != NULL)
			{
				DWORD dwCompressedResponses = 0;
				DWORD dwRawSize = 0;
				DWORD dwCompressedSize = 0;
				if (pStats->GetStats(&dwCompressedResponses, &dwRawSize, &dwCompressedSize) == S_OK)
				{
					CString sData;					
					sData.Format(_T("%u"), dwCompressedResponses);
					CInternationalHelper::FormatIntegerNumber(sData);
					sCompressedResponses.TrimRight();
					sCompressedResponses += ' ';
					sCompressedResponses += sData;

					// only do ratio calculation if dwRawSize > 0 to get around divide by zero problem
					if (dwRawSize > 0)
					{
						float fCompressionRatio = (float) dwCompressedSize;
						fCompressionRatio /= (float) dwRawSize;
						fCompressionRatio = 1.0f - fCompressionRatio;
						fCompressionRatio *= 100.0f;

						// if the compression ratio is somehow less than zero set it to zero!
						if (fCompressionRatio < 0.0f)
							fCompressionRatio = 0.0f;

						sData.Format(_T("%.1f"), fCompressionRatio);	
						CInternationalHelper::FormatFloatNumber(sData, 1);
						sCompressionRatio.TrimRight();
						sCompressionRatio += ' ';
						sCompressionRatio += sData;
						sCompressionRatio += _T(" %");
					}

					if (dwRawSize >= KB_MB_THRESHOLD)
					{
						// turn into Mb
						dwRawSize >>= 10;
						sDataInUnits = m_sMBUnits;
					}
					
					sData.Format(_T("%u"), dwRawSize);
					CInternationalHelper::FormatIntegerNumber(sData);
					sDataIn.TrimRight();
					sDataIn += ' ';
					sDataIn += sData;
					sDataIn += ' ';
					sDataIn += sDataInUnits;

					if (dwCompressedSize >= KB_MB_THRESHOLD)
					{
						// turn into Mb
						dwCompressedSize >>= 10;
						sDataOutUnits = m_sMBUnits;
					}
					
					sData.Format(_T("%u"), dwCompressedSize);
					CInternationalHelper::FormatIntegerNumber(sData);
					sDataOut.TrimRight();
					sDataOut += ' ';
					sDataOut += sData;
					sDataOut += ' ';
					sDataOut += sDataOutUnits;
				}

				CComQIPtr<ICompressionStats2> pStats2 = pCompressionManager;
				if (pStats2 != NULL)
				{
					DWORD dwCacheHits = 0;
					DWORD dwCacheRawSize = 0;
					DWORD dwCacheCompressedSize = 0;
					if (pStats2->GetCacheStats(&dwCacheHits, &dwCacheRawSize, &dwCacheCompressedSize) == S_OK)
					{						
						CString sData;
						sData.Format(_T("%u"), dwCacheHits);
						CInternationalHelper::FormatIntegerNumber(sData);

						sCacheHits.TrimRight();
						sCacheHits += _T(" ");
						sCacheHits += sData;
					}				
				}
			}
		}
	}

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(IDS_STATUSBAR_NUMRESPONSES), sCompressedResponses, TRUE);
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(IDS_STATUSBAR_CACHEHITS), sCacheHits, TRUE);
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(IDS_STATUSBAR_DATAIN), sDataIn, TRUE);
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(IDS_STATUSBAR_DATAOUT), sDataOut, TRUE);
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(IDS_STATUSBAR_COMPRESSIONRATIO), sCompressionRatio, TRUE);
}

void CMainFrame::OnLocateIP()
{
	CLocateIPDlg dlg;
	dlg.DoModal();
}

void CMainFrame::MakeDisplayCookie(CString& sCookie)
{
	sCookie.Format(_T("%d%d%d%d"), 
		::GetSystemMetrics(SM_XVIRTUALSCREEN), ::GetSystemMetrics(SM_YVIRTUALSCREEN),
		::GetSystemMetrics(SM_CXVIRTUALSCREEN), ::GetSystemMetrics(SM_CYVIRTUALSCREEN));
}

void CMainFrame::RestoreWindowPosition()
{
	// recover the prevous screen coordinates and display cookie from the registry
	CWinApp* pApp = ::AfxGetApp();
	const int nInvalid = 1 << 31;
	int nXPos = pApp->GetProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWPOSX, nInvalid);
	int nYPos = pApp->GetProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWPOSY, nInvalid);
	int nWidth = pApp->GetProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWWIDTH, nInvalid);
	int nHeight = pApp->GetProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWHEIGHT, nInvalid);
	BOOL bZoomed = pApp->GetProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWMAXIMIZED, FALSE);
	CString sOldDisplayCookie = pApp->GetProfileString(IISXPRESSCS_SETTINGS, IISXPRESSCS_DISPLAYCOOKIE, _T(""));

	// make the current display cookie
	CString sDisplayCookie;
	MakeDisplayCookie(sDisplayCookie);

	// if we have valid coords and a matching display cookie then attempt to position the window
	if (nXPos != nInvalid && nYPos != nInvalid && nWidth != nInvalid && nHeight != nInvalid && sDisplayCookie == sOldDisplayCookie)
	{
		// get the min X and Y screen coords
		int nXMin = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
		int nYMin = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
		
		// get the virtual screen width and height
		int nScreenWidth = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int nScreenHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		// get the virtual screen rectangle
		CRect rcScreen(nXMin, nYMin, nXMin + nScreenWidth, nYMin + nScreenHeight);

		// get the sheet rectangle
		CRect rcSheet;
		GetWindowRect(rcSheet);
		rcSheet.MoveToXY(nXPos, nYPos);

		// get the intersection between the screen and the sheet rectangles
		CRect rcIntersect;
		rcIntersect.IntersectRect(rcScreen, rcSheet);

		// if we have a valid intersection then move the window to the stored position
		if (rcIntersect.IsRectNull() == FALSE)
		{			
			SetWindowPos(NULL, nXPos, nYPos, nWidth, nHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);						

			if (bZoomed == TRUE)
			{
				AfxGetApp()->m_nCmdShow = SW_SHOWMAXIMIZED;
			}
		}
	}
}

void CMainFrame::SaveWindowPosition()
{
	// get the window normal location
	WINDOWPLACEMENT windowPlacement;
	windowPlacement.length = sizeof(windowPlacement);
	GetWindowPlacement(&windowPlacement);

	CRect rcNormal(windowPlacement.rcNormalPosition);

	// save the window location to the registry
	CWinApp* pApp = ::AfxGetApp();
	pApp->WriteProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWPOSX, rcNormal.left);
	pApp->WriteProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWPOSY, rcNormal.top);
	pApp->WriteProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWWIDTH, rcNormal.Width());
	pApp->WriteProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWHEIGHT, rcNormal.Height());

	pApp->WriteProfileInt(IISXPRESSCS_SETTINGS, IISXPRESSCS_WINDOWMAXIMIZED, IsZoomed());

	// save the display cookie
	CString sDisplayCookie;
	MakeDisplayCookie(sDisplayCookie);
	pApp->WriteProfileString(IISXPRESSCS_SETTINGS, IISXPRESSCS_DISPLAYCOOKIE, sDisplayCookie);
}

void CMainFrame::OnAdminTool(void)
{
	TCHAR szSysDir[MAX_PATH + 1];
	if (::GetSystemDirectory(szSysDir, _countof(szSysDir)) == 0)
		return;

	CPathT<CString> AdminToolPath(szSysDir);
	AdminToolPath.Append(_T("inetsrv"));		
	AdminToolPath.Append(_T("iis.msc"));	

	::ShellExecute(::GetDesktopWindow(), _T("open"), AdminToolPath, NULL, szSysDir, SW_SHOWNORMAL);
}

void CMainFrame::OnToolbarsLocate()
{
	int id = m_wndLocateIPDlg.GetDlgCtrlID();
	CControlBar* pControlBar = GetControlBar(id);
	if (pControlBar != NULL)
	{
		BOOL bVisible = m_wndLocateIPDlg.IsWindowVisible() == TRUE ? FALSE : TRUE;
		ShowControlBar(pControlBar, bVisible, FALSE);
	}
}

void CMainFrame::OnUpdateUIToolbarsLocate(CCmdUI *pCmdUI)
{
	int id = m_wndLocateIPDlg.GetDlgCtrlID();
	CControlBar* pControlBar = GetControlBar(id);
	if (pControlBar != NULL)
	{
		BOOL bVisible = m_wndLocateIPDlg.IsWindowVisible();
		pCmdUI->SetCheck(bVisible == TRUE ? 1 : 0);
	}
}

void CMainFrame::OnToolbarsBrowsers()
{
	int id = m_wndBrowserDlg.GetDlgCtrlID();
	CControlBar* pControlBar = GetControlBar(id);
	if (pControlBar != NULL)
	{
		BOOL bVisible = m_wndBrowserDlg.IsWindowVisible() == TRUE ? FALSE : TRUE;
		ShowControlBar(pControlBar, bVisible, FALSE);
	}
}

void CMainFrame::OnUpdateUIToolbarsBrowsers(CCmdUI *pCmdUI)
{
	int id = m_wndBrowserDlg.GetDlgCtrlID();
	CControlBar* pControlBar = GetControlBar(id);
	if (pControlBar != NULL)
	{
		BOOL bVisible = m_wndBrowserDlg.IsWindowVisible();
		pCmdUI->SetCheck(bVisible == TRUE ? 1 : 0);
	}
}

void CMainFrame::OnToolbarsExclusionRules()
{
	int id = m_wndExclusionRuleToolbar.GetDlgCtrlID();
	CControlBar* pControlBar = GetControlBar(id);
	if (pControlBar != NULL)
	{
		BOOL bVisible = m_wndExclusionRuleToolbar.IsWindowVisible() == TRUE ? FALSE : TRUE;
		ShowControlBar(pControlBar, bVisible, FALSE);
	}
}

void CMainFrame::OnUpdateUIToolbarsExclusionRules(CCmdUI *pCmdUI)
{
	int id = m_wndExclusionRuleToolbar.GetDlgCtrlID();
	CControlBar* pControlBar = GetControlBar(id);
	if (pControlBar != NULL)
	{
		BOOL bVisible = m_wndExclusionRuleToolbar.IsWindowVisible();
		pCmdUI->SetCheck(bVisible == TRUE ? 1 : 0);
	}
}
