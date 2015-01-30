#include "stdafx.h"

#include "IISxpressRegSettings.h"
#include "RegistryKeys.h"

namespace Ripcord { namespace IISxpress {

IISxpressRegSettings::IISxpressRegSettings(void) : m_dwOpenFlags(0), m_hChangeNotificationThread(INVALID_HANDLE_VALUE)
{	
	m_dwLoadCookie = MakeLoadCookie();
}

IISxpressRegSettings::~IISxpressRegSettings(void)
{
	if (m_bDirty == true)
	{
		Save();
	}

	// if we started the notification thread then we need to ask it to stop
	if (!!m_NotificationTermEvent)
	{
		::SetEvent(m_NotificationTermEvent);

		// wait for the thread to terminate
		::WaitForSingleObject(m_hChangeNotificationThread, INFINITE);
	}
}

void IISxpressRegSettings::Init(LPCSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler)
{
	Init(CAtlStringW(pszRegPath), bLoad, bRegisterChangeHandler);
}

void IISxpressRegSettings::Init(LPCWSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler)
{
	m_bDirty = true;

	m_sRegPath = pszRegPath;

	m_dwEnabled = 1;
	m_dwDebugEnabled = 0;
	m_dwLoggingLevel = 0;

	if (bLoad == true)
	{
		Load();
	}

	// does the user want an automatic change handler?
	if (bRegisterChangeHandler == true)
	{
		// we need an event to kill the notificiation thread
		HANDLE hHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (hHandle != INVALID_HANDLE_VALUE)
		{
			// automatically manage the handle
			m_NotificationTermEvent.Attach(hHandle);
			hHandle = INVALID_HANDLE_VALUE;

			// create the thread (use CreateThread because of the DllMain locking issues)
			DWORD nChangeNotificationThreadId = 0;			
			m_hChangeNotificationThread = ::CreateThread(NULL, 0, ChangeNotificationProc, this, 0, &nChangeNotificationThreadId);

#if _DEBUG
			Ripcord::Utils::Threading::SetThreadName(nChangeNotificationThreadId, "ChangeNotificationProc");
#endif
		}
	}
}

DWORD WINAPI IISxpressRegSettings::ChangeNotificationProc(void* pData)
{
	DWORD dwStatus = ((IISxpressRegSettings*) pData)->ChangeNotificationProc();

	// terminate the thread (the destructor will be waiting)
	::TerminateThread(::GetCurrentThread(), dwStatus);

	return dwStatus;
}

DWORD IISxpressRegSettings::ChangeNotificationProc(void)
{	
	// we want low priority
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_LOWEST);

	// open the registry key for notifications
	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_NOTIFY | m_dwOpenFlags) != ERROR_SUCCESS)
	{
		return 0;
	}

	// create the notification event
	HANDLE hRegChange = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hRegChange == NULL)
		return 0;

	// allow the event to be automatically managed
	Ripcord::Types::AutoHandle::AutoEventHandle RegChangeEvent(hRegChange);
	hRegChange = INVALID_HANDLE_VALUE;

	// the events we want to monitor
	HANDLE hEvents[2];
	hEvents[0] = this->m_NotificationTermEvent;
	hEvents[1] = RegChangeEvent;		

	while (1)
	{
		// register the notification event
		if (Config.NotifyChangeKeyValue(TRUE, REG_NOTIFY_CHANGE_LAST_SET, RegChangeEvent) != ERROR_SUCCESS)
		{
			break;
		}		

		// wait for a change or an exit
		DWORD dwStatus = ::WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE);

		if (dwStatus == WAIT_OBJECT_0)
		{
			// it's an exit
			break;
		}
		else if (dwStatus == (WAIT_OBJECT_0 + 1))
		{
			// the registry has changed, update our settings
			Load();
		}
	}

	return 0;
}

bool IISxpressRegSettings::Load(void)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

	CRegKey Config;
	if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_READ | m_dwOpenFlags) != ERROR_SUCCESS)
	{
		return Save();
	}	

	// if we are running as 32bit then see if we need to use the 64bit registry
	if (sizeof(PVOID) == 4 && m_dwOpenFlags == 0)
	{
		DWORD dwUse64BitReg = 0;
		if (Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_REGKEY_USE64BITREG, dwUse64BitReg) == ERROR_SUCCESS && dwUse64BitReg != 0)
		{
			m_dwOpenFlags = KEY_WOW64_64KEY;

			Config.Close();

			return Load();
		}	
	}

	int nFails = 0;

	// get the enabled value
	DWORD dwEnabled = 1;
	if (Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_REGKEY_ENABLED, dwEnabled) != ERROR_SUCCESS)
	{
		nFails++;
	}		

	::InterlockedExchange((LONG*) &m_dwEnabled, dwEnabled);

	// get the logging level
	DWORD dwLoggingLevel = IISxpressRegKeys::IISXPRESS_LOGGINGLEVEL_NONE;
	if (Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_REGKEY_LOGGINGLEVEL, dwLoggingLevel) != ERROR_SUCCESS)
	{
		nFails++;
	}

	::InterlockedExchange((LONG*) &m_dwLoggingLevel, dwLoggingLevel);

	// get the debug value
	DWORD dwDebugEnabled = 0;
	Config.QueryDWORDValue(IISxpressRegKeys::IISXPRESS_REGKEY_DEBUGENABLED, dwDebugEnabled);

	::InterlockedExchange((LONG*) &m_dwDebugEnabled, dwDebugEnabled);

	TCHAR installPath[1024] = _T("\0");
	ULONG installPathSize = _countof(installPath);
	if (Config.QueryStringValue(IISxpressRegKeys::IISXPRESS_REGKEY_INSTALLPATH, installPath, &installPathSize) == ERROR_SUCCESS)
	{
		m_sInstallPath = installPath;
	}
	else
	{
		m_sInstallPath.Empty();
		nFails++;
	}

	if (nFails == 0)
		m_bDirty = false;	

	if (nFails > 0)
		Save();
	
	::InterlockedExchange((LONG*) &m_dwLoadCookie, MakeLoadCookie());

	return true;
}

bool IISxpressRegSettings::Save(void)
{
	bool bStatus = false;

	try
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		CRegKey Config;
		if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE | m_dwOpenFlags) != ERROR_SUCCESS)
		{
			if (Config.Create(HKEY_LOCAL_MACHINE, m_sRegPath) != ERROR_SUCCESS)
			{
				bStatus = false;
				throw bStatus;
			}
		}

		Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_REGKEY_ENABLED, m_dwEnabled);		
		Config.SetDWORDValue(IISxpressRegKeys::IISXPRESS_REGKEY_LOGGINGLEVEL, m_dwLoggingLevel);	

		// don't write the debug value
		//Config.SetDWORDValue(IISXPRESS_REGKEY_DEBUGENABLED, m_dwDebugEnabled);

		m_bDirty = false;
		bStatus = true;
	}
	catch (...)
	{
	}	

	return bStatus;
}

bool IISxpressRegSettings::GetEnabled(void)
{	
	// we don't need the interlock since the Set will do it for us
	//DWORD dwEnabled = (DWORD) ::InterlockedExchangeAdd((LONG*) &m_dwEnabled, 0);
	//bool bEnabled = (dwEnabled != 0);	
	bool bEnabled = (m_dwEnabled != 0);	

	return bEnabled;
}

void IISxpressRegSettings::SetEnabled(bool bEnabled, bool bSave)
{
	DWORD dwEnabled = (bEnabled == true ? 1 : 0);
	::InterlockedExchange((LONG*) &m_dwEnabled, (LONG) dwEnabled);

	if (bSave == false)	
	{
		m_bDirty = true;
	}	

	if (bSave == true)
	{
		Save();
	}
}

bool IISxpressRegSettings::GetDebugEnabled(void)
{
	// we don't need the interlock since the Set will do it for us
	//DWORD dwDebugEnabled = (DWORD) ::InterlockedExchangeAdd((LONG*) &m_dwDebugEnabled, 0);
	//bool bEnabled = (dwDebugEnabled != 0);	
	bool bEnabled = (m_dwDebugEnabled != 0);	

	return bEnabled;
}

/*void IISxpressRegSettings::SetDebugEnabled(bool bEnabled, bool bSave)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

	m_dwDebugEnabled = (bEnabled == true ? 1 : 0);

	if (bSave == false)	
	{
		m_bDirty = true;
	}	

	if (bSave == true)
	{
		Save();
	}
}*/

DWORD IISxpressRegSettings::GetLoggingLevel(void)
{	
	// we don't need the interlock since the Set will do it for us
	//DWORD dwLoggingLevel = (DWORD) ::InterlockedExchangeAdd((LONG*) &m_dwLoggingLevel, 0);
	DWORD dwLoggingLevel = m_dwLoggingLevel;

	return dwLoggingLevel;
}

bool IISxpressRegSettings::SetLoggingLevel(DWORD dwLevel, bool bSave)
{
	::InterlockedExchange((LONG*) &m_dwLoggingLevel, (LONG) dwLevel);

	if (bSave == false)	
	{
		m_bDirty = true;
	}

	if (bSave == true)
	{
		Save();
	}

	return true;
}

DWORD IISxpressRegSettings::MakeLoadCookie()
{
	LARGE_INTEGER nLoadCookie = { 0, 0 };
	::QueryPerformanceCounter(&nLoadCookie);
	return nLoadCookie.LowPart;
}

}}