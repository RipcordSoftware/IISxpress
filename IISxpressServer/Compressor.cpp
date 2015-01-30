#include "stdafx.h"

#include "Compressor.h"

CDeflateCompressor::CDeflateCompressor(CompressorMode mode) : m_bInitialized(false)
{
	SetMode(mode);
}

CDeflateCompressor::~CDeflateCompressor()
{
	if (m_bInitialized)
	{
		::deflateEnd(&m_Stream);
		m_bInitialized = false;
	}
}
	
bool CDeflateCompressor::SetMode(CompressorMode mode)
{
	if (m_bInitialized)
	{
		::deflateEnd(&m_Stream);
		m_bInitialized = false;
	}

	int nLevel = Z_DEFAULT_COMPRESSION;
	switch (mode)
	{
	case None: nLevel = Z_NO_COMPRESSION;
	case Size: nLevel = Z_BEST_COMPRESSION; break;
	case Speed: nLevel = Z_BEST_SPEED; break;
	}

	memset(&m_Stream, 0, sizeof(m_Stream));
	m_bInitialized = ::deflateInit2(&m_Stream, nLevel, Z_DEFLATED, -15, 9, Z_DEFAULT_STRATEGY) == Z_OK ? true : false;	
	return m_bInitialized;
}

CompressorStatus CDeflateCompressor::Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize)
{
	if (!m_bInitialized || pbyDest == NULL || dwDestSize == 0)
	{
		return Error;
	}

	if (pbySource != NULL)
	{
		m_Stream.next_in = const_cast<BYTE*>(pbySource);
		m_Stream.avail_in = dwSourceSize;
	}

	if (m_Stream.next_in == NULL)
	{
		return Error;
	}

	m_Stream.next_out = pbyDest;
	m_Stream.avail_out = dwDestSize;

	int nCompressedStatus = ::deflate(&m_Stream, Z_FINISH);

	switch (nCompressedStatus)
	{
	case Z_STREAM_END: dwCompressedSize = m_Stream.total_out; return Finished;
	case Z_OK: return OutBufferTooSmall;
	default: return Error;
	}
}

void CDeflateCompressor::Reset()
{
	if (m_bInitialized)
	{
		::deflateReset(&m_Stream);
	}
}

// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

CGZipCompressor::CGZipCompressor(CompressorMode mode) : m_bInitialized(false)
{
	SetMode(mode);
}

CGZipCompressor::~CGZipCompressor()
{
	if (m_bInitialized)
	{
		::deflateEnd(&m_Stream);
		m_bInitialized = false;
	}
}
	
bool CGZipCompressor::SetMode(CompressorMode mode)
{
	if (m_bInitialized)
	{
		::deflateEnd(&m_Stream);
		m_bInitialized = false;
	}

	int nLevel = Z_DEFAULT_COMPRESSION;
	switch (mode)
	{
	case None: nLevel = Z_NO_COMPRESSION;
	case Size: nLevel = Z_BEST_COMPRESSION; break;
	case Speed: nLevel = Z_BEST_SPEED; break;
	}

	memset(&m_Stream, 0, sizeof(m_Stream));
	m_bInitialized = ::deflateInit2(&m_Stream, nLevel, Z_DEFLATED, 31, 9, Z_DEFAULT_STRATEGY) == Z_OK ? true : false;
	return m_bInitialized;
}

CompressorStatus CGZipCompressor::Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize)
{
	if (!m_bInitialized || pbyDest == NULL || dwDestSize == 0)
	{
		return Error;
	}

	if (pbySource != NULL)
	{
		m_Stream.next_in = const_cast<BYTE*>(pbySource);
		m_Stream.avail_in = dwSourceSize;
	}

	if (m_Stream.next_in == NULL)
	{
		return Error;
	}

	m_Stream.next_out = pbyDest;
	m_Stream.avail_out = dwDestSize;

	int nCompressedStatus = ::deflate(&m_Stream, Z_FINISH);

	switch (nCompressedStatus)
	{
	case Z_STREAM_END: dwCompressedSize = m_Stream.total_out; return Finished;
	case Z_OK: return OutBufferTooSmall;
	default: return Error;
	}
}

void CGZipCompressor::Reset()
{
	if (m_bInitialized)
	{
		::deflateReset(&m_Stream);
	}
}

// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

CBZip2Compressor::CBZip2Compressor(CompressorMode mode) : m_bInitialized(false), m_Mode(mode)
{
	SetMode(mode);
}

CBZip2Compressor::~CBZip2Compressor()
{
	if (m_bInitialized)
	{
		::BZ2_bzCompressEnd(&m_Stream);
		m_bInitialized = false;
	}
}
	
bool CBZip2Compressor::SetMode(CompressorMode mode)
{
	if (m_bInitialized)
	{
		::BZ2_bzCompressEnd(&m_Stream);
		m_bInitialized = false;
	}

	if (mode == None)
	{
		return false;
	}
	
	int nBlockSize = 4;
	switch (mode)
	{	
	case Size: nBlockSize = 9; break;
	case Speed: nBlockSize = 2; break;
	}

	memset(&m_Stream, 0, sizeof(m_Stream));
	m_bInitialized = ::BZ2_bzCompressInit(&m_Stream, nBlockSize, 0, 0) == BZ_OK ? true : false;

	// store the initialized mode
	if (m_bInitialized)
	{
		m_Mode = mode;
	}

	return m_bInitialized;
}

CompressorStatus CBZip2Compressor::Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize)
{
	if (!m_bInitialized || pbyDest == NULL || dwDestSize == 0)
	{
		return Error;
	}

	if (pbySource != NULL)
	{
		m_Stream.next_in = (char*) const_cast<BYTE*>(pbySource);
		m_Stream.avail_in = dwSourceSize;
	}

	if (m_Stream.next_in == NULL)
	{
		return Error;
	}

	m_Stream.next_out = (char*) pbyDest;
	m_Stream.avail_out = dwDestSize;
	
	int nCompressedStatus = BZ2_bzCompress(&m_Stream, BZ_FINISH);	

	switch (nCompressedStatus)
	{
	case BZ_STREAM_END: dwCompressedSize = m_Stream.total_out_lo32; return Finished;
	case BZ_FINISH_OK: return OutBufferTooSmall;
	default: return Error;
	}
}

void CBZip2Compressor::Reset()
{
	// TODO: how to do a faster reset?
	SetMode(m_Mode);
}