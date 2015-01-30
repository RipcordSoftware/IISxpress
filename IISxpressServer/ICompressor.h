#pragma once

enum CompressorMode
{
	None = 0,
	Default = 1,
	Speed = 2,
	Size = 3
};

enum CompressorStatus
{
	Error = -1,
	Finished = 0,
	OutBufferTooSmall = 1
};

// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************
// ****************************************************************************************************************************************************************

class ICompressor
{
public:

	virtual bool SetMode(CompressorMode) = 0;

	virtual CompressorStatus Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize) = 0;

	virtual void Reset() = 0;

	virtual bool CanContinueOnOutBufferTooSmall() = 0;

};