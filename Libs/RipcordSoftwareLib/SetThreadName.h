#pragma once

namespace Ripcord { namespace Utils
{

class RIPCORDSOFTWARELIB_DLL Threading
{
public:
	static void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);
};

}}