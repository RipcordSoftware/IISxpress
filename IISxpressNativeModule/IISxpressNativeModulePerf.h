#ifndef _IISxpressNativeModuleperf_H
#define _IISxpressNativeModuleperf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <perflib.h>
#include <winperf.h>

#ifdef __INIT_IISxpressNativeModuleperf_IMP
#define PROVIDER_GUID_IISxpressNativeModuleperf_1 { 0x4E821C6E, 0x9B92, 0x423D, 0xA5, 0xED, 0x7F, 0x37, 0xCA, 0x82, 0x1C, 0xC8 }
GUID ProviderGuid_IISxpressNativeModuleperf_1 = PROVIDER_GUID_IISxpressNativeModuleperf_1;
#else
extern GUID ProviderGuid_IISxpressNativeModuleperf_1;
#endif

typedef struct _CTRSET_IISxpressNativeModuleperf_1_1 {
    PERF_COUNTERSET_INFO CtSet_IISxpressNativeModuleperf_1_1;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_1;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_2;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_3;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_4;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_5;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_6;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_7;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_8;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_9;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_10;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_11;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_12;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_13;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_14;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_15;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_16;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_17;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_18;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_19;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_20;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_21;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_22;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_23;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_24;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_25;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_26;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_27;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_28;
    PERF_COUNTER_INFO    Ctr_IISxpressNativeModuleperf_1_1_29;
} CTRSET_IISxpressNativeModuleperf_1_1, * PCTRSET_IISxpressNativeModuleperf_1_1;

#ifdef __INIT_IISxpressNativeModuleperf_IMP
#define CTRSET_GUID_IISxpressNativeModuleperf_1_1 { 0xE3D5CC4A, 0xB0EE, 0x44D4, 0xAC, 0x78, 0xFD, 0x9D, 0x1C, 0xDB, 0xD8, 0x49 }
GUID CtrSetGuid_IISxpressNativeModuleperf_1_1 = CTRSET_GUID_IISxpressNativeModuleperf_1_1;
CTRSET_IISxpressNativeModuleperf_1_1 CtrSet_IISxpressNativeModuleperf_1_1 = {
    { CTRSET_GUID_IISxpressNativeModuleperf_1_1, PROVIDER_GUID_IISxpressNativeModuleperf_1, 29, PERF_COUNTERSET_MULTI_INSTANCES },
    { 1, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 3, 0 },
    { 2, PERF_COUNTER_COUNTER, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 8 },
    { 3, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 3, 16 },
    { 4, PERF_COUNTER_COUNTER, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 24 },
    { 5, PERF_COUNTER_LARGE_RAWCOUNT, 0, sizeof(ULONGLONG), PERF_DETAIL_NOVICE, 3, 32 },
    { 6, PERF_COUNTER_BULK_COUNT, 0, sizeof(ULONGLONG), PERF_DETAIL_NOVICE, 3, 40 },
    { 7, PERF_COUNTER_LARGE_RAWCOUNT, 0, sizeof(ULONGLONG), PERF_DETAIL_NOVICE, 3, 48 },
    { 8, PERF_COUNTER_BULK_COUNT, 0, sizeof(ULONGLONG), PERF_DETAIL_NOVICE, 3, 56 },
    { 9, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 3, 64 },
    { 10, PERF_COUNTER_COUNTER, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 72 },
    { 11, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 3, 80 },
    { 12, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 88 },
    { 13, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 96 },
    { 14, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 104 },
    { 15, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 112 },
    { 16, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 120 },
    { 17, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 128 },
    { 18, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 136 },
    { 19, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 144 },
    { 20, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 152 },
    { 21, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 160 },
    { 22, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 168 },
    { 23, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 176 },
    { 24, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 184 },
    { 25, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 192 },
    { 26, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 200 },
    { 27, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 208 },
    { 28, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 216 },
    { 29, PERF_COUNTER_RAWCOUNT, 0, sizeof(DWORD), PERF_DETAIL_NOVICE, 0, 224 }
};

ULONG dwCtrSet_IISxpressNativeModuleperf_1_1 = sizeof(CTRSET_IISxpressNativeModuleperf_1_1);
#else
extern GUID CtrSetGuid_IISxpressNativeModuleperf_1_1;
extern CTRSET_IISxpressNativeModuleperf_1_1 CtrSet_IISxpressNativeModuleperf_1_1;
extern ULONG dwCtrSet_IISxpressNativeModuleperf_1_1;
#endif

#ifdef __INIT_IISxpressNativeModuleperf_IMP
HANDLE hDataSource_IISxpressNativeModuleperf_1 = NULL;
#else
extern HANDLE hDataSource_IISxpressNativeModuleperf_1;
#endif


ULONG PerfAutoInitialize(void);
ULONG PerfAutoCleanup(void);

#ifdef __cplusplus
}
#endif

#endif // _IISxpressNativeModuleperf_H

