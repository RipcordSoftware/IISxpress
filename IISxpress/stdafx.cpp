// stdafx.cpp : source file that includes just the standard includes
// IISxpress.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

const CLSID CLSID_IISxpressHTTPRequest = { 0xE9D82120,0x347A,0x41BD,0xBF,0x61,0x1B,0xD6,0xBA,0x23,0xED,0x50 };
const IID IID_IIISxpressHTTPRequest = { 0x86BEC801,0x0653,0x42BE,0xA2,0x0A,0x84,0xD7,0x1D,0xEE,0xCE,0x51 };

Ripcord::Utils::HiResTimer g_Timer;

// object which can InterlockAdd64
Ripcord::Utils::Interlocked64 g_interlock;