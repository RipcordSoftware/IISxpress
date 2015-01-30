// stdafx.h : 標準系統 Include 檔的 Include 檔，
// 或是經常使用卻很少變更的專案專用 Include 檔案

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 從 Windows 標頭排除不常使用的成員
#endif

// 如果您有必須優先選取的平台，請修改下列定義。
// 參考 MSDN 取得不同平台對應值的最新資訊。
#ifndef WINVER				// 允許使用 Windows 95 與 Windows NT 4 (含) 以後版本的特定功能。
#define WINVER 0x0400		// 將它變更為針對 Windows 98 和 Windows 2000 (含) 以後版本適當的值。
#endif

#ifndef _WIN32_WINNT		// 允許使用 Windows NT 4 (含) 以後版本的特定功能。
#define _WIN32_WINNT 0x0400		// 將它變更為針對 Windows 98 和 Windows 2000 (含) 以後版本適當的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允許使用 Windows 98 (含) 以後版本的特定功能。
#define _WIN32_WINDOWS 0x0410 // 將它變更為針對 Windows Me (含) 以後版本適當的值。
#endif

#ifndef _WIN32_IE			// 允許使用 IE 4.0 (含) 以後版本的特定功能。
#define _WIN32_IE 0x0400	// 將它變更為針對 IE 5.0 (含) 以後版本適當的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 明確定義部分的 CString 建構函式

// 關閉 MFC 隱藏一些常見或可忽略警告訊息的功能
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心與標準元件
#include <afxext.h>         // MFC 擴充功能

#include <afxdtctl.h>		// MFC 支援的 Internet Explorer 4 通用控制項
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 支援的 Windows 通用控制項
#endif // _AFX_NO_AFXCMN_SUPPORT

