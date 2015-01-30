#pragma once

#include <atlstr.h>

#include "AutoHandle.h"

class CIISxpressRegSettings
{

public:

	CIISxpressRegSettings(void);

	virtual ~CIISxpressRegSettings(void);	

	virtual void Init(LPCTSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler = false);

	virtual bool Load(void);
	virtual bool Save(void);

	virtual bool GetEnabled(void);
	virtual void SetEnabled(bool bEnabled, bool bSave = true);

	virtual bool GetDebugEnabled(void);
	//virtual void SetDebugEnabled(bool bEnabled, bool bSave = true);

	virtual DWORD GetLoggingLevel(void);
	virtual bool SetLoggingLevel(DWORD dwLevel, bool bSave = true);	

	DWORD GetLoadCookie(void) { return m_dwLoadCookie; }

	template <typename T> void GetInstallPath(T& path) { path = m_sInstallPath; }

protected:

	DWORD MakeLoadCookie();

	CComAutoCriticalSection		m_csConfig;

	HANDLE							m_hChangeNotificationThread;
	AutoHandle::AutoEventHandle		m_NotificationTermEvent;
	static DWORD WINAPI				ChangeNotificationProc(void*);
	DWORD							ChangeNotificationProc(void);
	
	CAtlString					m_sRegPath;
	volatile bool				m_bDirty;

	volatile DWORD				m_dwEnabled;
	volatile DWORD				m_dwDebugEnabled;
	volatile DWORD				m_dwLoggingLevel;	

	volatile DWORD				m_dwLoadCookie;

	CAtlString					m_sInstallPath;

	DWORD						m_dwOpenFlags;
};
