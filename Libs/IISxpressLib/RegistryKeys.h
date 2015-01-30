#pragma once

namespace Ripcord { namespace IISxpress {

class IISXPRESSLIB_DLL IISxpressRegKeys
{
public:
	static const wchar_t* RIPCORD_SOFTWARE;

	static const wchar_t* IISXPRESSFILTER_REGKEY;
	static const wchar_t* IISXPRESSSERVER_REGKEY;
	static const wchar_t* IISXPRESSTRAY_REGKEY;

	static const wchar_t* IISXPRESS_REGKEY_USE64BITREG;

	static const wchar_t* IISXPRESS_REGKEY_ENABLED;
	static const wchar_t* IISXPRESS_REGKEY_DEBUGENABLED;
	static const wchar_t* IISXPRESS_REGKEY_LOGGINGLEVEL;
	static const wchar_t* IISXPRESS_REGKEY_INSTALLPATH;

	const static int IISXPRESS_LOGGINGLEVEL_NONE = 0;
	const static int IISXPRESS_LOGGINGLEVEL_BASIC = 1;
	const static int IISXPRESS_LOGGINGLEVEL_ENH = 2;
	const static int IISXPRESS_LOGGINGLEVEL_FULL = 3;

	static const wchar_t* IISXPRESS_INSTALLPATH;

	static const wchar_t* IISXPRESS_PERFCOUNTERS_ENABLED;

	static const wchar_t* IISXPRESS_HANDLEPOSTRESPONSES;

	static const wchar_t* IISXPRESS_SERVER_LICENSEMODE;
	static const wchar_t* IISXPRESS_SERVER_CHECKPHYSICALMEMORY;
	static const wchar_t* IISXPRESS_SERVER_CHECKCPULOAD;
	static const wchar_t* IISXPRESS_SERVER_CHECKNEVERRULES;
	static const wchar_t* IISXPRESS_SERVER_CHECKUSERRULES;
	static const wchar_t* IISXPRESS_SERVER_BZIP2MODE;

	static const wchar_t* IISXPRESS_SERVER_AHA_ENABLED;
	static const wchar_t* IISXPRESS_SERVER_AHA_DETECTOVERHEAT;

	static const wchar_t* IISXPRESS_COMPRESSLOCALHOST;
	static const wchar_t* IISXPRESS_ISAPILOADPRIORITY;

	static const wchar_t* IISXPRESS_ENABLEUSERAGENTEXCLUSION;
	static const wchar_t* IISXPRESS_EXCLUDEDUSERAGENTS;

	// Keys for the tray (user specific)
	static const wchar_t* IISXPRESSTRAY_SETTINGS;
	static const wchar_t* IISXPRESSTRAY_WINDOWPOSX;
	static const wchar_t* IISXPRESSTRAY_WINDOWPOSY;
	static const wchar_t* IISXPRESSTRAY_DISPLAYCOOKIE;
	static const wchar_t* IISXPRESSTRAY_HISTORYENABLED;

	// Keys for the tray (global)
	static const wchar_t* IISXPRESSTRAY_UPDATEURL;
	static const wchar_t* IISXPRESSTRAY_SHOWAHACONFIG;

	// Keys for the Compression Studio
	static const wchar_t* IISXPRESSCS_SETTINGS;
	static const wchar_t* IISXPRESSCS_WINDOWPOSX;
	static const wchar_t* IISXPRESSCS_WINDOWPOSY;
	static const wchar_t* IISXPRESSCS_WINDOWWIDTH;
	static const wchar_t* IISXPRESSCS_WINDOWHEIGHT;
	static const wchar_t* IISXPRESSCS_WINDOWMAXIMIZED;
	static const wchar_t* IISXPRESSCS_DISPLAYCOOKIE;
	static const wchar_t* IISXPRESSCS_SETTINGS_TOOLBARS;

	// keys for the filter/module cache
	static const wchar_t* IISXPRESS_CACHE_ENABLED;
	static const wchar_t* IISXPRESS_CACHE_MAXENTRIES;
	static const wchar_t* IISXPRESS_CACHE_MAXSIZEKB;
	static const wchar_t* IISXPRESS_CACHE_STATECOOKIE;
};

}}