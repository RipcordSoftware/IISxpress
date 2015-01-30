#include "StdAfx.h"
#include "AHA363Connector.h"

CAHA363Connector::CAHA363Connector(void) : module(NULL), CreateCompressor(NULL), FreeCompressor(NULL)
{
	module = ::LoadLibrary(_T("IISxpressAHAComp.dll"));
	if (module != NULL)
	{
		CreateCompressor = (PFNCREATEAHACOMPRESSOR) ::GetProcAddress(module, "CreateAHACompressor");
		FreeCompressor = (PFNFREEAHACOMPRESSOR) ::GetProcAddress(module, "FreeAHACompressor");

		if (CreateCompressor == NULL || FreeCompressor == NULL)
		{
			CreateCompressor = NULL;
			FreeCompressor = NULL;

			::FreeLibrary(module);
			module = NULL;
		}
	}
}

CAHA363Connector::~CAHA363Connector(void)
{
	if (module != NULL)
	{
		::FreeLibrary(module);
		module = NULL;
	}
}

AHA::CAHA363* CAHA363Connector::CreateAHACompressor(int board, ULONG DmaChannel, ULONG DmaBlockSize, AHA::STRM_FLAGS StrmFlags, AHA::SGL_FLAGS SglFlags, BOOL ZLib)
{
	AHA::CAHA363* pCompressor = NULL;

	if (CreateCompressor != NULL)
	{
		pCompressor = CreateCompressor(board, DmaChannel, DmaBlockSize, StrmFlags, SglFlags, ZLib);
	}

	return pCompressor;
}

void CAHA363Connector::FreeAHACompressor(AHA::CAHA363* pCompressor)
{
	if (FreeCompressor != NULL && pCompressor != NULL)
	{
		FreeCompressor(pCompressor);
	}
}