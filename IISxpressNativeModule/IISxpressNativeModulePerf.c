//
// Common function implementation generated by CTRPP tool.
//

#define __INIT_IISxpressNativeModuleperf_IMP
#include "IISxpressNativeModuleperf.h"

ULONG
ControlCallback_IISxpressNativeModuleperf_1(
    IN ULONG   RequestCode,
    IN PVOID   pBuffer,
    IN ULONG   BufferSize
)
{
    ULONG Status = ERROR_SUCCESS;

    // The customized callback will be called when consumer/controller
    // requests counters that the provider supports. Buffer will be an
    // input buffer that carries additional information. There will be no
    // output buffer as PERFLIB V2 will prepare all response.
    //
    // A provider does not need to have ControlCallback() implemented
    // (thus passing NULL in RegisterTraceGuids() call). PERFLIB V2
    // infrastructure will handle default actions. Implementing customized
    // notification callback() gives provider chances to perform
    // tasks beforem PERFLIB V2 infrastructure performance the requests
    // from consumers, and block PERFLIB V2 action by returning error (
    // that is, not return ERROR_SUCCESS).

    switch (RequestCode) {
    case PERF_ADD_COUNTER:
        // This will be called when one consumer adds one counter/datapoint
        // to the query.
        //
        break;

    case PERF_REMOVE_COUNTER:
        // This will be called when one consumer remove counter/datapoint
        // from the query.
        //
        break;

    case PERF_ENUM_INSTANCES:
        // This will be called when consumer requests to enumerate all
        // instances for specified dataset, which is identified by
        // CounterSetGuid.
        //
        break;

    case PERF_COLLECT_START:
        // This will be called when PERFLIB V2 infrastructure is about to
        // collect counter data from the provider.
        //
        break;

    case PERF_COLLECT_END:
        // This will be called when PERFLIB V2 infrastructure just finishes
        // collecting counter data from the provider.
        // Usually PERF_COLLECT_START/PERF_COLLECT_END will come as a pair.
        //
        break;

    case PERF_FILTER:
        // This will be called when consumer/controller applications send
        // enable/disable/control/filter notification to provider.
        // The syntax/semantics of filter is still under discussion.
        //
        break;

    default:
        Status = ERROR_INVALID_PARAMETER;
        break;
    }
    return Status;
}

ULONG
PerfAutoInitialize(
    void
)
{
    ULONG Status = ERROR_SUCCESS;

    Status = PerfStartProvider(
            (LPGUID) & ProviderGuid_IISxpressNativeModuleperf_1,
            (PERFLIBREQUEST) ControlCallback_IISxpressNativeModuleperf_1,
            & hDataSource_IISxpressNativeModuleperf_1);
    if (Status != ERROR_SUCCESS) {
        goto Cleanup;
    }

    Status = PerfSetCounterSetInfo(
            hDataSource_IISxpressNativeModuleperf_1,
            (PPERF_COUNTERSET_INFO) & CtrSet_IISxpressNativeModuleperf_1_1,
            dwCtrSet_IISxpressNativeModuleperf_1_1);
    if (Status != ERROR_SUCCESS) {
        goto Cleanup;
    }

Cleanup:
    if (Status != ERROR_SUCCESS) {
        PerfStopProvider(hDataSource_IISxpressNativeModuleperf_1);
    }
    return Status;
}

ULONG
PerfAutoCleanup(
    void
)
{
    ULONG Status;

    Status = PerfStopProvider(hDataSource_IISxpressNativeModuleperf_1);
    return ERROR_SUCCESS;
}
