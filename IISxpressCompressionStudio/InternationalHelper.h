#pragma once

class CInternationalHelper
{

public:

	CInternationalHelper(void);
	~CInternationalHelper(void);

	static bool GetListSeperator(CString& sSep);
	static bool GetDecimalSeperator(CString& sSep);
	static bool GetThousandSeperator(CString& sSep);
	static bool GetNegativeSymbol(CString& sSep);

	static bool FormatIntegerNumber(CString& sNumber);	
	static bool FormatFloatNumber(CString& sNumber, int nDecimalPlaces);

	static bool Format24hTime(const SYSTEMTIME& stTime, CString& sTime);
	static bool FormatDateTime(const SYSTEMTIME& stTime, BOOL bOutputSeconds, CString& sTime);
};
