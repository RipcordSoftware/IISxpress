#pragma once

#include "AHA363Compressor.h"

extern "C" __declspec(dllexport) AHA::CAHA363* CreateAHACompressor(int board, ULONG DmaChannel, ULONG DmaBlockSize, AHA::STRM_FLAGS StrmFlags, AHA::SGL_FLAGS SglFlags, BOOL ZLib);
extern "C" __declspec(dllexport) void FreeAHACompressor(AHA::CAHA363* pCompressor);

typedef AHA::CAHA363* (*PFNCREATEAHACOMPRESSOR)(int board, ULONG DmaChannel, ULONG DmaBlockSize, AHA::STRM_FLAGS StrmFlags, AHA::SGL_FLAGS SglFlags, BOOL ZLib);
typedef void (*PFNFREEAHACOMPRESSOR)(AHA::CAHA363* pCompressor);