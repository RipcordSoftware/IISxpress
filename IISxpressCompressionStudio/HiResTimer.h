#pragma once

class HiResTimer
{
public:
	HiResTimer(void);	

	__int64 GetTimerCount();
	__int64 GetMicroSecTimerCount();

	static __int64 GetTicks();
	static __int64 GetFrequency();

private:
	__int64	m_nTimerFreq;
};
