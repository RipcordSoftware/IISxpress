#pragma once

// sets the current thread's affinity
class CSetThreadAffinity
{

public:

	//CSetThreadAffinity() : m_hThread(NULL), m_dwOldMask(0) {}

	CSetThreadAffinity(DWORD_PTR dwCPUMask) : m_hThread(NULL), m_dwOldMask(0)
	{		
		if (dwCPUMask != 0)
		{
			m_hThread = ::GetCurrentThread();
			m_dwOldMask = ::SetThreadAffinityMask(m_hThread, dwCPUMask);	
		}
	}

	~CSetThreadAffinity(void)
	{
		if (m_hThread != NULL)
		{
			::SetThreadAffinityMask(m_hThread, m_dwOldMask);
		}
	}

	//void operator =(CSetThreadAffinity& other) 
	//{ 
	//	ATLASSERT(m_hThread == NULL);			

	//	m_hThread = other.m_hThread; 
	//	m_dwOldMask = other.m_dwOldMask; 
	//	other.m_hThread = NULL;
	//	other.m_dwOldMask = 0;
	//}

private:

	HANDLE		m_hThread;
	DWORD_PTR	m_dwOldMask;
};