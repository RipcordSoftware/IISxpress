#pragma once

class AutoMemberCriticalSection
{

public:

	AutoMemberCriticalSection(CComAutoCriticalSection* pcs)
	{
		m_pcs = NULL;

		Lock(pcs);
	}

	~AutoMemberCriticalSection(void)
	{
		Unlock();
	}

	void Lock(CComAutoCriticalSection* pcs)
	{
		if (pcs != NULL)
		{
			m_pcs = pcs;
			m_pcs->Lock();
		}
	}

	void Unlock(void)
	{
		if (m_pcs != NULL)
		{
			m_pcs->Unlock();
			m_pcs = NULL;
		}
	}	

private:

	CComAutoCriticalSection* m_pcs;

};