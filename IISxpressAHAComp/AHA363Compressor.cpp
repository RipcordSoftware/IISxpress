#include "stdafx.h"

#include "AHA363Compressor.h"

#include <Cfgmgr32.h>

// TODO: make sure memory fail responses are all the same

ULONG AHA::CAHA363::Open(int board, ULONG DmaChannel, ULONG DmaBlockSize, STRM_FLAGS StrmFlags, SGL_FLAGS SglFlags, BOOL ZLib)
{
    // Create an auto-resetting event used to signal stream finished
    glbhStrmFinishEvent.Attach(CreateEvent(NULL, FALSE, FALSE, NULL));
    if (!glbhStrmFinishEvent)
    {
        ULONG status = GetLastError();	
        DbgPrint(_T("AHAStrmInit: Failed to create stream finish event. Status = %i\n"), status);
        return status;
    }

	ULONG status = AHA363OpenHandle(board);
	if (status == ERROR_SUCCESS)
	{	
		// Create a completion port
		glbhCompletionPort.Attach(CreateIoCompletionPort(glbhDevice, NULL, 0, 1));
		if (!glbhCompletionPort)
		{
			status = GetLastError();
			DbgPrint(_T("AHAStrmInit: Failed to create a completion port. Status: %i\n"), status);			
		}

		//  Start the asynchronous receive thread.
		if (status == ERROR_SUCCESS)
		{
			glbhReceiveThread.Attach(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) AHAStrmThread, (LPVOID) this, 0, NULL));
			if (!glbhReceiveThread)
			{
				status = GetLastError();
				DbgPrint(_T("AHAStrmInit: Failed to create the async thread. Status: %i\n"), status);			
			}
		}

		if (status != ERROR_SUCCESS)
		{
			AHA363Cleanup();
		}
		else
		{
			// Set up the global stream parameters
			glbDMAChannel	= DmaChannel;
			glbSglFlags		= SglFlags;
			glbStrmFlags	= StrmFlags;
			glbZLib			= ZLib;
			glbDmaBlockSize = DmaBlockSize;
		}
	}	

	return status;
}

/****************************************************************************
**
** AHA363OpenHandle
**	
** PURPOSE: This routine will scan the registry to find all of the available 
**			AHA363 Compression/Decompression cards.
** 
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHA363OpenHandle(IN ULONG BoardIndex)
{
    ULONG status = ERROR_SUCCESS;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    SP_INTERFACE_DEVICE_DATA ifDevData;
    ifDevData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

    // Start by opening an enumeration handle for the WA USB class GUID
    hDevInfo = SetupDiGetClassDevs((GUID*) &AHA363_GUID, 
                                       NULL,                      // No Enumerator
                                       NULL,                      // No parent hWnd
                                       DIGCF_PRESENT | 
                                       DIGCF_INTERFACEDEVICE);   
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        status = GetLastError();
        DbgPrint(_T("AHA363OpenHandle: Failed to open a handle to the class key. Status: %i\n"), status);
        return status;
    }

    // Loop through and enumerate all of the devices in device info.    
    ULONG dwDevIndex = 0;
    while (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, (GUID*) &AHA363_GUID, dwDevIndex, &ifDevData) != 0)
    {
        // Get the size needed to store the device interface details
		ULONG dwSizeRequired = 0;
        if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifDevData, NULL, 0, &dwSizeRequired, NULL) == FALSE)
        {
            status = GetLastError();
            if (status != ERROR_INSUFFICIENT_BUFFER)
            {
                status = GetLastError();
                DbgPrint(_T("SetupDiGetDeviceInterfaceDetail Failed. Status = %x\n"), status);
                break;                
            }

			status = ERROR_SUCCESS;
        }

        if ((dwDevIndex + 1) != BoardIndex)
        {
            ++dwDevIndex;
            continue;
        }

        //  Allocate the detail buffer 
        PSP_INTERFACE_DEVICE_DETAIL_DATA pfDevDetailData = (PSP_INTERFACE_DEVICE_DETAIL_DATA) alloca(dwSizeRequired);
        if (pfDevDetailData == NULL)
        {
			status = ERROR_OUTOFMEMORY;
            DbgPrint(_T("Failed to alloc buffer of IF data.\n"));
            
            // Exit the loop
            break;
        }

        pfDevDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

        //  Get the Device Interface details
        if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifDevData, pfDevDetailData, dwSizeRequired, NULL, &devInfoData) == FALSE)
        {
            status = GetLastError();
            DbgPrint(_T("SetupDiGetDeviceInterfaceDetail failed. Status: %x\n"), status);            

            // Exit the loop
            break;
        }

		// Get information about the board
		GetBoardInfo(hDevInfo, devInfoData, m_BoardInfo);

        // Open a handle to the device
        glbhDevice = CreateFile(pfDevDetailData->DevicePath, 
                                GENERIC_READ | GENERIC_WRITE, 
                                FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_OVERLAPPED,
                                NULL);

        if (glbhDevice == INVALID_HANDLE_VALUE)
        {
            status = GetLastError();
            DbgPrint(_T("Failed to open the file handle. Status: %x\n"), status);            
        }

        // Exit the loop
        break;        
    }
    
    //  Close any resources we actually acquired.
    if (hDevInfo != INVALID_HANDLE_VALUE)
    {
        // Free the handle the device information
        SetupDiDestroyDeviceInfoList(hDevInfo);
		hDevInfo = INVALID_HANDLE_VALUE;
    }

	// if no devices have been detected then there are none connected
	if (glbhDevice == INVALID_HANDLE_VALUE)
	{
		status = ERROR_DEVICE_NOT_CONNECTED;
	}

    return status;
}

bool AHA::CAHA363::GetBoardInfo(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, AHABoardInfo& info)
{
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	
	DWORD dwDataType = 0;
	DWORD dwRequiredSize = 0;
	SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_DEVICEDESC, &dwDataType, (PBYTE) &info.szDescription[0], _countof(info.szDescription), &dwRequiredSize);

	dwDataType = 0;
	dwRequiredSize = 0;
	SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_INSTALL_STATE, &dwDataType, (PBYTE) &info.InstallState, sizeof(info.InstallState), &dwRequiredSize);

	DWORD dwConfigFlags = 0;
	dwDataType = 0;
	dwRequiredSize = 0;
	SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_CONFIGFLAGS , &dwDataType, (PBYTE) &dwConfigFlags, sizeof(dwConfigFlags), &dwRequiredSize);

	info.enabled = (dwConfigFlags & 1) == 0;	

	return true;
}

/****************************************************************************
**
** AHA363Cleanup
**	
** PURPOSE: This routine will free all resources that have been create for 
**			the application.
** 
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
VOID AHA::CAHA363::AHA363Cleanup()
{
    // Close handle to the board
    if (glbhDevice != INVALID_HANDLE_VALUE)
    {
        CloseHandle(glbhDevice);
        glbhDevice = INVALID_HANDLE_VALUE;
    }
}

/****************************************************************************
**
** DbgPrint
** PURPOSE: 
** 
**	This function will format a string and prints it to the debugger. It takes
**	a variable number of arguments.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
VOID AHA::CAHA363::DbgPrint(IN TCHAR* szFormat, IN ...)
{    
    // Initialize the arg pointer to the start of the variable parameters
    va_list	varList;
    va_start(varList, szFormat);

	TCHAR szDbgString[4096] = _T("");
    _vstprintf_s(szDbgString, _countof(szDbgString), szFormat, varList);

    // Display the string
    OutputDebugString(szDbgString);
}

/****************************************************************************
**
** AHAStrmStart
**	
** PURPOSE: This function is used to start a compression/decompression stream
**			on the specified file.
** 
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
CompressorStatus AHA::CAHA363::Compress(const BYTE* pbySource, DWORD dwSourceSize, BYTE* pbyDest, DWORD dwDestSize, DWORD& dwCompressedSize)
{
	if (pbySource == NULL || dwSourceSize == 0 || pbyDest == NULL)
	{
		return Error;
	}

	if (!IsBoardInit())
	{
		return Error;
	}

	// Initialize the output parms
	dwCompressedSize = 0;

	// TODO: is this the best place for this?
	if (issueReinit == TRUE)
	{
		m_InOverlapped.Init();
		m_OutOverlapped.Init();
	}    

	glbSeqRevd = 0;
	glbDataOutputSize = 0;
	glbOutputSize = 0;	
	glbOutstandingInputRequests = 0;
	glbOutstandingOutputRequests = 0;
	glbFinalOpStatus = ERROR_SUCCESS;	

	// CM: store for later
	m_pbyInBuffer = pbySource;
	m_dwInBufferSize = dwSourceSize;
	m_pbyOutBuffer = pbyDest;
	m_dwOutBufferSize = dwDestSize;

	CompressorStatus status = Finished;
	ULONG64 finalDataSize = 0;
	ULONG ahaStatus = AHAStrmFile(issueReinit, &finalDataSize);
	if (ahaStatus == ERROR_NOT_ENOUGH_MEMORY)
	{
		status = OutBufferTooSmall;
	}
	else if (ahaStatus != ERROR_SUCCESS)    
    {
        DbgPrint(_T("AHAStrmStart: AHAStrmFile failed. Status: %i\n"), ahaStatus);
		status = Error;
    }        
	else
	{
		dwCompressedSize = (DWORD) finalDataSize;
	}

	issueReinit = TRUE;    
        
    return status;
}

/****************************************************************************
**
** AHAStrmFile
**	
** PURPOSE: This function is used to stream out a file.
** 
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAStrmFile(BOOL ReinitStrm, PULONG64 pFinalDataSize)
{
    ULONG status = ERROR_SUCCESS;        

    // Initialize the input parms
    *pFinalDataSize = 0;

    // Get an overlapped item
    POVERLAPPED_EX pInOverLapped = m_InOverlapped.GetOverlap();	
    if (pInOverLapped == NULL)
    {
        return ERROR_TOO_MANY_SEM_REQUESTS;
    }

	// Increment the number of outstanding requests
    InterlockedIncrement(&glbOutstandingInputRequests);

	// track when we get the end of the input file
	bool inEOF = false;
    
    if (ReinitStrm == FALSE)
    {
        status = AHAInitStrm(pInOverLapped, glbStrmFlags.u.Value, glbSglFlags.u.Value, inEOF);
        if (status != ERROR_SUCCESS)
        {
            DbgPrint(_T("AHAStrmFiles: Failed to init the stream. Status: %i\n"), status);

            // Put the overlap back on the list
            m_InOverlapped.ReturnOverlap(pInOverLapped);

			InterlockedDecrement(&glbOutstandingInputRequests);

            return status;
        }
    }
    else
    {	
        status = AHAReinitStrm(pInOverLapped, glbSglFlags.u.Value, inEOF);
        if (status != ERROR_SUCCESS)
        {
            DbgPrint(_T("AHAStrmFiles: Failed to Re-init the stream. Status: %i\n"), status);

            // Put the overlap back on the list
            m_InOverlapped.ReturnOverlap(pInOverLapped);

			InterlockedDecrement(&glbOutstandingInputRequests);

            return status;
        }
    }    

	// calculate how many output buffers we can preload
	DWORD dwOutputPreload = m_dwOutBufferSize / glbDmaBlockSize;
	dwOutputPreload += (m_dwOutBufferSize % glbDmaBlockSize > 0);
	if (dwOutputPreload > glbOutputPreload)
	{
		dwOutputPreload = glbOutputPreload;
	}

    // queue up the initial output buffers
	bool outEOF = false;
    for (ULONG loopIndex = 0; !outEOF && loopIndex < dwOutputPreload; ++loopIndex)
    {
        // add an output buffer        
		POVERLAPPED_EX pOutOverLapped = m_OutOverlapped.GetOverlap();
        if (pOutOverLapped == NULL)
        {
            DbgPrint(_T("AHAStrmFile: Output overlap is null\n"));
            continue;
        }

		// increment the number of outstanding requests
        InterlockedIncrement(&glbOutstandingOutputRequests);

        if (loopIndex == (dwOutputPreload-1))
        {
            // interrupt when this buffer is full
            status = AHAAddInitOutput(TRUE, pOutOverLapped, outEOF);
        }
        else
        {
            // don't interrupt when this buffer is full
            status = AHAAddInitOutput(FALSE, pOutOverLapped, outEOF);
        }

        if (status != ERROR_SUCCESS)
        {
            DbgPrint(_T("AHAStrmFile: Failed to add initial output . Status: %i\n"), status);

            // put the overlap back on the list            
			m_OutOverlapped.ReturnOverlap(pOutOverLapped);

			InterlockedDecrement(&glbOutstandingInputRequests);

            continue;
        }        
    }

    if (glbOutstandingOutputRequests == 0)
    {
        DbgPrint(_T("AHAStrmFile: Failed to queue any initial output buffers\n"));
       
        return status;
    }

    // Queue an AFOB Report
    status = AHASendAFOBReport();
    if (status != ERROR_SUCCESS)
    {
        DbgPrint(_T("AHAStrmFile: Failed to queue AFOB Report\n"));
        
        return status;
    }

    status = AHAStartStrm();        
    if (status != ERROR_SUCCESS)
    {        
        return status;
    }

    ULONG resourceCount = 0;

	while (!inEOF)
    {
        pInOverLapped = m_InOverlapped.GetOverlap();
        if (pInOverLapped == NULL)
        {
            DbgPrint(_T("AHAStrmFile: Failed to get input overlap.\n"));
            ++resourceCount;

            if (resourceCount > MAX_RESOURCE_RETRIES)
            {
                DbgPrint(_T("AHAStrmFile: Exceeded Max Resource Wait count.\n"));
                break;
            }

            Sleep(10);
            continue;
        }

        resourceCount = 0;

		// Increment the number of outstanding requests
        InterlockedIncrement(&glbOutstandingInputRequests);

        status = AHAAddMoreInput(pInOverLapped, glbSglFlags.u.Value, inEOF);
        if (status != ERROR_SUCCESS)
        {
            DbgPrint(_T("AHAStrmFile: Failed to add more input buffers. Status: %i\n"), status);

            // Put the overlap back on the list
            m_InOverlapped.ReturnOverlap(pInOverLapped);

			InterlockedDecrement(&glbOutstandingInputRequests);

            continue;
        }        
    }

    // Input is complete. Wait for the thread to terminate
	// TODO: review this wait time
    ULONG waitStat = WaitForSingleObject(glbhStrmFinishEvent, COMPRESSION_WAIT_TIME);
    if (waitStat != WAIT_OBJECT_0)
    {
        if (waitStat == WAIT_TIMEOUT)
        {
            DbgPrint(_T("AHAStrmFile: Timed out waiting for stream finish event\n"));
        }
        else
        {
            status = GetLastError();
            DbgPrint(_T("AHAStrmFile: Failed to wait for stream finish event: %i\n"), status);
        }
    }

    *pFinalDataSize	= glbOutputSize;

    return glbFinalOpStatus;
}

/****************************************************************************
**
** AHASendAFOBReport
** PURPOSE: 
** 
**	This function gets an input overlap off the input queue.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHASendAFOBReport()
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}

    ULONG bytesRet = 0;
    BOOL success = DeviceIoControl(glbhDevice, IOCTL_ADD_AFOB_REPORT, NULL, 0, NULL, 0, &bytesRet, &glbAFOBOverLap.Overlapped);
    if (success == FALSE)
    {
        ULONG status = GetLastError();
        if (status != ERROR_IO_PENDING)
        {
            DbgPrint(_T("AHASendAFOBReport: Failed to queue the AFOB Report. Status: %i\n"), status);
            return status;
        }
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHAStrmThread
**	
** PURPOSE: This is the thread that will block on the Io completion port and 
**			then process completed stram requests. These requests include: 
**			stream complete, buffer almost full, stream errors, etc.
** 
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
DWORD AHA::CAHA363::AHAStrmThread(IN PVOID pContext)
{
	if (pContext != NULL)
	{
		CAHA363* pThis = (CAHA363*) pContext;
		pThis->AHAStrmThread();
	}

	return 0;
}

VOID AHA::CAHA363::AHAStrmThread()
{
    //  Loop forever processing data or cancel
    while (1)
    {
		ULONG bytesRet = 0;
		ULONG_PTR compKey = 0;
		POVERLAPPED_EX pOverLapped = NULL;

        BOOL success = GetQueuedCompletionStatus(glbhCompletionPort, &bytesRet, &compKey, (LPOVERLAPPED*) &pOverLapped, INFINITE);
        if (pOverLapped == NULL)
        {
            ULONG status = GetLastError();

            //  Check to see if the thread is being terminated
            if (compKey == -1)
            {
                ExitThread(0);
            }

            DbgPrint(_T("AHAStrmThread: Failed to dequeue a completion packet. Status: %i\n"), status);

            continue;
        }

        if (success == FALSE)
        {
            ULONG status = GetLastError();
            if (status == ERROR_MORE_DATA)
            {
                // We ran out of space while generating an output buffer
                _ASSERTE(pOverLapped->RequestType == ADD_OUTPUT || 
                       pOverLapped->RequestType == AFOB_REPORT);

                if (pOverLapped->RequestType == ADD_OUTPUT)
                {
                    _ASSERTE(bytesRet == sizeof(OBAF_INFO));

                    // Decrement the number of outstanding requests
                    LONG outstandingOutputRequests = InterlockedDecrement(&glbOutstandingOutputRequests);
                    _ASSERTE(outstandingOutputRequests >= 0);

                    // Write the data to the output file
                    status = AHAUpdateOutput(pOverLapped);
                    if (status != ERROR_SUCCESS)
                    {
                        DbgPrint(_T("AHAStrmThread (ERROR_MORE_DATA): Failed to write output file -- non fatal. Status: %i\n"), status);
                    }                    
					
					m_OutOverlapped.ReturnOverlap(pOverLapped);
                }
                else
                {
                    // Recycle the AFOB Report
                    status = AHASendAFOBReport();                    
                    if (status != ERROR_SUCCESS)
                    {
                        DbgPrint(_T("AHAStrmThread: Failed to queue ADD AFOB Report request -- Non-FATAL. Status: %i\n"), status);
                    }
                }

				// Get an output overlap				
				pOverLapped = m_OutOverlapped.GetOverlap();
				_ASSERTE(pOverLapped != NULL);

                // We need to queue up a new buffer
                // 
                // Set up the output buffer parameters
				bool outEOF = false;
                status = AHAAddMoreOutput(TRUE, pOverLapped, outEOF);                
                if (status != ERROR_SUCCESS)
                {
                    DbgPrint(_T("AHAStrmThread: Failed to queue ADD MORE OUTPUT BUFFERS request -- FATAL. Status: %i\n"), status);

                    // Put the overlap back on the list                    
					m_OutOverlapped.ReturnOverlap(pOverLapped);

                    // Consider this a fatal error
                    glbFinalOpStatus = status;
                }
                
                // Increment the number of outstanding requests
                InterlockedIncrement(&glbOutstandingOutputRequests);
                continue;
            }

            // We got another error
            // Put the overlap back on the list
            if (pOverLapped->RequestType == ADD_OUTPUT)
            {
                DbgPrint(_T("AHAStrmThread: Stream error encountered on add output -- FATAL. Status: %i\n"), status);

                _ASSERTE(bytesRet == sizeof(OBAF_INFO));
                _ASSERTE(pOverLapped->RequestType == ADD_OUTPUT);

                // Decrement the number of outstanding requests
                LONG outstandingOutputRequests = InterlockedDecrement(&glbOutstandingOutputRequests);
                _ASSERTE(outstandingOutputRequests >= 0);

                // Errors on Add Output requests are fatal
                POBAF_INFO pObafInfo = &pOverLapped->BafInfo;
                if (pObafInfo->OutputFinished == TRUE)
                {    
                    glbOutputSize = pObafInfo->OutputDataSize;
                }

                // Put the overlap back on the list                
				m_OutOverlapped.ReturnOverlap(pOverLapped);
                glbFinalOpStatus = status;

                // Signal a waiting thread
                SetEvent(glbhStrmFinishEvent);
                continue;
            }
            else
            {
                DbgPrint(_T("AHAStrmThread: Stream error encountered on add input -- non-fatal. Status: %i\n"), status);

                LONG outstandingInputRequests = InterlockedDecrement(&glbOutstandingInputRequests);
                _ASSERTE(outstandingInputRequests >= 0);

                m_InOverlapped.ReturnOverlap(pOverLapped);
            }
            continue;
        }

        // The stream segement must have completed
        // 
        if (pOverLapped->RequestType == ADD_OUTPUT)
        {
            // An output segment just completed
            LONG outstandingOutputRequests = InterlockedDecrement(&glbOutstandingOutputRequests);
            _ASSERTE(outstandingOutputRequests >= 0);

            // Write the he output data to the file
            ULONG status = AHAUpdateOutput(pOverLapped);
            if (status != ERROR_SUCCESS)
            {
                DbgPrint(_T("AHAStrmThread: Failed to write file output data -- non-Fatal. Status: %i. \n"), status);
            }            
            
            // Get the buffer almost full data
            POBAF_INFO pObafInfo = &pOverLapped->BafInfo;
            if (pObafInfo->OutputFinished == TRUE)
            {    
                glbOutputSize = pObafInfo->OutputDataSize;

                // Put the overlap back on the list                
				m_OutOverlapped.ReturnOverlap(pOverLapped);

                // Signal a waiting thread
                SetEvent(glbhStrmFinishEvent);
            }
            else
            {
                // Put the overlap back on the list                
				m_OutOverlapped.ReturnOverlap(pOverLapped);
            }
            continue;
        }
    
        // We must have an Build Stream, Add Input request, or AFOB Report
        _ASSERTE(pOverLapped->RequestType == BUILD_STRM || 
                pOverLapped->RequestType == ADD_INPUT  ||
                pOverLapped->RequestType == AFOB_REPORT);

        if (pOverLapped->RequestType == BUILD_STRM || 
            pOverLapped->RequestType == ADD_INPUT)
        {
            LONG outstandingInputRequests = InterlockedDecrement(&glbOutstandingInputRequests);
            _ASSERTE(outstandingInputRequests >= 0);
        
            // Return the input overlap
            m_InOverlapped.ReturnOverlap(pOverLapped);
        }
    }
}

/****************************************************************************
**
** AHAInitStrm
** PURPOSE: 
** 
**	This function is sets up the init stream parameters and then issue the 
**	IOCTL_INIT_STREAM to the device.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAInitStrm(IN POVERLAPPED_EX pOverLapped, IN ULONG StrmFlags, IN ULONG SglFlags, bool& eof)
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}    

    // Set up the stream parms for the first buffer
    AHASetupBuildStream(pOverLapped, StrmFlags, SglFlags, eof);	

	ULONG status = ERROR_SUCCESS;
	const ULONG maxResourceRetry = 10000;
    ULONG resourceRetry = maxResourceRetry;
    do
    {
        // Queue the second request
        ULONG bytesRet = 0;
        BOOL success = DeviceIoControl(glbhDevice, 
                                IOCTL_INIT_STREAM,
                                &pOverLapped->BuildStream, 
                                sizeof(BUILD_STREAM),
                                NULL,
                                0,
                                &bytesRet,
                                &pOverLapped->Overlapped);
    
        if (success == FALSE)
        {
            status = GetLastError();
            if (status != ERROR_IO_PENDING && status != ERROR_NO_SYSTEM_RESOURCES)
            {
                DbgPrint(_T("AHAInitStrm: Failed to initialize the stream. Status: %i\n"), status);
                return status;
            }

            if (status == ERROR_NO_SYSTEM_RESOURCES)
            {
                --resourceRetry;

                if (resourceRetry > 0)
                {
                    Sleep(5);
                }
                if (resourceRetry % 10 == 0)
                {
                    DbgPrint(_T("AHAInitStrm: Failed %i consecutive attempts.\n"), maxResourceRetry - resourceRetry);
                }
            }
        }
    } while (status == ERROR_NO_SYSTEM_RESOURCES && resourceRetry > 0);

    if (resourceRetry == 0)
    {
        DbgPrint(_T("AHAInitStrm: Failed waiting for resources. Status: %i\n"),  status);
        return status;
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHAReinitStrm
** PURPOSE: 
** 
**	This function is sets up the Re-init stream parameters and then issue the 
**	IOCTL_REINIT_STREAM to the device.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAReinitStrm(IN POVERLAPPED_EX pOverLapped, IN ULONG SglFlags, bool& eof)
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}
    
    AHASetupAddInput(pOverLapped, &pOverLapped->AddInput, SglFlags, eof);

    // Issue the command
    ULONG bytesRet = 0;
    BOOL success = DeviceIoControl(glbhDevice, 
                            IOCTL_REINIT_STREAM,
                            &pOverLapped->AddInput, 
                            sizeof(ADD_INPUT_BUFFERS),
                            NULL,
                            0,
                            &bytesRet,
                            &pOverLapped->Overlapped);
    
    if (success == FALSE)
    {
        ULONG status = GetLastError();
        if (status != ERROR_IO_PENDING)
        {
            DbgPrint(_T("AHAReinitStrm: Failed to queue add more input buffers. Status: %i\n"), status);
            return status;
        }
    }
    
    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHAAddInitOutput
** PURPOSE: 
** 
**	This function sets up the Add Output parameters and then issues the 
**	IOCTL_ADD_INIT_OUTPUT_BUFFERS to the device.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAAddInitOutput(IN BOOLEAN InterruptWhenFull, IN POVERLAPPED_EX pOverLapped, bool& eof) 
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}

    // Set up the Add Output parameters
	eof = false;
    AHASetupAddOutput(pOverLapped, InterruptWhenFull, &pOverLapped->AddOutputBuffer, eof);

    // Queue the ADD OUTPUT request
    ULONG bytesRet = 0;
    BOOL success = DeviceIoControl(glbhDevice, IOCTL_ADD_INIT_OUTPUT_BUFFERS, &pOverLapped->AddOutputBuffer, sizeof(ADD_OUTPUT_BUFFERS),
			&pOverLapped->BafInfo, sizeof(OBAF_INFO), &bytesRet, &pOverLapped->Overlapped);

    if (success == FALSE)
    {
        ULONG status = GetLastError();
        if (status != ERROR_IO_PENDING)
        {
            DbgPrint(_T("AHAAddInitOutput: Failed to queue add output buffer request. Status: %i\n"), status);
            return status;
        }
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHAAddMoreInput
** PURPOSE: 
** 
**	This function sets up the Add Input parameters and then issues the 
**	IOCTL_ADD_MORE_INPUT_BUFFERS to the device.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAAddMoreInput(IN POVERLAPPED_EX pOverLapped, IN ULONG SglFlags, bool& eof) 
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}

    AHASetupAddInput(pOverLapped, &pOverLapped->AddInput, SglFlags, eof);

	ULONG status = ERROR_SUCCESS;

	const ULONG maxResourceRetry = 10000;
    ULONG resourceRetry = maxResourceRetry;
    do
    {
        // Queue the second request
        ULONG bytesRet = 0;
        BOOL success = DeviceIoControl(glbhDevice, 
                                IOCTL_ADD_MORE_INPUT_BUFFERS,
                                &pOverLapped->AddInput, 
                                sizeof(ADD_INPUT_BUFFERS),
                                NULL,
                                0,
                                &bytesRet,
                                &pOverLapped->Overlapped);
    
        if (success == FALSE)
        {
            status = GetLastError();
            if (status != ERROR_IO_PENDING && status != ERROR_NO_SYSTEM_RESOURCES)
            {
                DbgPrint(_T("AHAAddMoreInput: Failed to queue add more input buffers. Status: %i\n"), status);
                return status;
            }

            if (status == ERROR_NO_SYSTEM_RESOURCES)
            {
                --resourceRetry;

                if (resourceRetry > 0)
                {
                    Sleep(5);
                }
                if (resourceRetry % 10 == 0)
                {
                    DbgPrint(_T("AHAAddMoreInput: Failed %i consecutive attempts.\n"), maxResourceRetry - resourceRetry);
                }
            }
        }
    }
    while(status == ERROR_NO_SYSTEM_RESOURCES && resourceRetry > 0);

    if (resourceRetry == 0)
    {
        DbgPrint(_T("AHAAddMoreInput: Failed to waiting for input resources. Status: %i\n"), status);
        return status;
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHAAddMoreOutput
** PURPOSE: 
** 
**	This function sets up the Add Output parameters and then issues the 
**	IOCTL_ADD_MORE_OUTPUT_BUFFERS to the device.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAAddMoreOutput(IN BOOLEAN InterruptWhenFull, IN POVERLAPPED_EX pOverLapped, bool& eof) 
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}

    // Set up the Add Output parameters
	eof = false;
    AHASetupAddOutput(pOverLapped, InterruptWhenFull, &pOverLapped->AddOutputBuffer, eof);    

    // Queue the ADD OUTPUT request
    ULONG bytesRet = 0;
    BOOL success = DeviceIoControl(glbhDevice, 
                                IOCTL_ADD_MORE_OUTPUT_BUFFERS,
                                &pOverLapped->AddOutputBuffer, 
                                sizeof(ADD_OUTPUT_BUFFERS),
                                &pOverLapped->BafInfo,
                                sizeof(OBAF_INFO),
                                &bytesRet,
                                &pOverLapped->Overlapped);
    if (success == FALSE)
    {
        ULONG status = GetLastError();
        if (status != ERROR_IO_PENDING)
        {
            DbgPrint(_T("AHAAddMoreOutput: Failed to queue add output buffer request. Status: %i\n"), status);
            return status;
        }
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHAStartStrm
** PURPOSE: 
** 
**	This function sets up the Add Output parameters and then issues the 
**	IOCTL_ADD_INIT_OUTPUT_BUFFERS to the device.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAStartStrm()
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}

    ULONG status = ERROR_SUCCESS;

	const ULONG maxResourceRetry = 10000;
    ULONG resourceRetry = maxResourceRetry;
    do
    {
        // Queue the second request
        ULONG bytesRet = 0;
        BOOL success = DeviceIoControl(glbhDevice, 
                                IOCTL_START_STREAM,
                                NULL, 
                                0,
                                NULL,
                                0,
                                &bytesRet,
                                NULL);
    
        if (success == FALSE)
        {
            status = GetLastError();

            if (status != ERROR_IO_PENDING && status != ERROR_NO_SYSTEM_RESOURCES)
            {
                DbgPrint(_T("AHAStrmStart: Failed to start the stream. Status: %i\n"), status);
                return status;
            }

            if (status == ERROR_NO_SYSTEM_RESOURCES)
            {
                --resourceRetry;

                if (resourceRetry > 0)
                {
                    Sleep(5);
                }
                if (resourceRetry % 10 == 0)
                {
                    DbgPrint(_T("AHAStartStrm: Failed %i consecutive attempts.\n"), maxResourceRetry - resourceRetry);
                }
            }
        }
    }
    while(status == ERROR_NO_SYSTEM_RESOURCES && resourceRetry > 0);

    if (resourceRetry == 0)
    {
        DbgPrint(_T("AHAStrmStart: Failed waiting for resources. Status: %i\n"), status);
        return status;
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHATerminateThread
**	
** PURPOSE: This function is used to terminate the async receive thread.
** 
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
BOOL AHA::CAHA363::AHATerminateThread()
{
    // Wait for the outstanding requests to complete
    ULONG waitCount = 1000;
    while ( glbOutstandingOutputRequests != 0 && 
            glbOutstandingInputRequests != 0 && 
            waitCount > 0)
    {
        Sleep(10);
        --waitCount;
    }

    if (waitCount == 0 && 
        (glbOutstandingOutputRequests != 0 || glbOutstandingInputRequests != 0))
    {
        DbgPrint(_T("AHATerminateThread: Timed out waiting for outstanding requests to complete. Outstanding input requests: %i. Outstanding output requests: %i.\n"),
                   glbOutstandingInputRequests, glbOutstandingOutputRequests);

        _ASSERTE(glbOutstandingOutputRequests == 0 && glbOutstandingInputRequests == 0);
    }
    
    //  Tell the thread to cancel itself.
    ULONG_PTR compKey = -1;
    BOOL success = PostQueuedCompletionStatus(glbhCompletionPort, 0, compKey, NULL);
    if (success == FALSE)
    {
        ULONG status = GetLastError();
        DbgPrint(_T("AHATerminateThread: PostQueuedCompletionStatus Failed. Status: %i\n"), status);
        return FALSE;
    }

    // Wait for the thread to terminate
    DWORD status = WaitForSingleObject(glbhReceiveThread, THREAD_WAIT_TIME);
    if (status == WAIT_TIMEOUT)
    {
        DbgPrint(_T("AHATerminateThread: Failed to terminate the receive thread\n"));        
		return FALSE;
    }

    return TRUE;
}

/****************************************************************************
**
** AHAUpdateOutputFile
** PURPOSE: 
** 
**	This function is used to setup the start stream parameters.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
ULONG AHA::CAHA363::AHAUpdateOutput(IN POVERLAPPED_EX pOverLapped)
{
	if (!IsBoardInit())
	{
		return ERROR_BAD_DEVICE;
	}

    ULONG64 bytesToWrite = 0;

    POBAF_INFO pObafInfo = &pOverLapped->BafInfo;

    if (pOverLapped->Index != glbSeqRevd)
    {
        DbgPrint(_T("AHAUpdateOutputFile: Rcv sequence number: %i. Expected: %i.\n"), pOverLapped->Index, glbSeqRevd);

        if (pObafInfo->OutputDataSize == 0)
        {
            bytesToWrite = 0;
        }
        else
        {
            // Calculate the output size based on what the output ought to be.
            bytesToWrite = pObafInfo->OutputDataSize - (pOverLapped->Index-1) * glbDmaBlockSize;
        }
    }
    else
    {
        // Update the received seq number
        ++glbSeqRevd;

        if (pObafInfo->OutputDataSize == 0)
        {
            bytesToWrite = 0;
        }
        else
        {
            // Calculate the amount of data complete since the last time
            bytesToWrite = pObafInfo->OutputDataSize - glbDataOutputSize;
        }
    }

    if (pObafInfo->OutputDataSize == 0 || bytesToWrite == 0)
    {
        return ERROR_SUCCESS;
    }

    if (bytesToWrite > glbDmaBlockSize)
    {
        DbgPrint(_T("AHAUpdateOutputFile: Bytes to write exceed dma block size. BytesToWrite: %i. OutputDataSize: %i. Total OutputDataSize: %i.\n"), 
                    bytesToWrite, pObafInfo->OutputDataSize, glbDataOutputSize);

        return ERROR_INVALID_BLOCK;
    }

    // Update the data output size
    glbDataOutputSize += bytesToWrite;
    if (glbDataOutputSize != pObafInfo->OutputDataSize)
    {
        DbgPrint(_T("AHAUpdateOutputFile: Total output does not match current. Current: %i. Total Output: %i.\n"), pObafInfo->OutputDataSize, glbDataOutputSize);
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
**
** AHASetupBuildStream
** PURPOSE: 
** 
**	This function is used to setup the build stream parameters.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
VOID AHA::CAHA363::AHASetupBuildStream(IN POVERLAPPED_EX pOverLapped, IN ULONG StrmFlags, IN ULONG SglFlags, bool& eof)
{
    pOverLapped->RequestType = BUILD_STRM;

    PBUILD_STREAM pBuildStream = &pOverLapped->BuildStream;
    pBuildStream->Operation = Compression;
    pBuildStream->DmaChannel = glbDMAChannel;
    pBuildStream->SglFlags.u.Value = SglFlags;
    pBuildStream->StrmFlags.u.Value = StrmFlags;
    pBuildStream->StrmOps.u.bits.ZLib = glbZLib;
    pBuildStream->DmaBlockSize = glbDmaBlockSize;
    pBuildStream->ContextSize = 0;	

	DWORD dwBufferOffset = 0;
	DWORD dwBufferRemaining = 0;
	eof = false;
	GetInBufferParams(m_dwInBufferSize, pOverLapped->Index, glbDmaBlockSize, dwBufferOffset, dwBufferRemaining, pBuildStream->SglFlags, eof);

    pBuildStream->InBufferLength = dwBufferRemaining;	

    if (pBuildStream->InBufferLength == 0)
    {
        pBuildStream->pInBuffer	= 0;
    }
    else
    {
		pBuildStream->pInBuffer = (__int64) (m_pbyInBuffer + dwBufferOffset);
    }    
}

/****************************************************************************
**
** AHASetupAddInput
** PURPOSE: 
** 
**	This function is used to setup the Add Input stream parameters.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
VOID AHA::CAHA363::AHASetupAddInput(IN POVERLAPPED_EX pOverLapped, IN PADD_INPUT_BUFFERS pAddInput, IN ULONG SglFlags, bool& eof)
{
    pOverLapped->RequestType = ADD_INPUT;
    pAddInput->SglFlags.u.Value = SglFlags;

	DWORD dwBufferOffset = 0;
	DWORD dwBufferRemaining = 0;
	eof = false;
	GetInBufferParams(m_dwInBufferSize, pOverLapped->Index, glbDmaBlockSize, dwBufferOffset, dwBufferRemaining, pAddInput->SglFlags, eof);

	pAddInput->InBufferLength = dwBufferRemaining;

	if (pAddInput->InBufferLength == 0)
	{
		pAddInput->pInBuffer = 0;
	}
	else
	{
		pAddInput->pInBuffer = (__int64) (m_pbyInBuffer + dwBufferOffset);
	}
    
}

/****************************************************************************
**
** AHASetupAddOutput
** PURPOSE: 
** 
**	This function is used to setup the Add output stream parameters.
**
** RETURNS: 
**          
** NOTES:   
**
*****************************************************************************/
VOID AHA::CAHA363::AHASetupAddOutput(IN POVERLAPPED_EX pOverLapped,  IN BOOLEAN InterruptWhenFull, IN PADD_OUTPUT_BUFFERS pAddOutput, bool& eof)
{
    pOverLapped->RequestType = ADD_OUTPUT;

    pAddOutput->InterruptWhenFull = InterruptWhenFull;

	DWORD dwBufferOffset = 0;
	DWORD dwBufferRemaining = 0;
	eof = false;
	GetOutBufferParams(m_dwOutBufferSize, pOverLapped->Index, glbDmaBlockSize, dwBufferOffset, dwBufferRemaining, eof);

	if (dwBufferRemaining > 0)
	{
		pAddOutput->OutBufferLength = dwBufferRemaining;
		pAddOutput->pOutBuffer = (__int64) (m_pbyOutBuffer + dwBufferOffset);
	}
	else
	{
		pAddOutput->OutBufferLength = overflowBuffer.size();
		pAddOutput->pOutBuffer = (__int64) &overflowBuffer[0];

		glbFinalOpStatus = ERROR_NOT_ENOUGH_MEMORY;
	}
}

bool AHA::CAHA363::GetTemps(DWORD& dwFPGA, DWORD& dwBoard)
{
	if (!IsBoardInit())
	{
		return false;
	}

	DWORD dwTemp = 0;
	ULONG bytesRet = 0;
	BOOL status = DeviceIoControl(glbhDevice, IOCTL_QUERY_TEMP_REG, NULL, 0, &dwTemp, sizeof(dwTemp), &bytesRet, NULL);
	if (status == TRUE)
	{
		dwFPGA = (dwTemp >> 8) & 0xff;
		dwBoard = dwTemp & 0xff;
	}

	return status == TRUE;
}

bool AHA::CAHA363::GetStats(STATS& stats)
{
	if (!IsBoardInit())
	{
		return false;
	}

	memset(&stats, 0, sizeof(stats));

	ULONG bytesRet = 0;
	BOOL status = DeviceIoControl(glbhDevice, IOCTL_QUERY_STREAM_STATS, NULL, 0, &stats, sizeof(stats), &bytesRet, NULL);
	return status == TRUE && bytesRet > 0;
}

bool AHA::CAHA363::ClearStats()
{
	if (!IsBoardInit())
	{
		return false;
	}

	ULONG bytesRet = 0;
	BOOL status = DeviceIoControl(glbhDevice, IOCTL_CLEAR_STREAM_STATS, NULL, 0, NULL, 0, &bytesRet, NULL);
	return status == TRUE;
}

void AHA::CAHA363::GetInBufferParams(DWORD dwInBufferLength, DWORD index, DWORD dmaBlockSize, DWORD& dwBufferOffset, DWORD& dwBufferRemaining, SGL_FLAGS& sglFlags, bool& eof)
{
	// calculate the offset into the buffer
	dwBufferOffset = index * dmaBlockSize;

	// set the start flag
	sglFlags.u.bits.Fs = (dwBufferOffset == 0 ? 1 : 0);

	// calculate the number of bytes remaining and the end flag
	dwBufferRemaining = dwInBufferLength - dwBufferOffset;
	if (dwBufferRemaining <= dmaBlockSize)
	{
		eof = true;
		sglFlags.u.bits.Fe = 1;		
	}	
	else
	{
		eof = false;
		sglFlags.u.bits.Fe = 0;
		dwBufferRemaining = dmaBlockSize;
	}

	// if there has been an error elsewhere then we want to terminate the input
	if (glbFinalOpStatus != ERROR_SUCCESS)
	{
		eof = true;
		sglFlags.u.bits.Fe = 1;
	}
}

void AHA::CAHA363::GetOutBufferParams(DWORD dwOutBufferLength, DWORD index, DWORD dmaBlockSize, DWORD& dwBufferOffset, DWORD& dwBufferRemaining, bool& eof)
{
	// calculate the offset into the buffer
	dwBufferOffset = index * dmaBlockSize;

	if (dwBufferOffset < dwOutBufferLength)
	{
		// calculate the number of bytes remaining and the end flag
		dwBufferRemaining = dwOutBufferLength - dwBufferOffset;
		if (dwBufferRemaining <= dmaBlockSize)
		{
			eof = true;
		}	
		else
		{
			eof = false;
			dwBufferRemaining = dmaBlockSize;
		}
	}
	else
	{
		eof = true;
		dwBufferRemaining = 0;
	}
}