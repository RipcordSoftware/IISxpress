#ifndef _REGISTRYKEYS_H
#define _REGISTRYKEYS_H

#define RIPCORD_SOFTWARE							_T("Ripcord Software")

#define IISXPRESSFILTER_REGKEY						_T("SOFTWARE\\") RIPCORD_SOFTWARE _T("\\IISxpress")
#define IISXPRESSSERVER_REGKEY						_T("SOFTWARE\\") RIPCORD_SOFTWARE _T("\\IISxpressServer")
#define IISXPRESSTRAY_REGKEY						_T("SOFTWARE\\") RIPCORD_SOFTWARE _T("\\IISxpressTray")

#define IISXPRESS_REGKEY_USE64BITREG				_T("Use64BitReg")

#define IISXPRESS_REGKEY_ENABLED					_T("Enabled")
#define IISXPRESS_REGKEY_DEBUGENABLED				_T("DebugEnabled")
#define IISXPRESS_REGKEY_LOGGINGLEVEL				_T("LoggingLevel")
#define IISXPRESS_REGKEY_INSTALLPATH				_T("InstallPath")

#define IISXPRESS_LOGGINGLEVEL_NONE					0
#define IISXPRESS_LOGGINGLEVEL_BASIC				1
#define IISXPRESS_LOGGINGLEVEL_ENH					2
#define IISXPRESS_LOGGINGLEVEL_FULL					3

#define IISXPRESS_INSTALLPATH						_T("InstallPath")

#define IISXPRESS_PERFCOUNTERS_ENABLED				_T("EnablePerfCounters")

#define IISXPRESS_HANDLEPOSTRESPONSES				_T("HandlePOSTResponses")

#define IISXPRESS_SERVER_LICENSEMODE				_T("LicenseMode")
#define IISXPRESS_SERVER_CHECKPHYSICALMEMORY		_T("CheckPhysicalMemory")
#define IISXPRESS_SERVER_CHECKCPULOAD				_T("CheckCPULoad")
#define IISXPRESS_SERVER_CHECKNEVERRULES			_T("CheckNeverRules")
#define IISXPRESS_SERVER_CHECKUSERRULES				_T("CheckUserRules")
#define IISXPRESS_SERVER_BZIP2MODE					_T("BZIP2Mode")

#define IISXPRESS_SERVER_AHA_ENABLED				_T("AHAEnabled")
#define IISXPRESS_SERVER_AHA_DETECTOVERHEAT			_T("AHADetectOverheat")

#define IISXPRESS_COMPRESSLOCALHOST					_T("CompressLocalhost")
#define IISXPRESS_ISAPILOADPRIORITY					_T("ISAPILoadPriority")

#define IISXPRESS_ENABLEUSERAGENTEXCLUSION			_T("EnableUserAgentExclusion")
#define IISXPRESS_EXCLUDEDUSERAGENTS				_T("ExcludedUserAgents")

// Keys for the tray (user specific)
#define IISXPRESSTRAY_SETTINGS						_T("Settings")
#define IISXPRESSTRAY_WINDOWPOSX					_T("X")
#define IISXPRESSTRAY_WINDOWPOSY					_T("Y")
#define IISXPRESSTRAY_DISPLAYCOOKIE					_T("DisplayCookie")
#define IISXPRESSTRAY_HISTORYENABLED				_T("HistoryEnabled")

// Keys for the tray (global)
#define IISXPRESSTRAY_UPDATEURL						_T("UpdateURL")
#define IISXPRESSTRAY_SHOWAHACONFIG					_T("ShowAHAConfig")

// Keys for the Compression Studio
#define IISXPRESSCS_SETTINGS						_T("Settings")
#define IISXPRESSCS_WINDOWPOSX						_T("X")
#define IISXPRESSCS_WINDOWPOSY						_T("Y")
#define IISXPRESSCS_WINDOWWIDTH						_T("Width")
#define IISXPRESSCS_WINDOWHEIGHT					_T("Height")
#define IISXPRESSCS_WINDOWMAXIMIZED					_T("Maximized")
#define IISXPRESSCS_DISPLAYCOOKIE					_T("DisplayCookie")
#define IISXPRESSCS_SETTINGS_TOOLBARS				IISXPRESSCS_SETTINGS _T("\\Toolbars")

// keys for the filter/module cache
#define IISXPRESS_CACHE_ENABLED						_T("CacheEnabled")
#define IISXPRESS_CACHE_MAXENTRIES					_T("CacheMaxEntries")
#define IISXPRESS_CACHE_MAXSIZEKB					_T("CacheMaxSizeKB")
#define IISXPRESS_CACHE_STATECOOKIE					_T("CacheStateCookie")

#endif