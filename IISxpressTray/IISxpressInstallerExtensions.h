#pragma once

LONG Internal_InstallIISxpressFilter(HWND hwnd, LONG IValue, LPCSTR lpszValue);
LONG Internal_RemoveIISxpressFilter(HWND hwnd, LONG IValue);
LONG APIENTRY IsUserAdmin(void);

// platform specific install/remove methods
LONG Internal_InstallIISxpressFilter_IIS5_6(HWND hwnd, LONG IValue, LPCSTR lpszValue);
LONG Internal_RemoveIISxpressFilter_IIS5_6(HWND hwnd, LONG IValue);
LONG Internal_InstallIISxpressFilter_IIS7(HWND hwnd, LONG IValue, LPCSTR lpszValue, int bitness);
LONG Internal_RemoveIISxpressFilter_IIS7(HWND hwnd, LONG IValue);