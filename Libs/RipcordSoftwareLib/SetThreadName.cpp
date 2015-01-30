#include "stdafx.h"

#include "SetThreadName.h"

namespace Ripcord { namespace Utils
{

const DWORD MS_VC_EXCEPTION = 0x406D1388;

typedef struct
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;

void Threading::SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = szThreadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info );
   }
   __except(EXCEPTION_CONTINUE_EXECUTION)
   {
   }
}

}}