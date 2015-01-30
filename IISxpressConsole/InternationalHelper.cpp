#include "StdAfx.h"
#include "InternationalHelper.h"

#include <Winnls.h>

CInternationalHelper::CInternationalHelper(void)
{
}

CInternationalHelper::~CInternationalHelper(void)
{
}

bool CInternationalHelper::GetListSeperator(CString& sSep)
{
	// get the list seperator from the registry
	CRegKey ListSepKey;
	if (ListSepKey.Open(HKEY_CURRENT_USER, _T("Control Panel\\International"), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szSep[16];
		ULONG nSize = sizeof(szSep) / sizeof(szSep[0]);
		if (ListSepKey.QueryStringValue(_T("sList"), szSep, &nSize) == ERROR_SUCCESS)
		{
			sSep = szSep;
			return true;
		}
	}
	
	return false;
}

bool CInternationalHelper::GetDecimalSeperator(CString& sSep)
{
	// get the list seperator from the registry
	CRegKey ListSepKey;
	if (ListSepKey.Open(HKEY_CURRENT_USER, _T("Control Panel\\International"), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szSep[16];
		ULONG nSize = sizeof(szSep) / sizeof(szSep[0]);
		if (ListSepKey.QueryStringValue(_T("sDecimal"), szSep, &nSize) == ERROR_SUCCESS)
		{
			sSep = szSep;
			return true;
		}
	}
	
	return false;
}

bool CInternationalHelper::GetThousandSeperator(CString& sSep)
{
	// get the list seperator from the registry
	CRegKey ListSepKey;
	if (ListSepKey.Open(HKEY_CURRENT_USER, _T("Control Panel\\International"), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szSep[16];
		ULONG nSize = sizeof(szSep) / sizeof(szSep[0]);
		if (ListSepKey.QueryStringValue(_T("sThousand"), szSep, &nSize) == ERROR_SUCCESS)
		{
			sSep = szSep;
			return true;
		}
	}
	
	return false;
}

bool CInternationalHelper::GetNegativeSymbol(CString& sSep)
{
	// get the list seperator from the registry
	CRegKey ListSepKey;
	if (ListSepKey.Open(HKEY_CURRENT_USER, _T("Control Panel\\International"), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szSep[16];
		ULONG nSize = sizeof(szSep) / sizeof(szSep[0]);
		if (ListSepKey.QueryStringValue(_T("sNegativeSign"), szSep, &nSize) == ERROR_SUCCESS)
		{
			sSep = szSep;
			return true;
		}
	}
	
	return false;
}

/*bool CInternationalHelper::FormatInteger(int nNumber, CString& sNumber)
{
	::GetNumberFormat(LOCALE_USER_DEFAULT, 
}*/

bool CInternationalHelper::FormatIntegerNumber(CString& sNumber)
{
	static CString sDecimalSep;
	if (sDecimalSep.GetLength() <= 0)
	{
		GetDecimalSeperator(sDecimalSep);	
	}

	static CString sThousandSep;
	if (sThousandSep.GetLength() <= 0)
	{
		GetThousandSeperator(sThousandSep);
	}
	
	static int nINegNumber = -1;
	if (nINegNumber < 0)
	{
		char szINegNumber[4] = "\0";
		::GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, szINegNumber, sizeof(szINegNumber));
		nINegNumber = atoi(szINegNumber);
	}

	NUMBERFMT NumberFmt;
	NumberFmt.NumDigits = 0;
	NumberFmt.LeadingZero = 0;
	NumberFmt.Grouping = 3;	
	NumberFmt.lpDecimalSep = const_cast<LPTSTR>((LPCTSTR) sDecimalSep);
	NumberFmt.lpThousandSep = const_cast<LPTSTR>((LPCTSTR) sThousandSep);
	NumberFmt.NegativeOrder = nINegNumber;

	TCHAR szNumberBuffer[512];	
	int nChars = ::GetNumberFormat(LOCALE_USER_DEFAULT, 0, sNumber, &NumberFmt, 
		szNumberBuffer, sizeof(szNumberBuffer) / sizeof(szNumberBuffer[0]));

	if (nChars <= 0)
		return false;

	sNumber = szNumberBuffer;

	return true;
}

bool CInternationalHelper::FormatFloatNumber(CString& sNumber, int nDecimalPlaces)
{
	static CString sDecimalSep;
	if (sDecimalSep.GetLength() <= 0)
	{
		GetDecimalSeperator(sDecimalSep);	
	}

	static CString sThousandSep;
	if (sThousandSep.GetLength() <= 0)
	{
		GetThousandSeperator(sThousandSep);
	}
	
	static int nINegNumber = -1;
	if (nINegNumber < 0)
	{
		char szINegNumber[4] = "\0";
		::GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, szINegNumber, sizeof(szINegNumber));
		nINegNumber = atoi(szINegNumber);
	}

	NUMBERFMT NumberFmt;
	NumberFmt.NumDigits = nDecimalPlaces;
	NumberFmt.LeadingZero = 1;
	NumberFmt.Grouping = 3;	
	NumberFmt.lpDecimalSep = const_cast<LPTSTR>((LPCTSTR) sDecimalSep);
	NumberFmt.lpThousandSep = const_cast<LPTSTR>((LPCTSTR) sThousandSep);
	NumberFmt.NegativeOrder = nINegNumber;

	TCHAR szNumberBuffer[512];	
	int nChars = ::GetNumberFormat(LOCALE_USER_DEFAULT, 0, sNumber, &NumberFmt, 
		szNumberBuffer, sizeof(szNumberBuffer) / sizeof(szNumberBuffer[0]));

	if (nChars <= 0)
		return false;

	sNumber = szNumberBuffer;

	return true;
}

bool CInternationalHelper::Format24hTime(const SYSTEMTIME& stTime, CString& sTime)
{
	TCHAR szTimeBuffer[128];	
	int nChars = ::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER, &stTime,
		NULL, szTimeBuffer, sizeof(szTimeBuffer) / sizeof(szTimeBuffer[0]));

	if (nChars <= 0)
		return false;

	sTime = szTimeBuffer;

	return true;
}

bool CInternationalHelper::FormatDateTime(const SYSTEMTIME& stTime, BOOL bOutputSeconds, CString& sTime)
{
	TCHAR szDateBuffer[128] = _T("");
	int nDateChars = ::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stTime, NULL, szDateBuffer, _countof(szDateBuffer));

	if (nDateChars <= 0)
		return false;

	DWORD dwTimeFlags = TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER;
	if (bOutputSeconds == FALSE)
	{
		dwTimeFlags |= TIME_NOSECONDS;
	}

	TCHAR szTimeBuffer[128] = _T("");
	int nTimeChars = ::GetTimeFormat(LOCALE_USER_DEFAULT, dwTimeFlags, &stTime, NULL, szTimeBuffer, _countof(szTimeBuffer));

	if (nTimeChars <= 0)
		return false;

	sTime = szDateBuffer;
	sTime += _T(" ");
	sTime += szTimeBuffer;

	return true;
}