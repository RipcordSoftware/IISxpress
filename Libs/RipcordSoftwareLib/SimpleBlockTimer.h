#pragma once

#include "HiResTimer.h"

namespace Ripcord { namespace Utils
{

class RIPCORDSOFTWARELIB_DLL SimpleBlockTimer
{

public:

	SimpleBlockTimer(void)
	{
		m_nStart = 0;
		m_nEnd = 0;
		m_nInterval = 0;
	}

	void Start(void)
	{
		m_nStart = m_Timer.GetMicroSecTimerCount();
	}

	__int64 End(void)
	{
		m_nEnd = m_Timer.GetMicroSecTimerCount();
		m_nInterval = m_nEnd - m_nStart;
		return m_nInterval;
	}

	void OutputIntervalToDebugWindow(LPCTSTR pszPrefix)
	{
		TCHAR szBuffer[512];
		wsprintf(szBuffer, _T("%s %I64d us\n"), pszPrefix != NULL ? pszPrefix : _T(""), m_nInterval);

		::OutputDebugString(szBuffer);
	}

private:

	HiResTimer m_Timer;

	__int64		m_nStart;	
	__int64		m_nEnd;
	__int64		m_nInterval;
};

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

class RIPCORDSOFTWARELIB_DLL AutoBlockTimer
{

public:

	AutoBlockTimer(LPCTSTR pszName)
	{
		m_sName = pszName;
		m_Timer.Start();
	}

	~AutoBlockTimer(void)
	{
		m_Timer.End();
		m_Timer.OutputIntervalToDebugWindow(m_sName);
	}

private:

	CString				m_sName;
	SimpleBlockTimer	m_Timer;
};

}}