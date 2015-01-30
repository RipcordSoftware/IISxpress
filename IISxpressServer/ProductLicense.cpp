#include "StdAfx.h"

#include "ProductLicense.h"
#include "CRC16.h"

CProductLicense::CProductLicense(void)
{
}

CProductLicense::~CProductLicense(void)
{
}

bool CProductLicense::ParseProductKeyString(const CStringA& sKey, ProductKeyData& ProductKey)
{
	CStringA sTempKey = sKey;
	sTempKey.Remove('-');
		
	if (sTempKey.GetLength() != 16)
		return false;

	const BYTE* pbyKey = (const BYTE*) (const char*) sTempKey;

	ProductKey.byID = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byFlags = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byUserId[0] = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byUserId[1] = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byVersion = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byMonthYear = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byCRC[0] = ConvertHexCharsToByte(pbyKey); pbyKey += 2;
	ProductKey.byCRC[1] = ConvertHexCharsToByte(pbyKey); pbyKey += 2;

	return true;
}

BYTE CProductLicense::ConvertHexCharsToByte(const BYTE byBytes[2])
{
	BYTE byTempBytes[2] = { byBytes[0], byBytes[1] };

	// convert both bytes to lower case
	if (byTempBytes[0] >= 'A')
		byTempBytes[0] |= 0x20;

	if (byTempBytes[1] >= 'A')
		byTempBytes[1] |= 0x20;

	BYTE byValue = (byTempBytes[0] >= 'a' ?  byTempBytes[0] - 'a' + 10 : byTempBytes[0] - '0');
	byValue <<= 4;
	byValue |= (byTempBytes[1] >= 'a' ?  byTempBytes[1] - 'a' + 10 : byTempBytes[1] - '0');

	return byValue;
}

void CProductLicense::ConvertProductKeyToString(const ProductKeyData& ProductKey, CString& sKey)
{
	const BYTE* pbyKey = (const BYTE*) &ProductKey;

	sKey.Format(
		_T("%02X%02X%02X%02X%02X%02X%02X%02X"), 
		pbyKey[0], pbyKey[1], pbyKey[2], pbyKey[3], 
		pbyKey[4], pbyKey[5], pbyKey[6], pbyKey[7]);
}

void CProductLicense::ConvertProductKeyToFriendlyString(const ProductKeyData& ProductKey, CString& sKey)
{
	const BYTE* pbyKey = (const BYTE*) &ProductKey;

	sKey.Format(
		_T("%02X%02X-%02X%02X-%02X%02X-%02X%02X"), 
		pbyKey[0], pbyKey[1], pbyKey[2], pbyKey[3], 
		pbyKey[4], pbyKey[5], pbyKey[6], pbyKey[7]);
}