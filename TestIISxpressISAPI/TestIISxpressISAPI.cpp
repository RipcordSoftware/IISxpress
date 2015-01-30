// This is the main DLL file.

#include "stdafx.h"

#include "TestIISxpressISAPI.h"

#include <algorithm>

TestIISxpressISAPI::TestIISxpressISAPI::TestIISxpressISAPI() : m_hIISxpress(NULL), m_pfnHttpFilterProc(NULL), m_pfnGetFilterVersion(NULL)
{
	m_hIISxpress = ::LoadLibrary(L"IISxpress.dll");	
	if (m_hIISxpress != NULL)
	{
		m_pfnGetFilterVersion = (PFNGETFILTERVERSION) ::GetProcAddress(m_hIISxpress, "GetFilterVersion");	
		m_pfnHttpFilterProc = (PFNHTTPFILTERPROC) ::GetProcAddress(m_hIISxpress, "HttpFilterProc");	
	}
}

TestIISxpressISAPI::TestIISxpressISAPI::~TestIISxpressISAPI()
{
	if (m_hIISxpress != NULL)
	{		
		m_pfnGetFilterVersion = NULL;
		m_pfnHttpFilterProc = NULL;
		::FreeLibrary(m_hIISxpress);
		m_hIISxpress = NULL;
	}
}

void TestIISxpressISAPI::TestIISxpressISAPI::Init()
{
	IsNotNull(m_hIISxpress);
	IsNotNull(m_pfnGetFilterVersion);	
	IsNotNull(m_pfnHttpFilterProc);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TearDown()
{
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestFilterVersion()
{
	HTTP_FILTER_VERSION version;
	memset(&version, 0, sizeof(version));
	version.dwServerFilterVersion = 1;

	BOOL status = m_pfnGetFilterVersion(&version);

	Assert::IsTrue(status ? true : false);

	DWORD dwFlags = SF_NOTIFY_URL_MAP | SF_NOTIFY_SEND_RESPONSE | 
		SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST | SF_NOTIFY_END_OF_NET_SESSION | 
		SF_NOTIFY_ORDER_LOW;

	Assert::AreEqual((int) version.dwFlags, (int) dwFlags);

	Assert::AreEqual(strcmp(version.lpszFilterDesc, "IISxpress HTTP Compression Filter"), 0);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseRFC2616()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\rfc2616.txt";
	data.sContentType = "text/plain";
	data.sURL = "/rfc2616.txt";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseRFC2616_WithCache()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\rfc2616.txt";
	data.sContentType = "text/plain";
	data.sURL = "/rfc2616.txt";
	data.sLastModified = "01-01-01";
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseRFC2616_NoContentLength()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\rfc2616.txt";
	data.sContentType = "text/plain";
	data.sURL = "/rfc2616.txt";
	data.bOmitContentLengthFromHeader = true;
	data.sConnection = "close";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponse_Threads()
{
	array<System::Threading::Thread^>^ threads = gcnew array<System::Threading::Thread^>(8);
	for (int i = 0; i < threads->Length; i++)
	{
		System::Threading::ThreadStart^ threadStart = gcnew System::Threading::ThreadStart(this, &TestIISxpressISAPI::TestResponse_Threads_Worker);
		threads[i] = gcnew System::Threading::Thread(threadStart);
	}

	for (int i = 0; i < threads->Length; i++)
	{
		threads[i]->Start();
	}

	System::Threading::Thread::Sleep(1000);

	int jobs = threads->Length;
	while (jobs < 1000)
	{		
		for (int i = 0; i < threads->Length; i++)
		{		
			if (threads[i]->Join(0))
			{
				System::Threading::ThreadStart^ threadStart = gcnew System::Threading::ThreadStart(this, &TestIISxpressISAPI::TestResponse_Threads_Worker);
				threads[i] = gcnew System::Threading::Thread(threadStart);
				threads[i]->Start();
				jobs++;
			}
		}
	}

	// wait until all the threads are finished
	for (int i = 0; i < threads->Length; i++)
	{		
		threads[i]->Join();
	}
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponse_Threads_Worker()
{		
	System::DateTime^ now = System::DateTime::Now;
	__int64 select = now->Ticks % 11;

	switch (select)
	{
	case 0:	TestResponseDefaultCSS(); break;
	case 1: TestResponseDefaultCSS_WithCache(); break;
	case 2: TestResponseDefaultCSS_InlineHeader(); break;
	case 3: TestResponseDefaultCSS_InlineHeaderWithCache(); break;
	case 4: TestResponseDefaultCSS_InlineHeaderSmallBlock(); break;
	case 5: TestResponseDefaultCSS_InlineHeaderSmallBlockWithCache(); break;
	case 6: TestResponseDefaultCSS_GZIP(); break;
	case 7: TestResponseRFC2616(); break;
	case 8: TestResponseRFC2616_WithCache(); break;
	case 9: TestResponse_Selection(); break;
	case 10: TestResponse_SelectionWithCache(); break;
	}
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_WithCache()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.sLastModified = "01-01-01";
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_InlineHeader()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.bPutHeaderInFirstBlock = true;	
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_InlineHeaderWithCache()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.sLastModified = "01-01-01";
	data.bPutHeaderInFirstBlock = true;	
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_InlineHeaderSmallBlock()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.bPutHeaderInFirstBlock = true;
	data.nDefaultBlockSize = 2048;
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_InlineHeaderSmallBlockWithCache()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.sLastModified = "01-01-01";
	data.bPutHeaderInFirstBlock = true;
	data.nDefaultBlockSize = 2048;
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_GZIP()
{
	TestResponseData data;
	data.sAcceptEncoding = "gzip";
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_NoAcceptEnc()
{
	TestResponseData data;
	data.sAcceptEncoding = "";
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_POST()
{
	TestResponseData data;
	data.sMethod = "POST";
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_Chunked()
{
	TestResponseData data;
	data.sTransferEncoding = "chunked";
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_AlreadyGZIP()
{
	TestResponseData data;
	data.sContentEncoding = "gzip";
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_NoContentLength()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.sConnection = "close";
	data.bOmitContentLengthFromHeader = true;
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_NoContentLengthInlineHeader()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.sConnection = "close";
	data.bOmitContentLengthFromHeader = true;
	data.bPutHeaderInFirstBlock = true;
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponseDefaultCSS_NoContentLengthNoClose()
{
	TestResponseData data;
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\default.css";
	data.sContentType = "text/css";
	data.sURL = "/default.css";
	data.bOmitContentLengthFromHeader = true;
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponse_Selection()
{
	TestResponseData data;	
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\decl.txt";
	data.sContentType = "text/plain";
	data.sURL = "/decl.txt";
	TestResponse(data);

	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\gpl.txt";
	data.sContentType = "text/plain";
	data.sURL = "/gpl.txt";
	TestResponse(data);
	
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\526.txt";
	data.sContentType = "text/plain";
	data.sURL = "/526.txt";
	TestResponse(data);
	
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\rfc2616.txt";
	data.sContentType = "text/plain";
	data.sURL = "/rfc2616.txt";
	TestResponse(data);

	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\JSB_BWV1047_3.xml";
	data.sContentType = "text/xml";
	data.sURL = "/JSB_BWV1047_3.xml";
	TestResponse(data);

	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\wrnpc11.txt";
	data.sContentType = "text/plain";
	data.sURL = "/wrnpc11.txt";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponse_SelectionWithCache()
{
	TestResponseData data;	
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\decl.txt";
	data.sContentType = "text/plain";
	data.sURL = "/decl.txt";
	data.sLastModified = "01-01-01";
	TestResponse(data);

	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\gpl.txt";
	data.sContentType = "text/plain";
	data.sURL = "/gpl.txt";		
	TestResponse(data);
	
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\526.txt";
	data.sContentType = "text/plain";
	data.sURL = "/526.txt";
	TestResponse(data);
	
	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\rfc2616.txt";
	data.sContentType = "text/plain";
	data.sURL = "/rfc2616.txt";
	TestResponse(data);

	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\JSB_BWV1047_3.xml";
	data.sContentType = "text/xml";
	data.sURL = "/JSB_BWV1047_3.xml";
	TestResponse(data);

	data.sFileName = "c:\\inetpub\\wwwroot\\TestIISxpressISAPI\\wrnpc11.txt";
	data.sContentType = "text/plain";
	data.sURL = "/wrnpc11.txt";
	TestResponse(data);
}

void TestIISxpressISAPI::TestIISxpressISAPI::TestResponse(const TestResponseData& responseData)
{
	// read from the source file	
	AutoArray<BYTE> pbyFileData;
	DWORD dwFileLength = 0;	
	bool status = CISAPITestHelper::LoadFileData(responseData.sFileName, pbyFileData, dwFileLength);
	Assert::AreEqual(status, true, String::Format("The file {0} could not be found", gcnew System::String(responseData.sFileName)));

	HTTP_FILTER_CONTEXT context;
	memset(&context, 0, sizeof(context));
	context.cbSize = sizeof(context);

	CISAPITestHelper helper(context);	

	char szPhysicalPath[512] = "\0";
	strcpy_s(szPhysicalPath, responseData.sFileName);

	char szFullURL[2048] = "\0";
	strcpy_s(szFullURL, "http://");
	strcat_s(szFullURL, responseData.sHostName);
	strcat_s(szFullURL, responseData.sURL);

	HTTP_FILTER_URL_MAP UrlMap;
	memset(&UrlMap, 0, sizeof(UrlMap));
	UrlMap.pszURL = szFullURL;
	UrlMap.pszPhysicalPath = szPhysicalPath;
	UrlMap.cbPathBuff = sizeof(szPhysicalPath);

	// ******************************************************************************************
	helper.SetDisableNotificationsFlags(&context);
	DWORD dwStatus = m_pfnHttpFilterProc(&context, SF_NOTIFY_URL_MAP, &UrlMap);
	DWORD dwDisableNotifications = helper.GetDisableNotificationsFlags(&context);

	// we expect the filter chain to be able to continue
	if (dwStatus != SF_STATUS_REQ_NEXT_NOTIFICATION)
	{
		throw gcnew UnexpectedResponseFromFilterException();
	}

	// we expect the filter to ask for no more URL_MAP notifications
	if (dwDisableNotifications != SF_NOTIFY_URL_MAP)
	{
		throw gcnew UnexpectedResponseFromFilterException();
	}

	HTTP_FILTER_SEND_RESPONSE response;
	memset(&response, 0, sizeof(response));
	response.HttpStatus = responseData.nResponseCode;
	CISAPITestHelper::Attach(response);	

	helper.SetServerVariable("HTTP_METHOD", responseData.sMethod);	
	helper.SetServerVariable("HTTP_ACCEPT_ENCODING", responseData.sAcceptEncoding);
	helper.SetServerVariable("HTTP_TRANSFER_ENCODING", responseData.sTransferEncoding);
	helper.SetServerVariable("REMOTE_ADDR", responseData.sClientIP);
	helper.SetServerVariable("SERVER_NAME", responseData.sHostName);
	helper.SetServerVariable("SERVER_PORT", "80");
	helper.SetServerVariable("INSTANCE_ID", "1");
	helper.SetServerVariable("HTTP_USER_AGENT", responseData.sUserAgent);
	helper.SetServerVariable("URL", responseData.sURL);
	helper.SetServerVariable("HTTP_HOST", responseData.sHostName);
	helper.SetServerVariable("QUERY_STRING", responseData.sQueryString);

	CISAPITestHelper::AddHeader(&context, "Content-Type:", responseData.sContentType);

	CStringA sContentLength;	
	if (responseData.bOmitContentLengthFromHeader == false)
	{		
		CStringA sFileLength;
		sFileLength.Format("%u", dwFileLength);

		CISAPITestHelper::AddHeader(&context, "Content-Length:", sFileLength);

		sContentLength.Format("Content-Length: %u\r\n", dwFileLength);
	}

	if (responseData.sConnection.GetLength() > 0)
	{
		CISAPITestHelper::AddHeader(&context, "Connection:", responseData.sConnection);
	}

	if (responseData.sLastModified.GetLength() > 0)
	{
		CISAPITestHelper::AddHeader(&context, "Last-Modified:", responseData.sLastModified);
	}

	if (responseData.sTransferEncoding.GetLength() > 0)
	{
		CISAPITestHelper::AddHeader(&context, "Transfer-Encoding:", responseData.sTransferEncoding);
	}

	if (responseData.sContentEncoding.GetLength() > 0)
	{
		CISAPITestHelper::AddHeader(&context, "Content-Encoding:", responseData.sContentEncoding);
	}

	// ******************************************************************************************
	helper.SetDisableNotificationsFlags(&context);
	dwStatus = m_pfnHttpFilterProc(&context, SF_NOTIFY_SEND_RESPONSE, &response);
	dwDisableNotifications = helper.GetDisableNotificationsFlags(&context);

	// we expect the filter chain to be able to continue
	if (dwStatus != SF_STATUS_REQ_NEXT_NOTIFICATION)
	{
		throw gcnew UnexpectedResponseFromFilterException();
	}

	// if the filter has asked for no more RAW_DATA or END_OR_REQUEST notifications then it doesn't want to handle the request
	if (dwDisableNotifications == (SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST))
	{
		throw gcnew FilterWillNotHandleResponseException();		
	}

	// if the filter has disabled any other notifications then we want to know about it
	if (dwDisableNotifications != 0)
	{
		throw gcnew UnexpectedResponseFromFilterException();
	}

	CStringA sTransferEncoding;
	if (responseData.sTransferEncoding.GetLength() > 0)
	{
		sTransferEncoding = "Transfer-Encoding: ";
		sTransferEncoding += responseData.sTransferEncoding;
		sTransferEncoding += "\r\n";
	}

	CStringA sContentEncoding;
	if (responseData.sContentEncoding.GetLength() > 0)
	{
		sContentEncoding = "Content-Encoding: ";
		sContentEncoding += responseData.sContentEncoding;
		sContentEncoding += "\r\n";
	}

	CStringA sConnection;
	if (responseData.sConnection.GetLength() > 0)
	{
		sConnection = "Connection: ";
		sConnection += responseData.sConnection;
		sConnection += "\r\n";
	}

	CStringA sHeader;
	sHeader.Format(
		"HTTP/1.1 %d OK\r\n"
		"Content-Type: %s\r\n"
		"%s"
		"%s"
		"%s"
		"%s"
		"\r\n",
		responseData.nResponseCode, responseData.sContentType, 
		sContentLength,
		sTransferEncoding,
		sContentEncoding,
		sConnection);
	
	AutoArray<char> pszHeaderData(sHeader.GetLength());
	memcpy(pszHeaderData, sHeader, sHeader.GetLength());

	HTTP_FILTER_RAW_DATA data;

	bool bGotCacheHit = false;
	if (responseData.bPutHeaderInFirstBlock == false)
	{		
		data.pvInData = pszHeaderData;
		data.cbInData = sHeader.GetLength();
		data.cbInBuffer = sHeader.GetLength();
		data.dwReserved = 0;

		// ******************************************************************************************
		helper.SetDisableNotificationsFlags(&context);
		dwStatus = m_pfnHttpFilterProc(&context, SF_NOTIFY_SEND_RAW_DATA, &data);
		dwDisableNotifications = helper.GetDisableNotificationsFlags(&context);

		if (responseData.sLastModified.GetLength() > 0 && dwStatus == SF_STATUS_REQ_FINISHED_KEEP_CONN && dwDisableNotifications == (SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST))
		{
			bGotCacheHit = true;
		}
		else
		{
			// we expect the filter chain to be able to continue
			if (dwStatus != SF_STATUS_REQ_NEXT_NOTIFICATION)
			{
				throw gcnew UnexpectedResponseFromFilterException();
			}

			// if the filter has asked for no more RAW_DATA or END_OF_REQUEST notifications then it doesn't want to handle the request
			if (dwDisableNotifications == (SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST))
			{
				throw gcnew FilterWillNotHandleResponseException();
			}

			// if the filter has disabled any other notifications then we want to know about it
			if (dwDisableNotifications != 0)
			{
				throw gcnew UnexpectedResponseFromFilterException();
			}
		}
	}
	
	AutoArray<BYTE> pszISAPIBuffer(responseData.nDefaultBlockSize);
	if (bGotCacheHit == false)
	{	
		int nBlocks = dwFileLength / responseData.nDefaultBlockSize;
		nBlocks += ((dwFileLength % responseData.nDefaultBlockSize) > 0 ? 1 : 0);

		int nDataOffset = 0;
		for (int i = 0; i < nBlocks; i++)
		{
			int nHeaderBlockSize = 0;
			int nDataBlockSize = responseData.nDefaultBlockSize;		

			// shrink the size of the block if required
			if ((int) (dwFileLength - nDataOffset) < nDataBlockSize)
			{
				nDataBlockSize = dwFileLength - nDataOffset;
			}

			int nTotalBlockSize = nDataBlockSize;

			// copy in the header if required
			if (i == 0 && responseData.bPutHeaderInFirstBlock == true)
			{
				nHeaderBlockSize = sHeader.GetLength();

				// adjust the size of the data block
				nTotalBlockSize = nHeaderBlockSize + nDataBlockSize;
				if (nTotalBlockSize > responseData.nDefaultBlockSize)
				{
					nDataBlockSize -= (nTotalBlockSize - responseData.nDefaultBlockSize);
					nTotalBlockSize = responseData.nDefaultBlockSize;
					
					// figure out whether we have overflowed the final block
					int nBlockOverflow = (dwFileLength % responseData.nDefaultBlockSize);
					nBlockOverflow += nHeaderBlockSize;
					nBlocks += (nBlockOverflow / responseData.nDefaultBlockSize);
				}

				// copy the header into the block
				memcpy(pszISAPIBuffer, sHeader, nHeaderBlockSize);			
			}

			// copy in the body
			memcpy(pszISAPIBuffer + nHeaderBlockSize, pbyFileData + nDataOffset, nDataBlockSize);
			
			data.pvInData = pszISAPIBuffer;
			data.cbInData = nTotalBlockSize;
			data.cbInBuffer = nTotalBlockSize;
			data.dwReserved = 0;

			// ******************************************************************************************
			helper.SetDisableNotificationsFlags(&context);
			dwStatus = m_pfnHttpFilterProc(&context, SF_NOTIFY_SEND_RAW_DATA, &data);
			dwDisableNotifications = helper.GetDisableNotificationsFlags(&context);

			if (i == 0 && responseData.sLastModified.GetLength() > 0 && dwStatus == SF_STATUS_REQ_FINISHED_KEEP_CONN && dwDisableNotifications == (SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST))
			{
				bGotCacheHit = true;
				break;
			}

			// we expect the filter chain to be able to continue
			if (dwStatus != SF_STATUS_REQ_NEXT_NOTIFICATION)
			{
				throw gcnew UnexpectedResponseFromFilterException();
			}

			if (i == 0 && responseData.bPutHeaderInFirstBlock == true)
			{				
				// if the filter has asked for no more RAW_DATA or END_OF_REQUEST notifications then it doesn't want to handle the request
				if (dwDisableNotifications == (SF_NOTIFY_SEND_RAW_DATA | SF_NOTIFY_END_OF_REQUEST))
				{
					throw gcnew FilterWillNotHandleResponseException();
				}
			}

			// if the filter has disabled any other notifications then we want to know about it
			if (dwDisableNotifications != 0)
			{
				throw gcnew UnexpectedResponseFromFilterException();
			}

			nDataOffset += nDataBlockSize;
		}
	}

	// ******************************************************************************************	

	if (data.cbInData > 0)
	{
		ValidateResponseData(responseData, pbyFileData, dwFileLength, (BYTE*) data.pvInData, data.cbInData);

		helper.SetDisableNotificationsFlags(&context);
		dwStatus = m_pfnHttpFilterProc(&context, SF_NOTIFY_END_OF_REQUEST, NULL);
		dwDisableNotifications = helper.GetDisableNotificationsFlags(&context);
	}	
	else
	{
		helper.SetDisableNotificationsFlags(&context);
		dwStatus = m_pfnHttpFilterProc(&context, SF_NOTIFY_END_OF_REQUEST, NULL);
		dwDisableNotifications = helper.GetDisableNotificationsFlags(&context);

		std::vector<BYTE> writeResponse;
		CISAPITestHelper::GetWriteClientResponse(&context, writeResponse);		

		ValidateResponseData(responseData, pbyFileData, dwFileLength, (BYTE*) &writeResponse[0], writeResponse.size());
	}	
}

void TestIISxpressISAPI::TestIISxpressISAPI::ValidateResponseData(const TestResponseData& responseData, BYTE* pbyFileData, DWORD dwFileDataSize, BYTE* pbyResponseData, DWORD dwResponseDataSize)
{
	// check we have response data
	if (dwResponseDataSize == 0)
	{
		throw gcnew EmptyResponseException();
	}

	// get the end of the response header
	char* pEndOfData = (char*) pbyResponseData + dwResponseDataSize;
	char* pszHeaderEndToken = "\r\n\r\n";
	char* pszHeaderEnd = std::search((char*) pbyResponseData, pEndOfData, pszHeaderEndToken, pszHeaderEndToken + 4);
	if (pszHeaderEnd == pEndOfData)
	{
		throw gcnew ResponseHeaderMalformedException();
	}

	// terminate the header
	*pszHeaderEnd = '\0';

	// calculate the start of the response body
	const char* pszBodyStart = pszHeaderEnd + 4;

	const char* pszContentEncodingHeader = "Content-Encoding: ";
	const int nContentEncodingHeaderLength = strlen(pszContentEncodingHeader);

	// find the content encoding line in the header
	char* pszEncoding = strstr((char*) pbyResponseData, pszContentEncodingHeader);
	if (pszEncoding == NULL)
	{
		throw gcnew ContentEncodingMissingException();
	}

	// calculate the content encodng string offset
	char* pszEncodingType = pszEncoding + nContentEncodingHeaderLength;

	// make sure the content encoding matches the request
	int nEncodingMatch = strncmp(pszEncodingType, responseData.sAcceptEncoding, responseData.sAcceptEncoding.GetLength());
	if (nEncodingMatch != 0)
	{
		throw gcnew EncodingTypeMismatchException();
	}

	const char* pszContentLengthHeader = "Content-Length: ";
	const int nContentLengthHeaderLength = strlen(pszContentLengthHeader);

	// get the content length line
	char* pszContentLength = strstr((char*) pbyResponseData, pszContentLengthHeader);
	if (pszContentLength == NULL)
	{		
		throw gcnew ContentLengthMissingException();
	}

	// get the content length in the response
	DWORD dwContentLength = atoi(pszContentLength + nContentLengthHeaderLength);

	// allocate a buffer to hold the uncompressed response	
	AutoArray<BYTE> pbyUncompressedResponseData(dwFileDataSize);

	z_stream inflateStream;
	memset(&inflateStream, 0, sizeof(inflateStream));

	inflateStream.next_in = (BYTE*) pszBodyStart;
	inflateStream.avail_in = dwContentLength;
	inflateStream.next_out = pbyUncompressedResponseData;
	inflateStream.avail_out = dwFileDataSize;

	// calculate the window bits based on the content encoding
	int nWindowBits = -15;
	if (pszEncodingType[0] == 'g')
	{
		nWindowBits = 47;
	}

	// initialize ZLIB
	::inflateInit2(&inflateStream, nWindowBits);	

	// decompress the response
	int nInflateStatus = ::inflate(&inflateStream, Z_FINISH);
	if (nInflateStatus != Z_OK && nInflateStatus != Z_STREAM_END)
	{		
		throw gcnew CompressionStreamException();
	}

	::inflateEnd(&inflateStream);

	// match the response to the original data
	if (memcmp(pbyFileData, pbyUncompressedResponseData, dwFileDataSize) != 0)
	{
		throw gcnew ResponseDataMismatchException;
	}
}