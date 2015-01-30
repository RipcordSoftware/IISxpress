#include "StdAfx.h"
#include "hirestimer.h"

HiResTimer::HiResTimer(void) : m_nTimerFreq(GetFrequency())
{
}

// return the count in ms
__int64 HiResTimer::GetTimerCount()
{	
	return (GetTicks() * 1000) / m_nTimerFreq;
}

// return the count in us
__int64 HiResTimer::GetMicroSecTimerCount()
{
	return (GetTicks() * 1000000) / m_nTimerFreq;
}

__int64 HiResTimer::GetTicks()
{
	LARGE_INTEGER liCount;
	::QueryPerformanceCounter(&liCount);
	return liCount.QuadPart;
}

__int64 HiResTimer::GetFrequency()
{
	LARGE_INTEGER liTimerFreq;
	::QueryPerformanceFrequency(&liTimerFreq);	
	return liTimerFreq.QuadPart;	
}