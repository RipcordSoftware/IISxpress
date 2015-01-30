#pragma once

namespace Ripcord { namespace Utils
{

class RIPCORDSOFTWARELIB_DLL Interlocked64
{
	// See winbase.h for these decls
#if defined(_M_AMD64) 
	typedef LONG64 FNINTERLOCKEDCOMPAREEXCHANGE64(LONG64 volatile* Destination, LONG64 Exchange, LONG64 Comperand);
#else
	typedef LONGLONG WINAPI FNINTERLOCKEDCOMPAREEXCHANGE64(LONGLONG volatile* Destination, LONGLONG Exchange, LONGLONG Comperand);
#endif

public:
	Interlocked64(void);
	~Interlocked64(void);

	__int64 InterlockedAdd64(__int64 volatile* pnAddEnd, __int64 nValue);

private:

	HMODULE									m_hKernel;
	FNINTERLOCKEDCOMPAREEXCHANGE64*			m_pfnInterlockedCompareExchange64;

	CComAutoCriticalSection					m_csInterlockedAdd64;
};

}}