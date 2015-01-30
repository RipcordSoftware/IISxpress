#include "stdafx.h"

#include "RegistryKeys.h"

namespace Ripcord { namespace IISxpress {

const wchar_t* IISxpressRegKeys::RIPCORD_SOFTWARE = L"Ripcord Software";

const wchar_t* IISxpressRegKeys::IISXPRESSFILTER_REGKEY = L"SOFTWARE\\Ripcord Software\\IISxpress";
const wchar_t* IISxpressRegKeys::IISXPRESSSERVER_REGKEY = L"SOFTWARE\\Ripcord Software\\IISxpressServer";
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_REGKEY = L"SOFTWARE\\Ripcord Software\\IISxpressTray";

const wchar_t* IISxpressRegKeys::IISXPRESS_REGKEY_USE64BITREG = L"Use64BitReg";

const wchar_t* IISxpressRegKeys::IISXPRESS_REGKEY_ENABLED = L"Enabled";
const wchar_t* IISxpressRegKeys::IISXPRESS_REGKEY_DEBUGENABLED = L"DebugEnabled";
const wchar_t* IISxpressRegKeys::IISXPRESS_REGKEY_LOGGINGLEVEL = L"LoggingLevel";
const wchar_t* IISxpressRegKeys::IISXPRESS_REGKEY_INSTALLPATH = L"InstallPath";

const wchar_t* IISxpressRegKeys::IISXPRESS_INSTALLPATH = L"InstallPath";

const wchar_t* IISxpressRegKeys::IISXPRESS_PERFCOUNTERS_ENABLED = L"EnablePerfCounters";

const wchar_t* IISxpressRegKeys::IISXPRESS_HANDLEPOSTRESPONSES = L"HandlePOSTResponses";

const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_LICENSEMODE = L"LicenseMode";
const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_CHECKPHYSICALMEMORY = L"CheckPhysicalMemory";
const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_CHECKCPULOAD = L"CheckCPULoad";
const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_CHECKNEVERRULES = L"CheckNeverRules";
const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_CHECKUSERRULES = L"CheckUserRules";
const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_BZIP2MODE = L"BZIP2Mode";

const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_AHA_ENABLED = L"AHAEnabled";
const wchar_t* IISxpressRegKeys::IISXPRESS_SERVER_AHA_DETECTOVERHEAT = L"AHADetectOverheat";

const wchar_t* IISxpressRegKeys::IISXPRESS_COMPRESSLOCALHOST = L"CompressLocalhost";
const wchar_t* IISxpressRegKeys::IISXPRESS_ISAPILOADPRIORITY = L"ISAPILoadPriority";

const wchar_t* IISxpressRegKeys::IISXPRESS_ENABLEUSERAGENTEXCLUSION = L"EnableUserAgentExclusion";
const wchar_t* IISxpressRegKeys::IISXPRESS_EXCLUDEDUSERAGENTS = L"ExcludedUserAgents";

// Keys for the tray (user specific)
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_SETTINGS = L"Settings";
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_WINDOWPOSX = L"X";
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_WINDOWPOSY = L"Y";
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_DISPLAYCOOKIE = L"DisplayCookie";
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_HISTORYENABLED = L"HistoryEnabled";

// Keys for the tray (global)
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_UPDATEURL = L"UpdateURL";
const wchar_t* IISxpressRegKeys::IISXPRESSTRAY_SHOWAHACONFIG = L"ShowAHAConfig";

// Keys for the Compression Studio
const wchar_t* IISxpressRegKeys::IISXPRESSCS_SETTINGS = L"Settings";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_WINDOWPOSX = L"X";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_WINDOWPOSY = L"Y";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_WINDOWWIDTH = L"Width";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_WINDOWHEIGHT = L"Height";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_WINDOWMAXIMIZED = L"Maximized";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_DISPLAYCOOKIE = L"DisplayCookie";
const wchar_t* IISxpressRegKeys::IISXPRESSCS_SETTINGS_TOOLBARS = L"Settings\\Toolbars";

// keys for the filter/module cache
const wchar_t* IISxpressRegKeys::IISXPRESS_CACHE_ENABLED = L"CacheEnabled";
const wchar_t* IISxpressRegKeys::IISXPRESS_CACHE_MAXENTRIES = L"CacheMaxEntries";
const wchar_t* IISxpressRegKeys::IISXPRESS_CACHE_MAXSIZEKB = L"CacheMaxSizeKB";
const wchar_t* IISxpressRegKeys::IISXPRESS_CACHE_STATECOOKIE = L"CacheStateCookie";
}}