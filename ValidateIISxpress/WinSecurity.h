#pragma once

namespace RS { namespace WinSecurity {

template <typename T> class SmartBuffer
{
public:

	SmartBuffer() {}

	SmartBuffer(size_t size) : buffer(size) {}

	SmartBuffer(BYTE* pData, size_t size) : buffer(size)
	{
		std::copy(pData, pData + size, buffer.begin());
	}

	SmartBuffer(T* pSid, size_t size) : buffer(size)
	{
		BYTE* pBuffer = (BYTE*) pSid;
		std::copy(pBuffer, pBuffer + size, buffer.begin());
	}

	SmartBuffer& operator =(const SmartBuffer& other)
	{
		buffer = other.buffer;		
		return *this;
	}

	bool operator ==(const SmartBuffer& other)
	{
		return buffer == other.buffer;
	}

	bool Compare(BYTE* pData, size_t size)
	{		
		return buffer.size() == size ? std::equal(pData, pData + size, &buffer[0]) : false;
	}

	bool Compare(T* pData, size_t size)
	{
		return Compare((BYTE*) pData, size);
	}

	size_t size() { return buffer.size(); }

	operator void* ()
	{
		return (void*) &buffer[0];
	}

private:

	std::vector<BYTE> buffer;
};

class SmartSid : public SmartBuffer<SID>
{
public:

	SmartSid() {}

	SmartSid(size_t size) : SmartBuffer<SID>(size) {}

	SmartSid(BYTE* pData, size_t size) : SmartBuffer<SID>(pData, size)
	{		
	}

	SmartSid(SID* pSid, size_t size) : SmartBuffer<SID>(pSid, size)
	{
	}

	bool IsValid() { return ::IsValidSid(*this) == TRUE; }
};

class SmartSecurityDescriptor : public SmartBuffer<SECURITY_DESCRIPTOR>
{
public:

	SmartSecurityDescriptor() {}

	SmartSecurityDescriptor(size_t size) : SmartBuffer<SECURITY_DESCRIPTOR>(size) {}

	SmartSecurityDescriptor(BYTE* pData, size_t size) : SmartBuffer<SECURITY_DESCRIPTOR>(pData, size)
	{		
	}

	SmartSecurityDescriptor(SECURITY_DESCRIPTOR* pSD, size_t size) : SmartBuffer<SECURITY_DESCRIPTOR>(pSD, size)
	{
	}

	bool IsValid() { return ::IsValidSecurityDescriptor(*this) == TRUE; }
};

class WinSecurity
{
private:
	WinSecurity(void);
	~WinSecurity(void);

public:

	static bool GetAccountSid(LPCTSTR pszAccount, SmartSid& sid);
	static bool CheckPermissons(SmartSecurityDescriptor& SD, SmartSid& sid, ACCESS_MASK mask);

	static bool GetKeySecurity(HKEY key, SmartSecurityDescriptor& SD);

	static bool GetFileSecurity(LPCTSTR pszFileName, SmartSecurityDescriptor& SD);
};

} }