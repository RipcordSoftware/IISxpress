// IISxpressISAPI.h: interface for the CIISxpressISAPI class.
//


#pragma once

#include "myatlperf.h"

class __declspec(dllexport) CIISxpressISAPIPerfObject : public MYATL::CPerfObject
{
private:

	CIISxpressISAPIPerfObject() : instanceCount(0) {}

public:
	DECLARE_PERF_CATEGORY(CIISxpressISAPIPerfObject, 1, _T("IISxpress ISAPI"), _T("IISxpress ISAPI Performance Counters"), -1);
	BEGIN_COUNTER_MAP(CIISxpressISAPIPerfObject)

		DEFINE_COUNTER(totalCompressedResponses, _T("Compressed Responses"), _T("The total number of compressed responses handled by the IISxpress ISAPI filter"), PERF_COUNTER_RAWCOUNT, -3)
		DEFINE_COUNTER(totalCompressedResponses, _T("Compressed Responses/sec"), _T("The number of compressed responses handled per second by the IISxpress ISAPI filter"), PERF_COUNTER_COUNTER, 0)

		DEFINE_COUNTER(totalRejectedResponses, _T("Unhandled Responses"), _T("The total number of responses rejected by the IISxpress ISAPI filter"), PERF_COUNTER_RAWCOUNT, -3)
		DEFINE_COUNTER(totalRejectedResponses, _T("Unhandled Responses/sec"), _T("The number of responses rejected per second by the IISxpress ISAPI filter"), PERF_COUNTER_COUNTER, 0)

		DEFINE_COUNTER(totalRawResponseSize, _T("Total Size of Raw Responses in Bytes"), _T("The total number of raw uncompressed bytes processed by the IISxpress ISAPI filter"), PERF_COUNTER_LARGE_RAWCOUNT, -3)
		DEFINE_COUNTER(totalRawResponseSize, _T("Raw Responses Bytes/sec"), _T("The number of raw uncompressed bytes processed per second by the IISxpress ISAPI filter"), PERF_COUNTER_BULK_COUNT, 0)

		DEFINE_COUNTER(totalCompressedResponseSize, _T("Total Size of Compressed Responses in Bytes"), _T("The total number of compressed bytes returned by the IISxpress ISAPI filter"), PERF_COUNTER_LARGE_RAWCOUNT, -3)
		DEFINE_COUNTER(totalCompressedResponseSize, _T("Compressed Responses Bytes/sec"), _T("The number of compressed bytes returned per second by the IISxpress ISAPI filter"), PERF_COUNTER_BULK_COUNT, 0)

		DEFINE_COUNTER(totalCacheHits, _T("Cache Hits"), _T("The total number of compressed response cache hits by the IISxpress ISAPI filter"), PERF_COUNTER_RAWCOUNT, -3)
		DEFINE_COUNTER(totalCacheHits, _T("Cache Hits/sec"), _T("The number of compressed response cache hits per second by the IISxpress ISAPI filter"), PERF_COUNTER_COUNTER, 0)
		DEFINE_COUNTER(totalCacheSize, _T("Cache Size in Bytes"), _T("The total size of the compression cache in bytes"), PERF_COUNTER_RAWCOUNT, -3)
		DEFINE_COUNTER(totalCacheItems, _T("Cache Items"), _T("The total number of items held by the compression cache"), PERF_COUNTER_RAWCOUNT, 0)

		DEFINE_COUNTER(totalRejectedResponses_Internal, _T("Unhandled Responses - Internal Error"), _T("The number of responses that were not handled due to an internal error"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_FilterDisabled, _T("Unhandled Responses - Filter Disabled"), _T("The number of responses that were not handled because the IISxpress ISAPI filter is disabled"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_MemoryAllocationFailed, _T("Unhandled Responses - Memory Allocation"), _T("The number of responses that were not handled because the system failed to allocate memory"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_ResponseCode, _T("Unhandled Responses - Response Code"), _T("The number of responses that were not handled because the HTTP response code is not supported"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_LocalhostDisabled, _T("Unhandled Responses - Localhost Disabled"), _T("The number of responses that were not handled because localhost requests are not being handled by IISxpress"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_MissingContentType, _T("Unhandled Responses - Missing Content Type"), _T("The number of responses that were not handled because they did not have a valid HTTP content type"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_InvalidContentLength, _T("Unhandled Responses - Invalid Content Length"), _T("The number of responses that were not handled because they did not have a valid HTTP content length"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_IncompatibleClient, _T("Unhandled Responses - Incompatible Client"), _T("The number of responses that were not handled because the client did not support HTTP compression (gzip or deflate)"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_AlreadyEncoded, _T("Unhandled Responses - Already Encoded"), _T("The number of responses that were not handled because IIS or another ISAPI filter had already encoded the response"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_RequestMethod, _T("Unhandled Responses - Request Method"), _T("The number of responses that were not handled because the HTTP request method is not supported (see RFC 2616)"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_NoCompressionServer, _T("Unhandled Responses - Compression Server Offline"), _T("The number of responses that were not handled because the IISxpress Compression Service is stopped or not available"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_CompressionServerDisabled, _T("Unhandled Responses - Compression Server Disabled"), _T("The number of responses that were not handled because the IISxpress Compression Service was disabled"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_NeverRuleMatch, _T("Unhandled Responses - Matched a User Exclusion Rule"), _T("The number of responses that were not handled because a user specified exclusion rule was matched"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_StaticRuleMatch, _T("Unhandled Responses - Matched a System Exclusion Rule"), _T("The number of responses that were not handled because a system generated exclusion rule was matched"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_CPUBusy, _T("Unhandled Responses - CPU Busy"), _T("The number of responses that were not handled because the CPU load theshold was exceeded"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_MemoryLow, _T("Unhandled Responses - Memory Low"), _T("The number of responses that were not handled because the memory usage theshold was exceeded"), PERF_COUNTER_RAWCOUNT, 0)
		DEFINE_COUNTER(totalRejectedResponses_UserRuleExcluded, _T("Unhandled Responses - Explicitly Excluded"), _T("The number of responses that were not handled because of an explicit (URI or file) exclusion rule"), PERF_COUNTER_RAWCOUNT, 0)

	END_COUNTER_MAP()

	DWORD totalCompressedResponses;
	DWORD totalRejectedResponses;
	__int64 totalRawResponseSize;
	__int64 totalCompressedResponseSize;

	DWORD totalCacheHits;
	DWORD totalCacheSize;
	DWORD totalCacheItems;

	DWORD totalRejectedResponses_Internal;
	DWORD totalRejectedResponses_FilterDisabled;
	DWORD totalRejectedResponses_MemoryAllocationFailed;
	DWORD totalRejectedResponses_ResponseCode;
	DWORD totalRejectedResponses_LocalhostDisabled;
	DWORD totalRejectedResponses_MissingContentType;
	DWORD totalRejectedResponses_InvalidContentLength;
	DWORD totalRejectedResponses_IncompatibleClient;
	DWORD totalRejectedResponses_AlreadyEncoded;
	DWORD totalRejectedResponses_RequestMethod;
	DWORD totalRejectedResponses_NoCompressionServer;
	DWORD totalRejectedResponses_CompressionServerDisabled;
	DWORD totalRejectedResponses_NeverRuleMatch;
	DWORD totalRejectedResponses_StaticRuleMatch;
	DWORD totalRejectedResponses_CPUBusy;
	DWORD totalRejectedResponses_MemoryLow;
	DWORD totalRejectedResponses_UserRuleExcluded;

	volatile LONG instanceCount;

public:

	static CIISxpressISAPIPerfObject* Create(LPCSTR pszInstanceName);
	static void Release(CIISxpressISAPIPerfObject* pInstance);
};

// TODO:  First add Performance Monitor objects to your project.
// You can then add counters to those objects.  Use the context
// menu of CIISxpressISAPI in ClassView to add a PerfMon Object
// to your project.  Then use the context menu of the newly
// created PerfMon Object to add a PerfMon Counter.
class CIISxpressISAPIPerf : public MYATL::CPerfMon
{
public:
#define Perf_IISxpressISAPI _T("Perf_IISxpressISAPI")
	BEGIN_PERF_MAP(Perf_IISxpressISAPI)
		CHAIN_PERF_CATEGORY(CIISxpressISAPIPerfObject)
	END_PERF_MAP()
};

PERFREG_ENTRY(CIISxpressISAPIPerf);
