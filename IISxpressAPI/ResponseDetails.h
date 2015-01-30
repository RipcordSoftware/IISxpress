#pragma once

namespace IISxpressAPI
{	
	ref class ResponseDetails;

	private ref class ResponseDetailHolder
	{	
	public:
		System::DateTime^	time;
		System::String^		serverName;
		System::String^		serverPort;
		System::String^		instanceId;
		System::String^		URLMapPath;
		System::String^		userAgent;
		System::String^		hostname;
		System::String^		URI;
		System::String^		queryString;	
		System::String^		remoteAddress;
		System::String^		contentType;
		System::Boolean		compressed;
		System::UInt32		rawSize;
		System::UInt32		compressedSize;
	};

	/// <exclude/>
	public ref class ResponseDetails
	{	

	public:		

		ResponseDetails(System::Object^ d)
		{
			details = (ResponseDetailHolder^) d;
		}

		property System::DateTime^ Time
		{
			System::DateTime^ get() { return details->time; }
		}

		property System::String^ ServerName
		{
			System::String^ get() { return details->serverName; }
		}

		property System::String^ ServerPort
		{
			System::String^ get() { return details->serverPort; }
		}

		property System::String^ InstanceId
		{
			System::String^ get() { return details->instanceId; }
		}

		property System::String^ URLMapPath
		{
			System::String^ get() { return details->URLMapPath; }
		}

		property System::String^ UserAgent
		{
			System::String^ get() { return details->userAgent; }
		}

		property System::String^ Hostname
		{
			System::String^ get() { return details->hostname; }
		}

		property System::String^ URI
		{
			System::String^ get() { return details->URI; }
		}

		property System::String^ QueryString
		{
			System::String^ get() { return details->queryString; }
		}

		property System::String^ RemoteAddress
		{
			System::String^ get() { return details->remoteAddress; }
		}

		property System::String^ ContentType
		{
			System::String^ get() { return details->contentType; }
		}

		property System::Boolean IsCompressed
		{
			System::Boolean get() { return details->compressed; }
		}

		property System::UInt32 RawSize
		{
			System::UInt32 get() { return details->rawSize; }
		}

		property System::UInt32 CompressedSize
		{
			System::UInt32 get() { return details->compressedSize; }
		}

	private:

		ResponseDetailHolder^ details;
	};
}