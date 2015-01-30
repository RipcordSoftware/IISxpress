#include "StdAfx.h"
#include "IISxpressTrayIcon.h"

CIISxpressTrayIcon::CIISxpressTrayIcon(void)
{
	m_hwndParent = NULL;
	m_nTrayNotifyId = -1;
}	

CIISxpressTrayIcon::~CIISxpressTrayIcon(void)
{
	DeleteTrayIcon();
}

void CIISxpressTrayIcon::CreateTrayIcon(HWND hwndParent, UINT nTrayNotifyId, UINT nIconID, UINT nTipID)
{
	ASSERT(hwndParent != NULL);
	if (hwndParent == NULL)
		return;	

	ASSERT(m_hwndParent == NULL);
	if (m_hwndParent != NULL)
		return;	

	// make sure the window is valid
	ASSERT(::IsWindow(hwndParent) == TRUE);

	m_hwndParent = hwndParent;

	m_nTrayNotifyId = nTrayNotifyId;

	// load the icon
	m_hIcon = ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(nIconID));
	ASSERT(m_hIcon != NULL);

	// we can safely assume version 4 of the shell for now
	DWORD dwShellMajorVersion = 4;

	// get the version of the shell
	typedef HRESULT CALLBACK FNDLLGETVERSION(DLLVERSIONINFO *pdvi);
	HMODULE hShell = ::LoadLibrary(_T("shell32.dll"));
	if (hShell != NULL)
	{
		FNDLLGETVERSION* pfnDllGetVersion = (FNDLLGETVERSION*) ::GetProcAddress(hShell, "DllGetVersion");
		if (pfnDllGetVersion != NULL)
		{
			DLLVERSIONINFO VersionInfo;
			VersionInfo.cbSize = sizeof(DLLVERSIONINFO);

			if (pfnDllGetVersion(&VersionInfo) == S_OK)
			{
				dwShellMajorVersion = VersionInfo.dwMajorVersion;				
			}
		}

		::FreeLibrary(hShell);
	}	

	// if we have shell 5.x or better ask for W2K notification area mode
	if (dwShellMajorVersion >= 5)
	{
		NOTIFYICONDATA nid;
		memset(&nid, 0, sizeof(nid));	
		nid.cbSize = sizeof(nid);
		nid.uVersion = NOTIFYICON_VERSION;
		::Shell_NotifyIcon(NIM_SETVERSION, &nid);				
	}

	CString sAppName;
	sAppName.LoadString(nTipID);
	ASSERT(sAppName.GetLength() > 0);

	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));

	nid.cbSize = NOTIFYICONDATA_V1_SIZE;	
	nid.hWnd = m_hwndParent;
	nid.uID = m_nTrayNotifyId;	
	
	if (m_hIcon != NULL)
	{
		nid.uFlags |= NIF_ICON;
		nid.hIcon = m_hIcon;
	}
	
	nid.uFlags |= NIF_TIP;
	_tcscpy_s(nid.szTip, _countof(nid.szTip), sAppName);

	nid.uFlags |= NIF_MESSAGE;
	nid.uCallbackMessage = WM_TRAYMSG;

	// add the plain jane notification icon
	::Shell_NotifyIcon(NIM_ADD, &nid);

	// only when the shell is >= 5.x and the command line contains the /installer
	// switch do we display the balloon
	if (dwShellMajorVersion >= 5)
	{
		LPCTSTR pszCommandLine = ::GetCommandLine();		
		if (pszCommandLine != NULL && _tcsstr(pszCommandLine, _T("/installer")) != NULL)
		{
			NOTIFYICONDATA nid;
			memset(&nid, 0, sizeof(nid));	

			nid.cbSize = sizeof(nid);
			nid.hWnd = m_hwndParent;
			nid.uID = m_nTrayNotifyId;				
			nid.uFlags = NIF_INFO;			
			nid.uTimeout = 10000;
			nid.dwInfoFlags = NIIF_INFO;

			CString sInfo;
			sInfo.LoadString(IDS_STRING_INSTALLCOMPLETED);			

			_tcscpy_s(nid.szInfoTitle, _countof(nid.szInfoTitle), sAppName);
			_tcscpy_s(nid.szInfo, _countof(nid.szInfo), sInfo);						
			
			::Shell_NotifyIcon(NIM_MODIFY, &nid);			
		}
	}
}

void CIISxpressTrayIcon::DeleteTrayIcon(void)
{
	if (m_hwndParent == NULL)
		return;

	// make sure the window is valid
	ASSERT(::IsWindow(m_hwndParent) == TRUE);

	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hwndParent;
	nid.uID = m_nTrayNotifyId;

	::Shell_NotifyIcon(NIM_DELETE, &nid);

	m_hwndParent = NULL;
	m_nTrayNotifyId = -1;
}

void CIISxpressTrayIcon::SetTrayString(UINT nID)
{
	if (m_hwndParent == NULL)
		return;

	CString sMsg;
	sMsg.LoadString(nID);		
	ASSERT(sMsg.GetLength() > 0);

	if (sMsg.GetLength() <= 0)
	{
		sMsg.LoadString(IDS_IISXPRESS_APPNAME);
		ASSERT(sMsg.GetLength() > 0);
	}
	
	SetTrayString(sMsg);
}

void CIISxpressTrayIcon::SetTrayString(const CString& sMsg)
{
	if (m_hwndParent == NULL)
		return;

	// make sure the window is valid
	ASSERT(::IsWindow(m_hwndParent) == TRUE);

	// dont update the tray string unless we really need to
	static CString sLastMsg = _T("xyxyxyxyxyxyx");
	if (sLastMsg == sMsg)
		return;

	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hwndParent;
	nid.uID = m_nTrayNotifyId;
	nid.uFlags = NIF_TIP;
	memcpy_s(nid.szTip, sizeof(nid.szTip), sMsg, sMsg.GetLength() * sizeof(TCHAR));

	::Shell_NotifyIcon(NIM_MODIFY, &nid);

	sLastMsg = sMsg;
}