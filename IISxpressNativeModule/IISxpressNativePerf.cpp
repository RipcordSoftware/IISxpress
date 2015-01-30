#include "StdAfx.h"
#include "IISxpressNativePerf.h"

#include "IISxpressNativeModulePerf.h"

volatile DWORD IISxpressNativePerf::m_dwInstanceCounter = 0;

// the count of the number of times PerfInitialize\PerfCleanup has been called
volatile LONG IISxpressNativePerf::m_nInitializeRefCount = 0;

IISxpressNativePerf::IISxpressNativePerf() : m_pCounterSetInstance(NULL)
{
	PerfInitialize();

	if (IsValid())
	{
		DWORD instance = ++m_dwInstanceCounter;

		m_pCounterSetInstance = PerfQueryInstance(hDataSource_IISxpressNativeModuleperf_1, &CtrSetGuid_IISxpressNativeModuleperf_1_1, L"_Total", instance);

		if (m_pCounterSetInstance == NULL)
		{
			m_pCounterSetInstance = PerfCreateInstance(hDataSource_IISxpressNativeModuleperf_1, &CtrSetGuid_IISxpressNativeModuleperf_1_1, L"_Total", instance);
		}
	}
}

IISxpressNativePerf::IISxpressNativePerf(LPCWSTR pszInstanceName) : m_pCounterSetInstance(NULL)
{
	PerfInitialize();

	if (IsValid())
	{
		m_pCounterSetInstance = PerfCreateInstance(hDataSource_IISxpressNativeModuleperf_1, &CtrSetGuid_IISxpressNativeModuleperf_1_1, pszInstanceName, ++m_dwInstanceCounter);
	}
}

IISxpressNativePerf::~IISxpressNativePerf(void)
{
	if (IsValid())
	{
		if (m_pCounterSetInstance != NULL)
		{
			PerfDeleteInstance(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance);
		}

		PerfCleanup();
	}	
}

bool IISxpressNativePerf::IsValid()
{
	return hDataSource_IISxpressNativeModuleperf_1 != NULL;
}

void IISxpressNativePerf::IncrementCompressedResponses()
{
	if (IsValid())
	{
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 1, 1);
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 2, 1);
	}
}

void IISxpressNativePerf::IncrementUncompressedResponses()
{
	if (IsValid())
	{
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 3, 1);		
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 4, 1);		
	}
}

void IISxpressNativePerf::IncrementCacheHits()
{
	if (IsValid())
	{
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 9, 1);
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 10, 1);
	}
}

void IISxpressNativePerf::IncrementRawBytes(DWORD dwBytes)
{
	if (IsValid())
	{
		PerfIncrementULongLongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 5, dwBytes);
		PerfIncrementULongLongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 6, dwBytes);
	}
}

void IISxpressNativePerf::IncrementCompressedBytes(DWORD dwBytes)
{
	if (IsValid())
	{
		PerfIncrementULongLongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 7, dwBytes);
		PerfIncrementULongLongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 8, dwBytes);
	}
}

void IISxpressNativePerf::SetCacheSizeAndItems(DWORD dwCacheSize, DWORD dwCacheItems)
{
	if (IsValid())
	{
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 11, dwCacheSize);
		PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 12, dwCacheItems);
	}
}

void IISxpressNativePerf::IncrementUnhandledCount(RejectedResponseReasons reason)
{
	if (IsValid())
	{
		ULONG counterId = 0;

		switch (reason)
		{
			case Default:					counterId = 13; break;
			case Internal:					counterId = 13; break;
			case FilterDisabled:			counterId = 14; break;
			case MemoryAllocationFailed:	counterId = 15; break;
			case ResponseCode:				counterId = 16; break;
			case LocalhostDisabled:			counterId = 17; break;
			case MissingContentType:		counterId = 18; break;
			case InvalidContentLength:		counterId = 19; break;
			case IncompatibleClient:		counterId = 20; break;
			case AlreadyEncoded:			counterId = 21; break;
			case RequestMethod:				counterId = 22; break;
			case NoCompressionServer:		counterId = 23; break;
			case CompressionServerDisabled:	counterId = 24; break;
			case NeverRuleMatch:			counterId = 25; break;
			case StaticRuleMatch:			counterId = 26; break;
			case CPUBusy:					counterId = 27; break;
			case MemoryLow:					counterId = 28; break;
			case UserRuleExcluded:			counterId = 29; break;
			default:						counterId = 13; break;
		}
		
		if (counterId > 0)
		{
			PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, counterId, 1);
			PerfIncrementULongCounterValue(hDataSource_IISxpressNativeModuleperf_1, m_pCounterSetInstance, 3, 1);
		}
	}
}

void IISxpressNativePerf::PerfInitialize()
{
	LONG refCount = ::InterlockedIncrement(&m_nInitializeRefCount);
	if (refCount == 1)
	{
		::PerfAutoInitialize();
	}
}

void IISxpressNativePerf::PerfCleanup()
{
	LONG refCount = ::InterlockedDecrement(&m_nInitializeRefCount);
	if (refCount == 0)
	{
		::PerfAutoCleanup();
	}
}