#pragma once

#include "..\IISxpressAHAComp\AHACompressorFactory.h"

class CAHA363Connector
{

public:

	CAHA363Connector(void);
	~CAHA363Connector(void);

	AHA::CAHA363* CreateAHACompressor(int board, ULONG DmaChannel, ULONG DmaBlockSize, AHA::STRM_FLAGS StrmFlags, AHA::SGL_FLAGS SglFlags, BOOL ZLib);
	void FreeAHACompressor(AHA::CAHA363* pCompressor);

private:

	HMODULE module;
	PFNCREATEAHACOMPRESSOR CreateCompressor;
	PFNFREEAHACOMPRESSOR FreeCompressor;
};
