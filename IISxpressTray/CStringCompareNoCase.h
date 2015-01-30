#ifndef _CSTRINGCOMPARENOCASE_H
#define _CSTRINGCOMPARENOCASE_H

struct CStringCompareNoCase : public std::less<CString>
{
	bool operator()(const CString& a, const CString& b) const
	{return a.CompareNoCase(b) < 0 ? true : false;
	}
};

struct CStringWCompareNoCase : public std::less<CStringW>
{
	bool operator()(const CStringW& a, const CStringW& b) const
	{return a.CompareNoCase(b) < 0 ? true : false;
	}
};

#endif