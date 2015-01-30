// ConfigCacheDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "ConfigCacheDlg.h"

BEGIN_MESSAGE_MAP(CWantReturnEdit, CEdit)	
	ON_MESSAGE(WM_GETDLGCODE, OnGetDlgCode)
END_MESSAGE_MAP()

LRESULT CWantReturnEdit::OnGetDlgCode(WPARAM, LPARAM lParam)
{
	LPMSG pMsg = (LPMSG) lParam;
	if (pMsg != NULL && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		NMHDR hdr;
		hdr.code = NM_RETURN;
		hdr.hwndFrom = GetSafeHwnd();
		hdr.idFrom = GetDlgCtrlID();

		// tell the parent window about it (like it should do really)
		::SendNotifyMessage(GetParent()->GetSafeHwnd(), WM_NOTIFY, hdr.idFrom, (LPARAM) &hdr);
	}	

	// normal character processing is required
	return DLGC_WANTCHARS | DLGC_WANTARROWS;	
}

// CConfigCacheDlg dialog

IMPLEMENT_DYNAMIC(CConfigCacheDlg, CHelpAwareDialogBase)

CConfigCacheDlg::CConfigCacheDlg(CWnd* pParent /*=NULL*/)
	: CHelpAwareDialogBase(CConfigCacheDlg::IDD, pParent)
{
	MEMORYSTATUSEX MemoryStatus;
	MemoryStatus.dwLength = sizeof(MemoryStatus);
	::GlobalMemoryStatusEx(&MemoryStatus);

	// get the total physical memory on the machine in KB, then limit at 10% of what is available
	m_dwMaxLimitKB = (DWORD) (MemoryStatus.ullTotalPhys / 1024 / 10);	
}

CConfigCacheDlg::~CConfigCacheDlg()
{
}

void CConfigCacheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLECACHE, m_cEnableCache);
	DDX_Control(pDX, IDC_EDIT_MAXCACHESIZE, m_cMaxMemory);
	DDX_Control(pDX, IDC_EDIT_MAXCACHERESPONSES, m_cMaxResponses);
	DDX_Control(pDX, IDC_BUTTON_CACHEFLUSH, m_cFlushButton);
}


BEGIN_MESSAGE_MAP(CConfigCacheDlg, CHelpAwareDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_CACHEFLUSH, OnBnClickedButtonCacheFlush)
	ON_BN_CLICKED(IDC_CHECK_ENABLECACHE, OnBnClickedCheckEnableCache)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXCACHESIZE, OnEnKillFocusEditMaxCacheSize)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXCACHERESPONSES, OnEnKillFocusEditMaxCacheResponses)
	ON_NOTIFY(NM_RETURN, IDC_EDIT_MAXCACHESIZE, OnNmReturnMaxCacheSize)
	ON_NOTIFY(NM_RETURN, IDC_EDIT_MAXCACHERESPONSES, OnNmReturnMaxResponses)
	ON_EN_CHANGE(IDC_EDIT_MAXCACHESIZE, OnEnChangeEditMaxCacheSize)
	ON_EN_CHANGE(IDC_EDIT_MAXCACHERESPONSES, OnEnChangeEditMaxCacheResponses)
END_MESSAGE_MAP()


// CConfigCacheDlg message handlers

BOOL CConfigCacheDlg::OnInitDialog(void)
{
	BOOL bStatus = CHelpAwareDialogBase::OnInitDialog();	

	m_RegSettings.Init(IISXPRESSFILTER_REGKEY, false, false);
	if (m_RegSettings.Load() == true)
	{
		BOOL bCacheEnabled = m_RegSettings.GetCacheEnabled();
		if (bCacheEnabled == TRUE)
		{
			m_cEnableCache.SetCheck(BST_CHECKED);
		}
		else
		{
			m_cEnableCache.SetCheck(BST_UNCHECKED);
		}

		DWORD dwMaxSize = m_RegSettings.GetCacheMaxSizeKB();

		CString sMaxSize;
		sMaxSize.Format(_T("%u"), dwMaxSize);
		m_cMaxMemory.SetWindowText(sMaxSize);

		DWORD dwMaxResponses = m_RegSettings.GetCacheMaxEntries();

		CString sMaxResponses;
		sMaxResponses.Format(_T("%u"), dwMaxResponses);
		m_cMaxResponses.SetWindowText(sMaxResponses);

		// make sure the enable/disabled state of the controls is correct
		OnBnClickedCheckEnableCache();
	}

	LOGFONT logfontNormal;
	m_cMaxMemory.GetFont()->GetLogFont(&logfontNormal);
	m_ftNormal.CreateFontIndirect(&logfontNormal);

	LOGFONT logfontItalic = logfontNormal;
	logfontItalic.lfItalic = TRUE;
	logfontItalic.lfWeight = FW_BOLD;
	m_ftItalic.CreateFontIndirect(&logfontItalic);

	return bStatus;
}

void CConfigCacheDlg::ServerOnline(void)
{
}

void CConfigCacheDlg::ServerOffline(void)
{
}

void CConfigCacheDlg::Heartbeat(void)
{	
}

void CConfigCacheDlg::UpdateControls(void)
{
}

void CConfigCacheDlg::OnBnClickedButtonCacheFlush()
{		
	CIISxpressTrayPropertySheet::ResetResponseCache();

	CString sCaption;
	sCaption.LoadString(IDS_IISXPRESS_APPNAME);

	CString sMsg;
	sMsg.LoadString(IDS_CACHEFLUSHOK);

	MessageBox(sMsg, sCaption, MB_OK | MB_ICONINFORMATION);
}

void CConfigCacheDlg::OnBnClickedCheckEnableCache()
{
	BOOL bEnabled = m_cEnableCache.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	if (bEnabled == TRUE)
	{
		m_RegSettings.SetCacheEnabled(TRUE);

		m_cMaxMemory.EnableWindow(TRUE);
		m_cMaxResponses.EnableWindow(TRUE);
		m_cFlushButton.EnableWindow(TRUE);
	}
	else
	{
		m_RegSettings.SetCacheEnabled(FALSE);

		m_cMaxMemory.EnableWindow(FALSE);
		m_cMaxResponses.EnableWindow(FALSE);
		m_cFlushButton.EnableWindow(FALSE);
	}
}

void CConfigCacheDlg::OnEnKillFocusEditMaxCacheSize()
{
	DWORD dwOldMaxSizeKB = m_RegSettings.GetCacheMaxSizeKB();	

	CString sMaxMemory;
	m_cMaxMemory.GetWindowText(sMaxMemory);
	DWORD dwNewMaxSizeKB = _ttoi(sMaxMemory);

	bool bUpdateControl = false;

	if (dwNewMaxSizeKB < 128)
	{
		dwNewMaxSizeKB = 128;
		bUpdateControl = true;
	}
	else if (dwNewMaxSizeKB > m_dwMaxLimitKB)
	{				
		dwNewMaxSizeKB = m_dwMaxLimitKB;
		bUpdateControl = true;
	}			

	if (bUpdateControl == true)
	{
		CString sMaxMemory;
		sMaxMemory.Format(_T("%u"), dwNewMaxSizeKB);
		m_cMaxMemory.SetWindowText(sMaxMemory);
	}

	if (dwNewMaxSizeKB != dwOldMaxSizeKB)
	{
		m_RegSettings.SetCacheMaxSizeKB(dwNewMaxSizeKB);
	}

	if (((HFONT) m_ftNormal) != NULL)
		m_cMaxMemory.SetFont(&m_ftNormal, TRUE);
}

void CConfigCacheDlg::OnEnKillFocusEditMaxCacheResponses()
{
	DWORD dwOldMaxResponses = m_RegSettings.GetCacheMaxEntries();	

	CString sMaxResponses;
	m_cMaxResponses.GetWindowText(sMaxResponses);
	DWORD dwNewMaxResponses = _ttoi(sMaxResponses);

	bool bUpdateControl = false;
	if (dwNewMaxResponses < 16)
	{
		dwNewMaxResponses = 16;
		bUpdateControl = true;		
	}
	else if (dwNewMaxResponses > 32768)
	{
		dwNewMaxResponses = 32768;		
		bUpdateControl = true;		
	}

	if (bUpdateControl == true)
	{
		CString sMaxResponses;
		sMaxResponses.Format(_T("%u"), dwNewMaxResponses);
		m_cMaxResponses.SetWindowText(sMaxResponses);		
	}

	if (dwNewMaxResponses != dwOldMaxResponses)
	{
		m_RegSettings.SetCacheMaxEntries(dwNewMaxResponses);
	}

	if (((HFONT) m_ftNormal) != NULL)
		m_cMaxResponses.SetFont(&m_ftNormal, TRUE);
}

void CConfigCacheDlg::OnNmReturnMaxCacheSize(NMHDR * pNotifyStruct, LRESULT * result)
{
	OnEnKillFocusEditMaxCacheSize();
}

void CConfigCacheDlg::OnNmReturnMaxResponses(NMHDR* pNotifyStruct, LRESULT* result)
{
	OnEnKillFocusEditMaxCacheResponses();
}

void CConfigCacheDlg::OnEnChangeEditMaxCacheSize()
{
	if (((HFONT) m_ftItalic) != NULL)
		m_cMaxMemory.SetFont(&m_ftItalic, TRUE);
}

void CConfigCacheDlg::OnEnChangeEditMaxCacheResponses()
{
	if (((HFONT) m_ftItalic) != NULL)
		m_cMaxResponses.SetFont(&m_ftItalic, TRUE);
}
