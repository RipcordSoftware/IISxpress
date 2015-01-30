/****************************************************************************
**
** Copyright 2008 Comtech AHA Corp - All rights reserved.
**
** FILENAME: AHAIoctl.h
** PURPOSE:  Contains the definitions, prototypes, etc for the IOCTL 
**			communication.
**	
** NOTES:    
**           
** HISTORY:
** 
****************************************************************************/
#ifndef __AHAIOCTL_H
#define __AHAIOCTL_H

// Misc Constants
#define	MAX_DMA_BUFFER_SIZE		0x10000
#define	MAX_CONTEXT				0x1000
#define MAX_OUTPUT_BUFFERS		7
#define MAX_INPUT_BUFFERS		7

// Define the IOCTL codes
#define FILE_DEVICE_AHA363	0x33DD
#define BASE_IOCTL			0X700

// IOCTLs handled by the driver
#define IOCTL_INIT_STREAM \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+1, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_REINIT_STREAM \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+2, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ADD_INIT_OUTPUT_BUFFERS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+3, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_START_STREAM \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+4, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ADD_MORE_INPUT_BUFFERS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+5, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ADD_MORE_OUTPUT_BUFFERS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+6, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ADD_AFOB_REPORT \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+7, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_QUERY_STREAM_STATS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+8, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_CLEAR_STREAM_STATS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+9, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_JTAG_REG \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+10, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_QUERY_TEMP_REG \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+11, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_DUMP_REGS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+12, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_PARMS \
	CTL_CODE(FILE_DEVICE_AHA363, BASE_IOCTL+13, METHOD_BUFFERED, FILE_ANY_ACCESS)


// Interface structures
typedef enum
{
	Compression,
	Decompression
}
OPERATION;

typedef struct
{
	union
	{
		struct
		{
			ULONG	Acrc:1;
			ULONG	Dfo:1;
			ULONG	Res:30;
		} bits;
		ULONG		Value;
	}u;
}
STRM_FLAGS, *PBUILD_FLAGS;

typedef struct
{
	union
	{
		struct
		{
			ULONG	Fs:1;
			ULONG	Fe:1;
			ULONG	Hbr:1;
			ULONG	Dir:1;
			ULONG	Pc:3;
			ULONG	Cd:1;
			ULONG	Res:24;
		} bits;
		ULONG		Value;
	}u;
}
SGL_FLAGS, *PSTRM_FLAGS;

typedef struct
{
	union
	{
		struct
		{
			ULONG	ZLib:1;
			ULONG	Res:31;
		} bits;
		ULONG		Value;
	}u;
}
STRM_OPS, *PSTRM_OPS;

typedef struct
{
	OPERATION	Operation;
	ULONG		DmaChannel;
	STRM_OPS	StrmOps;
	STRM_FLAGS	StrmFlags;
	SGL_FLAGS	SglFlags;
	ULONG		ContextSize;
	ULONG		DmaBlockSize;
	__int64		pInBuffer;
	ULONG64		InBufferLength;
}
BUILD_STREAM, *PBUILD_STREAM;

typedef struct
{
	SGL_FLAGS	SglFlags;
	ULONG		Res;
	__int64		pInBuffer;
	ULONG64		InBufferLength;
}
ADD_INPUT_BUFFERS, *PADD_INPUT_BUFFERS;

typedef struct
{
	BOOLEAN		InterruptWhenFull;
	ULONG		Res;
	__int64		pOutBuffer;
	ULONG64		OutBufferLength;
}
ADD_OUTPUT_BUFFERS, *PADD_OUTPUT_BUFFERS;

typedef struct
{
	ULONG		OutputFinished;
	ULONG64		OutputDataSize;		// Total amount of data in all output buffers
	ULONG		ErrorResult;
}
OBAF_INFO, *POBAF_INFO;

typedef struct
{
	ULONG		NumOutBufferFull[4];
	ULONG		NumOpsCompleted[4];
	ULONG		NumUnrecoverableErrors[4];
	ULONG		NumStreamErrors[4];
	ULONG		NumCmdsQueued[4];
	ULONG		NumStreamOverflows[4];
	ULONG		NumCmdOverflows[4];
	ULONG		NumSglInOverflows[4];
	ULONG		NumSglOutOverflows[4];
	ULONG		SpuriousCmdInterrupts[4];
	ULONG		NoAFOBReports[4];
	ULONG		TotalOutstandingIos;
	ULONG		SpuriousInterrupts;
	ULONG		DPCQueueFailures;
	ULONG		IntsEnabled;
}
STATS, *PSTATS;

typedef struct
{
	ULONG		TailIndexIntCount;
	ULONG		CmdBufferIntLevel;
}
SET_PARMS, *PSET_PARMS;

#endif