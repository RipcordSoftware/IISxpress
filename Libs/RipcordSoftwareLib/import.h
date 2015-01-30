#pragma once

#ifndef RIPCORDSOFTWARELIB_DLL
#define RIPCORDSOFTWARELIB_DLL __declspec(dllimport)
#endif

#include "tstring.h"
#include "AppLog.h"
#include "HiResTimer.h"
#include "SetThreadName.h"
#include "Interlocked64.h"
#include "SimpleBlockTimer.h"
#include "AutoArray.h"
#include "AutoHandle.h"

#include "IISMetabase.h"

#include "ISAPIFilter.h"