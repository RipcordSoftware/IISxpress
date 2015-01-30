#pragma once

#include "CRC16.h"
#include "Blowfish.h"

#define PRODUCTKEY_ID					'X'
#define PRODUCTKEY_VERSION				0x10

#define PRODUCTKEY_ISSTRONGKEY(x)		((x.byFlags & 0x80) == 0 && (x.byFlags & 0x40) != 0)

#define PRODUCTKEY_REGISTERED			0xff
#define PRODUCTKEY_GETMONTH(x)			(x.byMonthYear >> 4)
#define PRODUCTKEY_GETYEAR(x)			(((unsigned) x.byMonthYear & 0x0f) + 2005)

#define PRODUCTKEY_BIGUSERIDFLAG		0x20
#define PRODUCTKEY_MAXBIGUSERID			(1 << (16 + 4))

#pragma pack(push)
#pragma pack(1)

struct ProductKeyData
{
	BYTE	byID;						// 0: Product ID field
	BYTE	byFlags;					// 1: flags
										//    bit 7: 1 = field is random data, 0 = field is used
										//    bit 6: 1 = key can replace expired trial key, 0 = normal
										//    bit 5: 1 = bits 0 to 3 are MSB of user id, 0 = random data
										//    bit 4: 0 (unused)										
										//    bits 0 to 3: MSB of user id OR random (see bit 5)

	union								// 2: the id of the user who registered the product	
	{
		WORD	wUserId;
		BYTE	byUserId[2];				
	};

	BYTE	byVersion;					// 4: major & minor version
	BYTE	byMonthYear;				// 5: expiry month & year

	union								// 6: crc-16
	{
		WORD	wCRC;						
		BYTE	byCRC[2];
	};
};

#pragma pack(pop)

class CProductLicense
{

public:

	CProductLicense(void);
	~CProductLicense(void);

	// S_OK				- key valid
	// E_INVALIDARG		- key invalid (somehow)
	__forceinline HRESULT Decrypt(ProductKeyData& ProductKey, const BYTE* pbyKey, int nKeySize)
	{
		if (pbyKey == NULL || nKeySize <= 0)
			return E_POINTER;		

		// set the key into blowfish and then decrypt
		m_Blowfish.Set_Passwd(pbyKey, nKeySize);
		if (m_Blowfish.Decrypt((void*) &ProductKey, sizeof(ProductKey)) == false)
			return E_FAIL;

		// calculate the crc and compare against the data
		WORD wCRC = CCRC16::CalcCRC16Words(
			(sizeof(ProductKey) - sizeof(ProductKey.wCRC)) / sizeof(short), 
			(short*) &ProductKey);

		if (wCRC != ProductKey.wCRC)
			return E_INVALIDARG;

		return S_OK;
	}

	// S_OK				- key valid
	// S_FALSE			- key valid but expired
	// E_INVALIDARG		- malformed key
	// E_FAIL			- general failure
	__forceinline HRESULT CheckProductKey(
							const ProductKeyData& ProductKey, 
							const BYTE* pbyKey, int nKeySize,
							bool& bStrongKey,
							bool& bRegisteredKey)
	{
		if (pbyKey == NULL || nKeySize <= 0)
			return E_POINTER;

		ProductKeyData CopyProductKey = ProductKey;
		HRESULT hr = Decrypt(CopyProductKey, pbyKey, nKeySize);
		if (hr != S_OK)
			return hr;		

		// check we have the correct id field
		if (CopyProductKey.byID != PRODUCTKEY_ID)
			return E_INVALIDARG;

		// check the product version matches
		if (CopyProductKey.byVersion != PRODUCTKEY_VERSION)
			return E_INVALIDARG;

		// get the local system time
		SYSTEMTIME stLocal;
		::GetLocalTime(&stLocal);		

		// determine the strength of the key
		if (PRODUCTKEY_ISSTRONGKEY(CopyProductKey) != 0)
			bStrongKey = true;
		else
			bStrongKey = false;

		// assume the key isn't registered
		bRegisteredKey = false;
		
		if (CopyProductKey.byMonthYear == PRODUCTKEY_REGISTERED)
		{
			// it's a registered key
			bRegisteredKey = true;
		}
		else if	(PRODUCTKEY_GETYEAR(CopyProductKey) < stLocal.wYear ||
				(PRODUCTKEY_GETYEAR(CopyProductKey) == stLocal.wYear && 
				PRODUCTKEY_GETMONTH(CopyProductKey) < stLocal.wMonth))
		{
			// the key has expired			
			return S_FALSE;
		}

		return S_OK;
	}

	// S_OK				- succeeded
	// S_FALSE			- succeeded, but key does not have expiry
	// E_INVALIDARG		- malformed key
	// E_FAIL			- general failure
	__forceinline HRESULT GetKeyExpiry(ProductKeyData& ProductKey, const BYTE* pbyKey, 
											int nKeySize, SYSTEMTIME& stExpiry)
	{
		if (pbyKey == NULL || nKeySize <= 0)
			return E_POINTER;

		ProductKeyData CopyProductKey = ProductKey;
		HRESULT hr = Decrypt(CopyProductKey, pbyKey, nKeySize);
		if (hr != S_OK)
			return hr;		

		// check we have the correct id field
		if (CopyProductKey.byID != PRODUCTKEY_ID)
			return E_INVALIDARG;

		// check the product version matches
		if (CopyProductKey.byVersion != PRODUCTKEY_VERSION)
			return E_INVALIDARG;

		// if the product is fully registered then it does not have an expiry date
		if (CopyProductKey.byMonthYear == PRODUCTKEY_REGISTERED)
			return S_FALSE;

		// the product expires, set the month and year into the SYSTEMTIME
		memset(&stExpiry, 0, sizeof(stExpiry));
		stExpiry.wMonth = PRODUCTKEY_GETMONTH(CopyProductKey);
		stExpiry.wYear = PRODUCTKEY_GETYEAR(CopyProductKey);		

		return S_OK;
	}

	__forceinline HRESULT Encrypt(ProductKeyData& ProductKey, const BYTE* pbyKey, int nKeySize)
	{
		if (pbyKey == NULL || nKeySize <= 0)
			return E_POINTER;

		// calculate the crc
		WORD wCRC = CCRC16::CalcCRC16Words(
			(sizeof(ProductKey) - sizeof(ProductKey.wCRC)) / sizeof(short), 
			(short*) &ProductKey);

		ProductKey.wCRC = wCRC;

		// set the key into blowfish and then decrypt
		m_Blowfish.Set_Passwd(pbyKey, nKeySize);
		if (m_Blowfish.Encrypt((void*) &ProductKey, sizeof(ProductKey)) == false)
			return E_FAIL;						

		return S_OK;
	}

	static void SetBigUserId(ProductKeyData& ProductKey, DWORD dwUserId)
	{
		ProductKey.byFlags = PRODUCTKEY_BIGUSERIDFLAG;
		ProductKey.byFlags |= ((dwUserId >> 16) & 0x7); 
		ProductKey.wUserId = (WORD) (dwUserId & 0xffff);
	}

	static bool IsBigUserId(const ProductKeyData& ProductKey)
	{
		return (ProductKey.byFlags & PRODUCTKEY_BIGUSERIDFLAG) == PRODUCTKEY_BIGUSERIDFLAG ? true : false;
	}

	static bool ParseProductKeyString(const CStringA& sKey, ProductKeyData& ProductKey);	

	static void ConvertProductKeyToString(const ProductKeyData& ProductKey, CString& sKey);
	static void ConvertProductKeyToFriendlyString(const ProductKeyData& ProductKey, CString& sKey);

private:

	static BYTE ConvertHexCharsToByte(const BYTE byBytes[2]);

	CBlowfish		m_Blowfish;

};
