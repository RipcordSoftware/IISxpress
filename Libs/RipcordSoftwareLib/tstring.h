#pragma once

#include <string>

namespace Ripcord { namespace Types
{	
	// conditionally decide what type of string to use
	#ifndef _UNICODE
	typedef std::string tstring;
	#else
	typedef std::wstring tstring;
	#endif
}}