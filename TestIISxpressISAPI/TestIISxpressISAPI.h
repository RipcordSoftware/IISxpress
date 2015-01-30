// TestIISxpressISAPI.h

#pragma once

#include <vector>

using namespace System;
using namespace NUnit::Framework;

extern "C" 
{
	typedef DWORD WINAPI FNHTTPFILTERPROC(PHTTP_FILTER_CONTEXT pfc, DWORD NotificationType, VOID* pvNotification);
	typedef FNHTTPFILTERPROC* PFNHTTPFILTERPROC;

	typedef BOOL WINAPI FNGETFILTERVERSION(PHTTP_FILTER_VERSION pVer);
	typedef FNGETFILTERVERSION* PFNGETFILTERVERSION;	
}

namespace TestIISxpressISAPI 
{
	struct TestResponseData
	{
		TestResponseData() : sMethod("GET"), sHostName("localhost"), sAcceptEncoding("deflate"), sClientIP("127.0.0.1"), sUserAgent("Mozilla/4"), 
			nDefaultBlockSize(8192), bPutHeaderInFirstBlock(false), bOmitContentLengthFromHeader(false), nResponseCode(200) {}

		CStringA	sMethod;
		CStringA	sFileName;
		CStringA	sContentType;
		CStringA	sHostName;
		CStringA	sURL;
		CStringA	sQueryString;
		CStringA	sAcceptEncoding;
		CStringA	sTransferEncoding;
		CStringA	sContentEncoding;
		CStringA	sClientIP;
		CStringA	sUserAgent;
		CStringA	sLastModified;
		CStringA	sConnection;
		int			nDefaultBlockSize;
		bool		bPutHeaderInFirstBlock;
		bool		bOmitContentLengthFromHeader;
		int			nResponseCode;
	};

	public ref class FilterWillNotHandleResponseException : System::ApplicationException
	{
	};

	public ref class UnexpectedResponseFromFilterException : System::ApplicationException
	{
	};

	public ref class CompressionStreamException : System::ApplicationException
	{
	};

	public ref class ContentLengthMissingException : System::ApplicationException
	{
	};

	public ref class ContentEncodingMissingException : System::ApplicationException
	{
	};

	public ref class EncodingTypeMismatchException : System::ApplicationException
	{
	};

	public ref class ResponseHeaderMalformedException : System::ApplicationException
	{
	};

	public ref class EmptyResponseException : System::ApplicationException
	{
	};

	public ref class ResponseDataMismatchException : System::ApplicationException
	{
	};

	[TestFixture]
	public ref class TestIISxpressISAPI
	{
	public:

		TestIISxpressISAPI();

		~TestIISxpressISAPI();

		[SetUp] void Init();

		[TearDown] void TearDown();

		[Test] void TestFilterVersion();

		[Test, Description("Gets rfc2616.txt")] 
		void TestResponseRFC2616();

		[Test, Description("Gets rfc2616.txt from the cache")] 
		void TestResponseRFC2616_WithCache();

		[Test]
		void TestResponseRFC2616_NoContentLength();

		[Test] 
		void TestResponseDefaultCSS();

		[Test] 
		void TestResponseDefaultCSS_WithCache();

		[Test] 
		void TestResponseDefaultCSS_InlineHeader();

		[Test] 
		void TestResponseDefaultCSS_InlineHeaderWithCache();

		[Test] 
		void TestResponseDefaultCSS_InlineHeaderSmallBlock();

		[Test] 
		void TestResponseDefaultCSS_InlineHeaderSmallBlockWithCache();

		[Test] 
		void TestResponse_Threads();

		[Test] 
		void TestResponseDefaultCSS_GZIP();

		[Test, ExpectedException(FilterWillNotHandleResponseException::typeid)] 
		void TestResponseDefaultCSS_NoAcceptEnc();

		[Test, ExpectedException(FilterWillNotHandleResponseException::typeid)] 
		void TestResponseDefaultCSS_POST();

		[Test, ExpectedException(FilterWillNotHandleResponseException::typeid)] 
		void TestResponseDefaultCSS_Chunked();

		[Test, ExpectedException(FilterWillNotHandleResponseException::typeid)] 
		void TestResponseDefaultCSS_AlreadyGZIP();

		[Test]
		void TestResponseDefaultCSS_NoContentLength();

		[Test]
		void TestResponseDefaultCSS_NoContentLengthInlineHeader();

		[Test, ExpectedException(FilterWillNotHandleResponseException::typeid)]
		void TestResponseDefaultCSS_NoContentLengthNoClose();

		[Test]
		void TestResponse_Selection();

		[Test]
		void TestResponse_SelectionWithCache();

	private:

		template<typename T> void IsNotNull(const T t)
		{
			Assert::IsNotNull(t != NULL ? gcnew Object() : nullptr);
		}

		template<typename T> void IsNull(const T t)
		{
			Assert::IsNull(t != NULL ? gcnew Object() : nullptr);
		}

		void TestResponse(const TestResponseData& responseData);

		void ValidateResponseData(const TestResponseData& responseData, BYTE* pbyFileData, DWORD dwFileDataSize, BYTE* pbyResponseData, DWORD dwResponseDataSize);

		void TestResponse_Threads_Worker();

		HMODULE						m_hIISxpress;
		PFNHTTPFILTERPROC			m_pfnHttpFilterProc;		
		PFNGETFILTERVERSION			m_pfnGetFilterVersion;			
	};
}
