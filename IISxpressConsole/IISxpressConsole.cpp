// IISxpressConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IISxpressConsole.h"

#include "AutoArray.h"

#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

#ifdef _UNICODE
#define tcout wcout
#else
#define tcout cout
#endif

void PrintBanner(void);
void PrintUsage(void);

void HandleFilterCommand(int argc, TCHAR* argv[]);
void HandleCompressionModeCommand(int argc, TCHAR* argv[]);

void MakeCommandArray(int argc, TCHAR* argv[], CStringArray& Args);

class CIISxpressConsoleModule :
	public CAtlMfcModule
{
	DECLARE_LIBID(LIBID_IISxpressConsoleLib);
};

CIISxpressConsoleModule _AtlModule;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{	
	PrintBanner();

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return 1;
	}

	int nStatus = 0;

	// get the OS version info
	g_VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	::GetVersionEx((OSVERSIONINFO*) &g_VerInfo);

	// we need COM
	::CoInitialize(NULL);	

	try
	{
		if (argc == 1 || (argc == 2 && 
			(_tcsicmp(argv[1], _T("-help")) == 0 || 
			 _tcsicmp(argv[1], _T("-h")) == 0 || 
			 _tcsicmp(argv[1], _T("help")) == 0 || 
			 _tcscmp(argv[1], _T("/?")) == 0)))
		{
			PrintUsage();
		}
		else if (argc >= 2 && _tcsicmp(argv[1], _T("help")) == 0)
		{
			CStringArray Args;
			MakeCommandArray(argc - 2, argv + 2, Args);
			HandleHelpCommand(Args);
		}
		else if (argc >= 2 && _tcsicmp(argv[1], _T("filter")) == 0)
		{
			HandleFilterCommand(argc, argv);
		}
		else if (argc >= 2 && _tcsicmp(argv[1], _T("compmode")) == 0)
		{
			HandleCompressionModeCommand(argc, argv);
		}
		else if (argc == 2 && _tcsicmp(argv[1], _T("monitor")) == 0)
		{
			HandleMonitorCommand(argc, argv);
		}
		else if (argc >= 2 && _tcsicmp(argv[1], _T("add")) == 0)
		{
			CStringArray Args;
			MakeCommandArray(argc - 2, argv + 2, Args);
			HandleAddCommand(Args);
		}
		else if (argc >= 2 && _tcsicmp(argv[1], _T("remove")) == 0)
		{
			CStringArray Args;
			MakeCommandArray(argc - 2, argv + 2, Args);
			HandleRemoveCommand(Args);
		}
		else if (argc >= 2 && _tcsicmp(argv[1], _T("list")) == 0)
		{
			CStringArray Args;
			MakeCommandArray(argc - 2, argv + 2, Args);
			HandleListCommand(Args);
		}		
		else
		{
			PrintUsage();
		}
	}
	catch (HRESULT hr)
	{
		_tprintf(_T("Error: 0x%08x\n"), hr);
		nStatus = 1;
	}
	catch (CString sError)
	{
		_tprintf(_T("Error: %s\n"), sError);
		nStatus = 1;
	}
	catch (ProcessingException pe)
	{
		_tprintf(_T("Error: 0x%08x\n%s\n"), pe.m_hrError, pe.m_sError);
		nStatus = 1;
	}

	::CoUninitialize();

	// when running on Vista we need to wait for a keypress
	if (g_VerInfo.dwMajorVersion >= 6)
	{
		_tprintf(_T("\nPress any key to continue...\n"));
		_getch();
	}

	return nStatus;
}

void PrintBanner(void)
{
	TCHAR szModuleName[512];
	VERIFY(::GetModuleFileName(NULL, szModuleName, _countof(szModuleName)) > 0);

	DWORD dwVersionHandle = 0;
	DWORD dwVersionInfoSize = ::GetFileVersionInfoSize(szModuleName, &dwVersionHandle);
	ASSERT(dwVersionInfoSize > 0);
	
	AutoArray<BYTE> pVersionInfo(dwVersionInfoSize);

	VERIFY(::GetFileVersionInfo(szModuleName, dwVersionHandle, dwVersionInfoSize, pVersionInfo));

	VS_FIXEDFILEINFO* pFixedFileInfo = NULL;
	UINT nFixedFileInfoSize = 0;
	VERIFY(::VerQueryValue(pVersionInfo, _T("\\"), (void**) &pFixedFileInfo, &nFixedFileInfoSize));	

	_tprintf(_T("\nIISxpress v%u.%u.%u.%u (c) Ripcord Software 2005-2008\n\n"), 
		pFixedFileInfo->dwProductVersionMS >> 16, 
		pFixedFileInfo->dwProductVersionMS & 0xffff,
		pFixedFileInfo->dwProductVersionLS >> 16, 
		pFixedFileInfo->dwProductVersionLS & 0xffff);
}

void PrintUsage(void)
{
	_tprintf(
		_T("Select from the following list of commands:\n")
		_T("\n")
		_T("add [extn|ct|uri|ip|file]     - add an extension, content type, URI, IP or\n")
		_T("                                file rule\n")
		_T("remove [extn|ct|uri|ip|file]  - remove a rule\n")
		_T("list [extn|ct|uri|ip|file]    - list a rule\n")
		_T("filter [enable|disable]       - enable or disable the ISAPI filter\n")
		_T("compmode [default|speed|size] - query or set the compression mode\n")
		_T("monitor                       - monitor compression activity\n")
		_T("help [command]                - display help information for a command\n")
		_T("\n")
		_T("For more information on a command try help [command]. For example: help add\n")
		);
}

void HandleFilterCommand(int argc, TCHAR* argv[])
{
	CRegKey FilterRegKey;
	LONG nStatus = FilterRegKey.Open(HKEY_LOCAL_MACHINE, IISXPRESSFILTER_REGKEY, KEY_READ | KEY_WRITE);
	if (nStatus != ERROR_SUCCESS)
	{		
		throw ProcessingException(MAKE_HRESULT(1, FACILITY_WIN32, nStatus), IDS_ERROR_REGISTRYPERMISSIONS);
	}

	if (argc == 2)
	{
		DWORD dwEnabled = 0;
		nStatus = FilterRegKey.QueryDWORDValue(IISXPRESS_REGKEY_ENABLED, dwEnabled);
		if (nStatus != ERROR_SUCCESS)
		{			
			throw ProcessingException(MAKE_HRESULT(1, FACILITY_WIN32, nStatus), IDS_ERROR_MISSINGIISXPRESSREGISTRY);
		}

		CString sMsg;
		if (dwEnabled != 0)
		{
			VERIFY(sMsg.LoadString(IDS_FILTERENABLED));
		}
		else
		{
			VERIFY(sMsg.LoadString(IDS_FILTERDISABLED));
		}
		
		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else if (argc == 3 && _tcsicmp(_T("enable"), argv[2]) == 0)
	{
		nStatus = FilterRegKey.SetDWORDValue(IISXPRESS_REGKEY_ENABLED, 1);
		if (nStatus != ERROR_SUCCESS)
		{
			throw ProcessingException(MAKE_HRESULT(1, FACILITY_WIN32, nStatus), IDS_ERROR_UNABLETOSETFILTERENABLESTATUS);
		}

		CString sMsg;		
		VERIFY(sMsg.LoadString(IDS_FILTERENABLED));				
		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else if (argc == 3 && _tcsicmp(_T("disable"), argv[2]) == 0)
	{
		nStatus = FilterRegKey.SetDWORDValue(IISXPRESS_REGKEY_ENABLED, 0);
		if (nStatus != ERROR_SUCCESS)
		{
			throw ProcessingException(MAKE_HRESULT(1, FACILITY_WIN32, nStatus), IDS_ERROR_UNABLETOSETFILTERENABLESTATUS);
		}		

		CString sMsg;	
		VERIFY(sMsg.LoadString(IDS_FILTERDISABLED));
		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDS_ERROR_BADPARAMETER, argv[2]);
		throw sMsg;
	}	
}

void HandleCompressionModeCommand(int argc, TCHAR* argv[])
{
	// get the active HTTPRequestUnk interface
	CComPtr<IUnknown> pHTTPRequestUnk;
	HRESULT hr = ::GetActiveObject(CLSID_IISxpressHTTPRequest, NULL, (IUnknown**) &pHTTPRequestUnk);
	if (hr != S_OK)
	{
		throw ProcessingException(hr, IDS_ERROR_NORUNNINGIISXPRESS);				
	}

	CComQIPtr<IIISxpressSettings> pSettings = pHTTPRequestUnk;
	if (pSettings == NULL)
	{
		CString sMsg;
		VERIFY(sMsg.LoadString(IDS_ERROR_UNABLETOSETCOMPMODEONSERVER));
		throw sMsg;
	}

	if (argc == 2)
	{
		CompressionMode Mode = CompressionModeDefault;
		hr = pSettings->get_CompressionMode(&Mode);

		CString sMode;
		switch (Mode)
		{
		case CompressionModeDefault:	
			VERIFY(sMode.LoadString(IDS_COMPMODE_DEFAULT));
			break;

		case CompressionModeSize:
			VERIFY(sMode.LoadString(IDS_COMPMODE_SIZE));
			break;

		case CompressionModeSpeed:
			VERIFY(sMode.LoadString(IDS_COMPMODE_SPEED));
			break;

		default:
			VERIFY(sMode.LoadString(IDS_COMPMODE_UNKNOWN));
			break;
		}

		CString sMsg;
		sMsg.Format(IDS_COMPMODE_STATUSFORMAT, sMode);

		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else if (argc == 3 && _tcsicmp(_T("default"), argv[2]) == 0)
	{
		hr = pSettings->put_CompressionMode(CompressionModeDefault);
		if (hr != S_OK)
		{
			throw ProcessingException(hr, IDS_ERROR_FAILEDTOSETCOMPMODE);			
		}

		CString sMsg;
		sMsg.Format(IDS_SETCOMPMODEOKFORMAT, argv[2]);
		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else if (argc == 3 && _tcsicmp(_T("speed"), argv[2]) == 0)
	{
		hr = pSettings->put_CompressionMode(CompressionModeSpeed);
		if (hr != S_OK)
		{
			throw ProcessingException(hr, IDS_ERROR_FAILEDTOSETCOMPMODE);			
		}

		CString sMsg;
		sMsg.Format(IDS_SETCOMPMODEOKFORMAT, argv[2]);
		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else if (argc == 3 && _tcsicmp(_T("size"), argv[2]) == 0)
	{
		hr = pSettings->put_CompressionMode(CompressionModeSize);
		if (hr != S_OK)
		{
			throw ProcessingException(hr, IDS_ERROR_FAILEDTOSETCOMPMODE);			
		}

		CString sMsg;
		sMsg.Format(IDS_SETCOMPMODEOKFORMAT, argv[2]);
		tcout << (LPCTSTR) sMsg << _T("\n");
	}
	else
	{
		CString sMsg;
		sMsg.Format(IDS_ERROR_BADPARAMETER, argv[2]);
		throw sMsg;
	}	
}

void MakeCommandArray(int argc, TCHAR* argv[], CStringArray& Args)
{
	Args.RemoveAll();

	for (int i = 0; i < argc; i++)
	{
		Args.Add(argv[i]);
	}
}