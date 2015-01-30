#include "stdafx.h"

#include "AHACompressorFactory.h"

__declspec(dllexport) AHA::CAHA363* CreateAHACompressor(int board, ULONG DmaChannel, ULONG DmaBlockSize, AHA::STRM_FLAGS StrmFlags, AHA::SGL_FLAGS SglFlags, BOOL ZLib)
{
	AHA::CAHA363* pCompressor = new AHA::CAHA363();
	if (pCompressor != NULL)
	{		
		if (pCompressor->Open(board, DmaChannel, DmaBlockSize, StrmFlags, SglFlags, ZLib) != ERROR_SUCCESS)
		{
			delete pCompressor;
			pCompressor = NULL;
		}
	}

	return pCompressor;
}

__declspec(dllexport) void FreeAHACompressor(AHA::CAHA363* pCompressor)
{
	delete pCompressor;
}