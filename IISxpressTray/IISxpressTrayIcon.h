#pragma once

class CIISxpressTrayIcon
{
public:

	CIISxpressTrayIcon(void);
	~CIISxpressTrayIcon(void);

	void CreateTrayIcon(HWND hwndParent, UINT nTrayNotifyId, UINT nIconID, UINT nTipID);
	void DeleteTrayIcon(void);	

	void SetTrayString(UINT nID);
	void SetTrayString(const CString& sMsg);

	UINT GetNotifyId(void) { return m_nTrayNotifyId; }

private:

	HWND				m_hwndParent;
	UINT				m_nTrayNotifyId;
	HICON				m_hIcon;
};
