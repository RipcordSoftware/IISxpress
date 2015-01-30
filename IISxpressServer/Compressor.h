#pragma once

// for Win64 we need WINAPI specified (according to http://www.winimage.com/zLibDll/)
#ifdef _WIN64
#define ZLIB_WINAPI
#endif

#include "zlib.h"
#include "bzip2\bzlib.h"

#include "ICompressor.h"

// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

class CDeflateCompressor : public ICompressor
{

public:

	CDeflateCompressor(CompressorMode mode = Default);
	virtual ~CDeflateCompressor();

	// ICompressor members
	bool SetMode(CompressorMode);
	CompressorStatus Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize);
	void Reset();
	bool CanContinueOnOutBufferTooSmall() { return true; }

private:

	bool		m_bInitialized;
	z_stream	m_Stream;

};

// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

class CGZipCompressor : public ICompressor
{

public:

	CGZipCompressor(CompressorMode mode = Default);
	virtual ~CGZipCompressor();

	// ICompressor members
	bool SetMode(CompressorMode);
	CompressorStatus Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize);
	void Reset();
	bool CanContinueOnOutBufferTooSmall() { return true; }

private:

	bool		m_bInitialized;
	z_stream	m_Stream;

};

// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

class CBZip2Compressor : public ICompressor
{

public:

	CBZip2Compressor(CompressorMode mode = Default);
	virtual ~CBZip2Compressor();

	// ICompressor members
	bool SetMode(CompressorMode);
	CompressorStatus Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize);
	void Reset();
	bool CanContinueOnOutBufferTooSmall() { return true; }

private:

	bool		m_bInitialized;
	bz_stream	m_Stream;

	CompressorMode	m_Mode;

};