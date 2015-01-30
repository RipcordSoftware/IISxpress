#pragma once

#include "HiResTimer.h"

class CSimpleBlockTimer
{

public:

	CSimpleBlockTimer(void)
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

	CHiResTimer m_Timer;

	__int64		m_nStart;	
	__int64		m_nEnd;
	__int64		m_nInterval;
};

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

class CAutoBlockTimer
{

public:

	CAutoBlockTimer(LPCTSTR pszName)
	{
		m_sName = pszName;
		m_Timer.Start();
	}

	~CAutoBlockTimer(void)
	{
		m_Timer.End();
		m_Timer.OutputIntervalToDebugWindow(m_sName);
	}

private:

	CString				m_sName;
	CSimpleBlockTimer	m_Timer;
};