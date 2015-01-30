#pragma once

#include <perflib.h>

class IISxpressNativePerf
{
public:

	enum RejectedResponseReasons
	{
		Default = 0,
		Internal,
		FilterDisabled,
		MemoryAllocationFailed,
		ResponseCode,
		LocalhostDisabled,
		MissingContentType,
		InvalidContentLength,
		IncompatibleClient,
		AlreadyEncoded,
		RequestMethod,
		NoCompressionServer,
		CompressionServerDisabled,
		NeverRuleMatch,
		StaticRuleMatch,
		CPUBusy,
		MemoryLow,
		UserRuleExcluded
	};

	IISxpressNativePerf();
	IISxpressNativePerf(LPCWSTR pszInstanceName);
	~IISxpressNativePerf(void);

	void IncrementCompressedResponses();
	void IncrementUncompressedResponses();
	void IncrementCacheHits();

	void IncrementRawBytes(DWORD dwBytes);
	void IncrementCompressedBytes(DWORD dwBytes);

	void SetCacheSizeAndItems(DWORD dwCacheSize, DWORD dwCacheItems);

	void IncrementUnhandledCount(RejectedResponseReasons reason);

private:

	static volatile LONG m_nInitializeRefCount;
	void PerfInitialize();
	void PerfCleanup();

	bool IsValid();

	PPERF_COUNTERSET_INSTANCE m_pCounterSetInstance;

	static volatile DWORD m_dwInstanceCounter;
};
