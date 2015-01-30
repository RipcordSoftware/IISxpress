#include "StdAfx.h"
#include "Interlocked64.h"

#include "AutoMemberCriticalSection.h"

Interlocked64::Interlocked64(void) : m_hKernel(NULL), m_pfnInterlockedCompareExchange64(NULL)
{
	m_hKernel = ::LoadLibrary(_T("kernel32.dll"));
	if (m_hKernel != NULL)
	{
		m_pfnInterlockedCompareExchange64 = (FNINTERLOCKEDCOMPAREEXCHANGE64*) ::GetProcAddress(m_hKernel, "InterlockedCompareExchange64");
	}
}

Interlocked64::~Interlocked64(void)
{
	if (m_hKernel != NULL)
	{
		m_pfnInterlockedCompareExchange64 = NULL;
		::FreeLibrary(m_hKernel);
	}
}

__int64 Interlocked64::InterlockedAdd64(__int64 volatile* pnAddEnd, __int64 nValue)
{	
	if (m_pfnInterlockedCompareExchange64 != NULL)
	{
		// this logic comes from the InterlockedExchangeAdd64() macro in winbase.h
		__int64 old;
		do
		{
			old = *pnAddEnd;
		}
		while (m_pfnInterlockedCompareExchange64(pnAddEnd, old + nValue, old) != old);

		return old;
	}
	else
	{
		AutoMemberCriticalSection cs(&m_csInterlockedAdd64);		
		__int64 old = *pnAddEnd;
		*pnAddEnd += nValue;
		return old;
	}
}