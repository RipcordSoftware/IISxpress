#pragma once

#include <setupapi.h>

#include <list>
#include <vector>

#include "AHAIncludes.h"

#include "..\IISxpressServer\ICompressor.h"

#include "IAHAStatus.h"

namespace AHA
{

class CSimpleCriticalSection
{

public:

	CSimpleCriticalSection() { InitializeCriticalSection(&cs); }
	~CSimpleCriticalSection() { DeleteCriticalSection(&cs); }

	void Lock() { EnterCriticalSection(&cs); }
	void Unlock() { LeaveCriticalSection(&cs); }

private:

	CRITICAL_SECTION cs;

};

class CSimpleManagedHandle
{

public:

	CSimpleManagedHandle() : handle(NULL) {}
	explicit CSimpleManagedHandle(HANDLE handle) { this->handle = handle; }

	~CSimpleManagedHandle() { Free(); }

	void Attach(HANDLE handle) { Free(); this->handle = handle; }
	void Free()
	{
		if (handle != NULL && handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle); 
			handle = NULL; 
		}
	}

	bool operator !() { return handle == NULL || handle == INVALID_HANDLE_VALUE; }

	operator HANDLE() { return handle; }

private:

	HANDLE handle;
};

enum REQ_TYPE
{
	UNKNOWN = 0,
	BUILD_STRM = 1,
	ADD_INPUT,
	ADD_OUTPUT,
	AFOB_REPORT
};

struct OVERLAPPED_EX
{
	OVERLAPPED_EX(REQ_TYPE type = UNKNOWN) : Index(0), RequestType(type)
	{
		memset(&Overlapped, 0, sizeof(Overlapped));		
		memset(&BuildStream, 0, sizeof(BuildStream));
		memset(&AddInput, 0, sizeof(AddInput));
		memset(&AddOutputBuffer, 0, sizeof(AddOutputBuffer));
		memset(&BafInfo, 0, sizeof(BafInfo));
	}

	~OVERLAPPED_EX()
	{
		FreeResources();
	}

	void FreeResources()
	{
		if (Overlapped.hEvent != NULL)
		{
			CloseHandle(Overlapped.hEvent);
			Overlapped.hEvent = NULL;
		}
	}

	OVERLAPPED			Overlapped;
	ULONG				Index;
	REQ_TYPE			RequestType;
	BUILD_STREAM		BuildStream;
	ADD_INPUT_BUFFERS	AddInput;
	ADD_OUTPUT_BUFFERS	AddOutputBuffer;
	OBAF_INFO			BafInfo;
};

typedef OVERLAPPED_EX* POVERLAPPED_EX;

template <int N, int SW> class CInOverlapped
{

public:

	CInOverlapped() : inOverlapIndex(0), QUEUE_SIZE(N), SEMAPHORE_WAIT(SW)
	{ 
		Init(); 		
	}

	bool Init()
	{
		inOverlapIndex = 0;
		
		for (ULONG loopIndex = 0; loopIndex < QUEUE_SIZE; ++loopIndex)
		{
			// Set up the Input overlapped structures
			POVERLAPPED_EX pInOverLapped = &glbInOverlappedArray[loopIndex];

			pInOverLapped->Index = 0;

			memset(&pInOverLapped->Overlapped, 0, sizeof(pInOverLapped->Overlapped));
			memset(&pInOverLapped->BuildStream, 0, sizeof(pInOverLapped->BuildStream));
			memset(&pInOverLapped->AddInput, 0, sizeof(pInOverLapped->AddInput));
			memset(&pInOverLapped->AddOutputBuffer, 0, sizeof(pInOverLapped->AddOutputBuffer));		
			memset(&pInOverLapped->BafInfo, 0, sizeof(pInOverLapped->BafInfo));			
		}	

		if (glbInOverlappedQueue.size() == 0)
		{			
			for (ULONG loopIndex = 0; loopIndex < QUEUE_SIZE; ++loopIndex)
			{
				// Add the item to the queue
				POVERLAPPED_EX pInOverLapped = &glbInOverlappedArray[loopIndex];					
				glbInOverlappedQueue.push_front(pInOverLapped);
			}
		}

		if (!glbhOverlappedSemaphore)
		{
			// Initialize a semaphore for the input overlapped queue
			glbhOverlappedSemaphore.Attach(CreateSemaphore(NULL, N, N, NULL));
			if (!glbhOverlappedSemaphore)
			{
				ULONG status = GetLastError();
				CAHA363::DbgPrint(_T("AHAStrmInit: Failed to create input semaphore. Status: %i\n"), status);				
			}
		}

		return glbInOverlappedQueue.size() > 0 && !!glbhOverlappedSemaphore;
	}


	POVERLAPPED_EX GetOverlap()
	{
		ULONG waitStat = WaitForSingleObject(glbhOverlappedSemaphore, SEMAPHORE_WAIT);
		if (waitStat != WAIT_OBJECT_0)
		{
			if (waitStat == WAIT_TIMEOUT)
			{
				CAHA363::DbgPrint(_T("AHAGetInputOverlap: Timed out waiting for input overlap\n"));
			}
			else
			{
				ULONG status = GetLastError();
				CAHA363::DbgPrint(_T("AHAGetInputOverlap: Failed to get overlap. Status: %i\n"), status);
			}

			return NULL;
		}

		csInOverlapped.Lock();

		POVERLAPPED_EX pInOverLapped = glbInOverlappedQueue.front();
		glbInOverlappedQueue.pop_front();

		_ASSERTE(pInOverLapped != NULL);

		pInOverLapped->Index = inOverlapIndex++;
	    
		csInOverlapped.Unlock();		

		return pInOverLapped;
	}

	VOID ReturnOverlap(POVERLAPPED_EX pInOverLapped)
	{
		csInOverlapped.Lock();
		glbInOverlappedQueue.push_front(pInOverLapped);
		csInOverlapped.Unlock();

		ReleaseSemaphore(glbhOverlappedSemaphore, 1, NULL);
	}


	const unsigned int QUEUE_SIZE;
	const unsigned int SEMAPHORE_WAIT;

private:

	int inOverlapIndex;

	OVERLAPPED_EX glbInOverlappedArray[N];
	std::list<POVERLAPPED_EX> glbInOverlappedQueue;
	CSimpleCriticalSection csInOverlapped;
	CSimpleManagedHandle glbhOverlappedSemaphore;

};

template <int N> class COutOverlapped
{

public:

	COutOverlapped() : outOverlapIndex(0), QUEUE_SIZE(N) { Init(); }

	bool Init()
	{
		outOverlapIndex = 0;

		for (ULONG loopIndex = 0; loopIndex < QUEUE_SIZE; ++loopIndex)
		{
			// Set up the Input overlapped structures
			POVERLAPPED_EX pOutOverLapped = &glbOutOverlappedArray[loopIndex];

			pOutOverLapped->Index = 0;

			memset(&pOutOverLapped->Overlapped, 0, sizeof(pOutOverLapped->Overlapped));
			memset(&pOutOverLapped->BuildStream, 0, sizeof(pOutOverLapped->BuildStream));
			memset(&pOutOverLapped->AddInput, 0, sizeof(pOutOverLapped->AddInput));
			memset(&pOutOverLapped->AddOutputBuffer, 0, sizeof(pOutOverLapped->AddOutputBuffer));		
			memset(&pOutOverLapped->BafInfo, 0, sizeof(pOutOverLapped->BafInfo));			
		}	

		if (glbOutOverlappedQueue.size() == 0)
		{							        
			for (ULONG loopIndex = 0; loopIndex < QUEUE_SIZE; ++loopIndex)
			{
				// Add the item to the queue            
				POVERLAPPED_EX pOutOverLapped = &glbOutOverlappedArray[loopIndex];					
				glbOutOverlappedQueue.push_front(pOutOverLapped);				
			}
		}

		return glbOutOverlappedQueue.size() > 0;
	}

	POVERLAPPED_EX GetOverlap()
	{
		if (glbOutOverlappedQueue.empty() == TRUE)
		{
			CAHA363::DbgPrint(_T("AHAGetOutputOverlap: Output overlap queue is empty\n"));
			return NULL;
		}

		csOutOverlapped.Lock();

		POVERLAPPED_EX pOutOverLapped = glbOutOverlappedQueue.front();
		glbOutOverlappedQueue.pop_front();

		pOutOverLapped->Index = outOverlapIndex++;

		csOutOverlapped.Unlock();		
	    
		return pOutOverLapped;
	}

	VOID ReturnOverlap(POVERLAPPED_EX pOutOverLapped)
	{
		csOutOverlapped.Lock();
		glbOutOverlappedQueue.push_front(pOutOverLapped);
		csOutOverlapped.Unlock();
	}

	const unsigned int QUEUE_SIZE;

private:

	int outOverlapIndex;

	OVERLAPPED_EX glbOutOverlappedArray[N];
	CSimpleCriticalSection csOutOverlapped;
	std::list<POVERLAPPED_EX> glbOutOverlappedQueue;

};

class CAHA363 : public ICompressor, public IAHAStatus
{

private:

	// the number of items to hold in the queue
	static const int MAX_QUEUE_ITEMS = 16;

	static const int MAX_RESOURCE_RETRIES = 100;

	// the amount of time the get overlap call will wait for the semaphore
	static const int OVERLAP_SEMAPHORE_WAIT_TIME = 100;

	// the amount of time to wait for a compression job to complete
	static const int COMPRESSION_WAIT_TIME = 30000;

	// the amount of time to wait for the IO thread to terminate
	static const int THREAD_WAIT_TIME = 10000;

public:

	// the maximum temperature of the AHA FPGA
	static const int MAX_AHA_FPGA_TEMP = 85;

	CAHA363() : 
		glbhDevice(INVALID_HANDLE_VALUE),
		glbOutstandingInputRequests(0), glbOutstandingOutputRequests(0), 
		glbDmaBlockSize(0), glbOutputPreload(MAX_OUTPUT_BUFFERS),
		glbDataOutputSize(0), glbOutputSize(0), glbFinalOpStatus(ERROR_SUCCESS), glbSeqRevd(0),
		glbAFOBOverLap(AFOB_REPORT),
		issueReinit(FALSE),
		m_pbyInBuffer(NULL), m_dwInBufferSize(0), m_pbyOutBuffer(NULL), m_dwOutBufferSize(0),
		overflowBuffer(64 * 1024)
		{}

	~CAHA363()
	{
		AHATerminateThread();

		AHA363Cleanup();		
	}

	ULONG Open(int board, ULONG DmaChannel, ULONG DmaBlockSize, STRM_FLAGS StrmFlags, SGL_FLAGS SglFlags, BOOL ZLib);
	VOID AHA363Cleanup();

	// ICompressor methods
	bool SetMode(CompressorMode) { return true; }
	CompressorStatus Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize);
	void Reset() {}	
	bool CanContinueOnOutBufferTooSmall() { return false; }
	
	// IAHAStatus methods
	bool GetStats(STATS& stats);
	bool ClearStats();
	bool GetTemps(DWORD& dwFPGA, DWORD& dwBoard);
	bool GetBoardInfo(AHABoardInfo& info) { info = m_BoardInfo; return true; }

	static VOID DbgPrint(IN TCHAR* szFormat, IN ...);

private:	

	bool IsBoardInit() { return glbhDevice != INVALID_HANDLE_VALUE; }

	ULONG AHA363OpenHandle(IN ULONG BoardIndex);
	static bool GetBoardInfo(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, AHABoardInfo& info);

	ULONG AHAStrmFile(IN BOOL ReinitStrm, OUT PULONG64 pFinalDataSize);

	ULONG AHASendAFOBReport();

	// the output thread
	static DWORD WINAPI AHAStrmThread(IN PVOID pContext);
	VOID AHAStrmThread();
	BOOL AHATerminateThread();
	ULONG AHAUpdateOutput(IN POVERLAPPED_EX pOverLapped);

	ULONG AHAInitStrm(IN POVERLAPPED_EX pOverLapped, IN ULONG StrmFlags, IN ULONG SglFlags, bool& eof);
	ULONG AHAReinitStrm(IN POVERLAPPED_EX pOverLapped, IN ULONG SglFlags, bool& eof);
	ULONG AHAStartStrm();

	ULONG AHAAddMoreInput(IN POVERLAPPED_EX pOverLapped, IN ULONG SglFlags, bool& eof);
	ULONG AHAAddInitOutput(IN BOOLEAN InterruptWhenFull, IN POVERLAPPED_EX pOverLapped, bool& eof);	
	ULONG AHAAddMoreOutput(IN BOOLEAN InterruptWhenFull, IN POVERLAPPED_EX pOverLapped, bool& eof);

	VOID AHASetupBuildStream(IN POVERLAPPED_EX pOverLapped, IN ULONG StrmFlags, IN ULONG SglFlags, bool& eof);
	VOID AHASetupAddInput(IN POVERLAPPED_EX pOverLapped, IN PADD_INPUT_BUFFERS pAddInput, IN ULONG SglFlags, bool& eof);
	VOID AHASetupAddOutput(IN POVERLAPPED_EX pOverLapped, IN BOOLEAN InterruptWhenFull,IN PADD_OUTPUT_BUFFERS pAddOutput, bool& eof);			

	void GetInBufferParams(DWORD dwInBufferLength, DWORD index, DWORD dmaBlockSize, DWORD& dwBufferOffset, DWORD& dwBufferRemaining, SGL_FLAGS& sglFlags, bool& eof);
	static void GetOutBufferParams(DWORD dwOutBufferLength, DWORD index, DWORD dmaBlockSize, DWORD& dwBufferOffset, DWORD& dwBufferRemaining, bool& eof);
	
	HANDLE glbhDevice;

	CInOverlapped<MAX_QUEUE_ITEMS, OVERLAP_SEMAPHORE_WAIT_TIME> m_InOverlapped;
	COutOverlapped<MAX_QUEUE_ITEMS> m_OutOverlapped;	

	OVERLAPPED_EX glbAFOBOverLap;
				
	CSimpleManagedHandle glbhStrmFinishEvent;
	
	CSimpleManagedHandle glbhCompletionPort;
	CSimpleManagedHandle glbhReceiveThread;
	volatile LONG glbOutstandingInputRequests;
	volatile LONG glbOutstandingOutputRequests;

	// Global stream parameters	
	ULONG glbDMAChannel;
	SGL_FLAGS glbSglFlags;
	STRM_FLAGS glbStrmFlags;
	BOOL glbZLib;
	ULONG64 glbDataOutputSize;
	ULONG glbDmaBlockSize;
	const ULONG glbOutputPreload;

	// Globals maintained by the thread
	ULONG64 glbOutputSize;
	ULONG glbFinalOpStatus;
	ULONG glbSeqRevd;

	// set to true if the stream is to be re-initialized
	BOOL issueReinit;

	// input buffer members
	const BYTE* m_pbyInBuffer;
	DWORD m_dwInBufferSize;

	// output buffer members
	BYTE* m_pbyOutBuffer;
	DWORD m_dwOutBufferSize;

	// a buffer to use when there is not enough output buffer left
	std::vector<BYTE> overflowBuffer;

	AHABoardInfo m_BoardInfo;
};

}