// SystemPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "SystemPage.h"
#include "InternationalHelper.h"

#include "resource.h"

#include <tlhelp32.h>
#include <psapi.h>

#define MAKE_INT64(r, h, l) { r = h; r <<= 32; r |= l; }

// CSystemPage dialog

IMPLEMENT_DYNAMIC(CSystemPage, CIISxpressPageBase)
CSystemPage::CSystemPage()
	: CIISxpressPageBase(CSystemPage::IDD)
{
	m_nLastIISCPUTime = 0;
	m_nLastIISCPUTimeTickCount = 0;

	m_nLastIISxpressCPUTime = 0;
	m_nLastIISxpressCPUTimeTickCount = 0;

	// we need to know the number of CPUs in the system since it affects the load calculation
	SYSTEM_INFO SysInfo;
	::GetSystemInfo(&SysInfo);
	m_dwNumberOfProcessors = SysInfo.dwNumberOfProcessors;
}

CSystemPage::~CSystemPage()
{
}

void CSystemPage::DoDataExchange(CDataExchange* pDX)
{
	CIISxpressPageBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WINDOWSVER, m_cWindowsVersion);
	DDX_Control(pDX, IDC_EDIT_PHYSICALMEMORY, m_cPhysicalMemory);
	DDX_Control(pDX, IDC_EDIT_CPUTYPE, m_cCPUType);
	DDX_Control(pDX, IDC_EDIT_IISVERSION, m_cIISVersion);
	DDX_Control(pDX, IDC_EDIT_ASPNETVERSION, m_cASPNetVersion);
	DDX_Control(pDX, IDC_EDIT_IISCPULOAD, m_cIISCPULoad);
	DDX_Control(pDX, IDC_EDIT_IISMEMORYUSAGE, m_cIISMemoryUsage);
	DDX_Control(pDX, IDC_EDIT_IISXPRESSCPULOAD, m_cIISxpressCPULoad);
	DDX_Control(pDX, IDC_EDIT_IISXPRESSMEMORYUSAGE, m_cIISxpressMemoryUsage);
	DDX_Control(pDX, IDC_STATIC_CPULOADCHARTPLACEHOLDER, m_cCPULoadChartPlaceholder);
}


BEGIN_MESSAGE_MAP(CSystemPage, CIISxpressPageBase)
END_MESSAGE_MAP()


// CSystemPage message handlers

BOOL CSystemPage::OnInitDialog(void)
{
	BOOL bStatus = CIISxpressPageBase::OnInitDialog();

	CRect rcCPULoadChart;
	m_cCPULoadChartPlaceholder.GetWindowRect(&rcCPULoadChart);
	ScreenToClient(&rcCPULoadChart);

	if (m_cCPULoadChart.Create(WS_CHILD | WS_VISIBLE, rcCPULoadChart, this, IDC_GRAPH_IISXPRESSCPULOAD) == TRUE)
	{
		CString sTime;
		sTime.LoadString(IDS_TIMECOLUMN);

		CString sLoad;
		sLoad.LoadString(IDS_GRAPH_LOADPERCENT);

		m_cCPULoadChart.SetRange(0, 100, 0);
		m_cCPULoadChart.SetYUnits(sLoad);
		m_cCPULoadChart.SetXUnits(sTime) ;
		m_cCPULoadChart.SetBackgroundColor(RGB(0, 0, 64)) ;
		m_cCPULoadChart.SetGridColor(RGB(192, 192, 255)) ;
		m_cCPULoadChart.SetPlotColor(RGB(255, 255, 255)) ;		
	}

	CString sWindowsVersion;
	GetWindowsVersion(sWindowsVersion);
	m_cWindowsVersion.SetWindowText(sWindowsVersion);
	m_cWindowsVersion.SetSel(-1, 0);

	CString sPhysicalMemory;
	GetPhysicalMemory(sPhysicalMemory);
	CInternationalHelper::FormatIntegerNumber(sPhysicalMemory);
	m_cPhysicalMemory.SetWindowText(sPhysicalMemory);
	m_cPhysicalMemory.SetSel(-1, 0);

	CString sCPU;
	GetCPUType(sCPU);
	m_cCPUType.SetWindowText(sCPU);
	m_cCPUType.SetSel(-1, 0);

	CString sIISVersion;
	GetIISVersion(sIISVersion);
	m_cIISVersion.SetWindowText(sIISVersion);
	m_cIISVersion.SetSel(-1, 0);

	CString sASPNetVersion;
	GetASPNetVersion(sASPNetVersion);
	m_cASPNetVersion.SetWindowText(sASPNetVersion);
	m_cASPNetVersion.SetSel(-1, 0);

	UpdateControls();

	return bStatus;
}

void CSystemPage::UpdateControls(void)
{
	if (::IsWindow(this->m_hWnd) == FALSE)
		return;

	CString sIISCPULoad;
	GetIISCPULoad(sIISCPULoad);
	m_cIISCPULoad.SetWindowText(sIISCPULoad);
	m_cIISCPULoad.SetSel(-1, 0);

	CString sIISMemUsage;
	GetIISMemoryUsage(sIISMemUsage);
	CInternationalHelper::FormatIntegerNumber(sIISMemUsage);
	m_cIISMemoryUsage.SetWindowText(sIISMemUsage);	
	m_cIISMemoryUsage.SetSel(-1, 0);

	CString sIISxpressCPULoad;
	GetIISxpressCPULoad(sIISxpressCPULoad);
	m_cIISxpressCPULoad.SetWindowText(sIISxpressCPULoad);
	m_cIISxpressCPULoad.SetSel(-1, 0);

	CString sIISxpressMemUsage;
	GetIISxpressMemoryUsage(sIISxpressMemUsage);
	CInternationalHelper::FormatIntegerNumber(sIISxpressMemUsage);
	m_cIISxpressMemoryUsage.SetWindowText(sIISxpressMemUsage);
	m_cIISxpressMemoryUsage.SetSel(-1, 0);
}

// this code came from Microsoft - it is included here with minor changes
BOOL CSystemPage::GetWindowsVersion(CString& sVersion)
{
	sVersion.Empty();

	// try Vista/Longhorn first
	if (GetVistaVersion(sVersion) == TRUE)
		return TRUE;

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
		case VER_PLATFORM_WIN32_NT:

			// Test for the specific product family.			
				
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)						
			{
				if (osvi.wProductType == VER_NT_DOMAIN_CONTROLLER || osvi.wProductType == VER_NT_SERVER)
				{
					sVersion += _T("Windows Server 2003 ");
				}
				else
				{
					sVersion += _T("Windows XP ");
				}
			}			
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			{
				sVersion += _T("Windows XP ");
			}
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			{
				sVersion += _T("Windows 2000 ");
			}
			else if ( osvi.dwMajorVersion <= 4 )
			{
				sVersion += _T("Windows NT ");
			}
			else
			{
				sVersion += _T("Windows (Unknown Version) ");
			}

			// get info on the system
			SYSTEM_INFO SystemInfo;				
			::GetSystemInfo(&SystemInfo);

			if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				sVersion += _T("64bit ");

			// Test for specific product on Windows NT 4.0 SP6 and later.
			if( bOsVersionInfoEx )
			{
				// Test for the workstation type.
				if ( osvi.wProductType == VER_NT_WORKSTATION )
				{
					if( osvi.dwMajorVersion == 4 )
						sVersion += _T("Workstation 4.0 ");
					else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
						sVersion += _T("Home Edition ");
					else
						sVersion += _T("Professional ");
				}
		        
				// Test for the server type.
				else if ( osvi.wProductType == VER_NT_SERVER )
				{
					if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
					{
						if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
							sVersion += _T("Datacenter Edition ");
						else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							sVersion += _T("Enterprise Edition ");
						else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
							sVersion += _T("Web Edition ");
						else
							sVersion += _T("Standard Edition ");
					}

					else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
					{
						if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
							sVersion += _T("Datacenter Server ");
						else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							sVersion += _T("Advanced Server ");
						else
							sVersion += _T("Server ");
					}

					else  // Windows NT 4.0 
					{
						if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							sVersion += _T("Server 4.0, Enterprise Edition ");
						else
							sVersion += _T("Server 4.0 ");
					}
				}
			}
			else  // Test for specific product on Windows NT 4.0 SP5 and earlier
			{
				HKEY hKey;
				TCHAR szProductType[256];
				DWORD dwBufLen = sizeof(szProductType) / sizeof(szProductType[0]);
				LONG lRet;

				lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					_T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
					0, KEY_QUERY_VALUE, &hKey );
				if( lRet != ERROR_SUCCESS )
					return FALSE;

				lRet = RegQueryValueEx( hKey, _T("ProductType"), NULL, NULL,
					(LPBYTE) szProductType, &dwBufLen);
				if( (lRet != ERROR_SUCCESS) /*|| (dwBufLen > BUFSIZE)*/ )
					return FALSE;

				RegCloseKey( hKey );

				if ( lstrcmpi( _T("WINNT"), szProductType) == 0 )
					sVersion += _T("Workstation " );
				if ( lstrcmpi( _T("LANMANNT"), szProductType) == 0 )
					sVersion += _T("Server " );
				if ( lstrcmpi( _T("SERVERNT"), szProductType) == 0 )
					sVersion += _T("Advanced Server " );

				//printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
				CString sTemp;
				sTemp.Format(_T("%d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion );
				sVersion += sTemp;
			}

			// Display service pack (if any) and build number.

			if( osvi.dwMajorVersion == 4 && 
				lstrcmpi( osvi.szCSDVersion, _T("Service Pack 6") ) == 0 )
			{
				HKEY hKey;
				LONG lRet;

				// Test for SP6 versus SP6a.
				lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"),
					0, KEY_QUERY_VALUE, &hKey );
				if( lRet == ERROR_SUCCESS )
				{
					//printf("Service Pack 6a (Build %d)\n"), osvi.dwBuildNumber & 0xFFFF );         

					CString sTemp;
					sTemp.Format(_T("Service Pack 6a (Build %d)"), osvi.dwBuildNumber & 0xFFFF );
					sVersion += sTemp;
				}
				else // Windows NT 4.0 prior to SP6a
				{
					//printf( "%s (Build %d)\n",
					//	osvi.szCSDVersion,
					//	osvi.dwBuildNumber & 0xFFFF);

					CString sTemp;
					sTemp.Format(_T("%s (Build %d)"), osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);

					sVersion += sTemp;
				}

				RegCloseKey( hKey );
			}
			else // Windows NT 3.51 and earlier or Windows 2000 and later
			{
				//printf( "%s (Build %d)\n",
				//	osvi.szCSDVersion,
				//	osvi.dwBuildNumber & 0xFFFF);

				CString sTemp;
				sTemp.Format(_T("%s (Build %d)"), osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);

				sVersion += sTemp;
			}

			break;

		// Test for the Windows 95 product family.
		case VER_PLATFORM_WIN32_WINDOWS:

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			{
				sVersion += _T("Windows 95 ");

				if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
					sVersion += _T("OSR2 " );
			} 

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				sVersion += _T("Windows 98 ");
				if ( osvi.szCSDVersion[1] == 'A' )
					sVersion += _T("SE " );
			} 

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			{
				sVersion += _T("Millennium Edition");
			} 
			break;

		case VER_PLATFORM_WIN32s:

			sVersion += _T("Win32s");
			break;
	}

	return TRUE; 
}

BOOL CSystemPage::GetVistaVersion(CString& sVersion)
{	
	if ((g_VerInfo.dwMajorVersion == 6 && g_VerInfo.dwMinorVersion == 0) == false)
		return FALSE;
	
	if (g_VerInfo.wProductType == VER_NT_DOMAIN_CONTROLLER || g_VerInfo.wProductType == VER_NT_SERVER)
	{
		sVersion = _T("Windows Server 2008");
	}
	else
	{
		sVersion += _T("Windows Vista ");

		if (::GetSystemMetrics(SM_STARTER) != 0)
		{
			sVersion += _T("Starter ");
		}
		else if ((g_VerInfo.wSuiteMask & VER_SUITE_PERSONAL) != 0)
		{
			sVersion += _T("Home ");
		}
		else
		{
			sVersion += _T("Business ");
		}
	}

	return TRUE;
}

BOOL CSystemPage::GetPhysicalMemory(CString& sMemory)
{
	sMemory.Empty();	
	
	MEMORYSTATUSEX MemoryStatus;
	MemoryStatus.dwLength = sizeof(MemoryStatus);
	if (::GlobalMemoryStatusEx(&MemoryStatus) == FALSE)
		return FALSE;	

	// create a constant to represent 1MB
	const DWORD dwOneMB = 1024 * 1024;

	// calculate the number of MB on the machine (we need to round up)
	__int64 nPhysicalMemory = (__int64) MemoryStatus.ullTotalPhys;
	nPhysicalMemory += dwOneMB - 1;
	nPhysicalMemory /= dwOneMB;

	sMemory.Format(_T("%I64u"), nPhysicalMemory);	

	return TRUE;
}

BOOL CSystemPage::GetCPUType(CString& sCPU)
{
	sCPU.Empty();

	HKEY hCPU = NULL;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\Description\\System\\CentralProcessor\\0"), 0, KEY_READ, &hCPU) == ERROR_SUCCESS)
	{
		TCHAR szProcessorName[256] = _T("Unknown");
		DWORD dwProcessorNameType = 0;
		DWORD dwProcessorNameSize = sizeof(szProcessorName) / sizeof(szProcessorName[0]);
		if (::RegQueryValueEx(hCPU, _T("ProcessorNameString"), NULL, &dwProcessorNameType, (LPBYTE) &szProcessorName[0], &dwProcessorNameSize) == ERROR_SUCCESS)
		{			
			sCPU += szProcessorName;

			DWORD dwMHz = 0;
			DWORD dwMhzType = 0;
			DWORD dwMhzSize = sizeof(dwMHz);
			if (::RegQueryValueEx(hCPU, _T("~Mhz"), NULL, &dwMhzType, (LPBYTE) &dwMHz, &dwMhzSize) == ERROR_SUCCESS)
			{
				CString sMHz;
				sMHz.Format(_T("(%u MHz)"), dwMHz);

				sCPU += ' ';
				sCPU += sMHz;
			}
		}
		else
		{
			TCHAR szIdentifier[256] = _T("Unknown");
			DWORD dwIdentifierType = 0;
			DWORD dwIdentifierSize = sizeof(szIdentifier) / sizeof(szIdentifier[0]);
			if (::RegQueryValueEx(hCPU, _T("Identifier"), NULL, &dwIdentifierType, (LPBYTE) &szIdentifier[0], &dwIdentifierSize) == ERROR_SUCCESS)
			{
				sCPU += szIdentifier;
			}
		}

		::RegCloseKey(hCPU);
	}

	sCPU.TrimLeft();
	sCPU.TrimRight();

	return TRUE;
}

BOOL CSystemPage::GetIISVersion(CString& sVersion)
{
	sVersion.Empty();
	sVersion.LoadString(IDS_IISVERSION_NOTINSTALLED);

	TCHAR szSystemDir[512] = _T("");
	UINT nSystemDirSize = sizeof(szSystemDir) / sizeof(szSystemDir[0]);

	if (::GetSystemDirectory(szSystemDir, nSystemDirSize) == 0)
		return FALSE;

	CString sIISDir = szSystemDir;
	sIISDir += _T("\\inetsrv");

	CString sInetInfoExePath  = sIISDir;

	// on Windows 2003 (and prior) we can get the version number from INETINFO.EXE, this isn't present
	// on Vista/Longhorn, so use W3WP.EXE instead
	if (g_VerInfo.dwMajorVersion < 6)
	{
		sInetInfoExePath += _T("\\inetinfo.exe");
	}
	else
	{
		sInetInfoExePath += _T("\\w3wp.exe");
	}

	DWORD dwTemp = 0;
	DWORD dwVersionInfoSize = ::GetFileVersionInfoSize(sInetInfoExePath, &dwTemp);
	if (dwVersionInfoSize == 0)
		return FALSE;

	sVersion.LoadString(IDS_IISVERSION_UNKNOWN);

	BYTE* pbyVersionInfo = new BYTE[dwVersionInfoSize];
	if (pbyVersionInfo == NULL)
		return FALSE;

	BOOL bStatus = ::GetFileVersionInfo(sInetInfoExePath, 0, dwVersionInfoSize, pbyVersionInfo);
	if (bStatus == TRUE)
	{		
		VS_FIXEDFILEINFO* pFixedFileInfo;		
		UINT nFixedFileInfoSize = 0;

		bStatus = ::VerQueryValue(pbyVersionInfo, _T("\\"), (void**) &pFixedFileInfo, &nFixedFileInfoSize);

		if (bStatus == TRUE)
		{	
			sVersion.Format(_T("%u.%u"), HIWORD(pFixedFileInfo->dwProductVersionMS), LOWORD(pFixedFileInfo->dwProductVersionMS));
		}
	}

	delete[] pbyVersionInfo;

	return bStatus;
}

BOOL CSystemPage::GetASPNetVersion(CString& sVersion)
{	
	sVersion.Empty();
	sVersion.LoadString(IDS_ASPNET_VERSION_NOTINSTALLED);

	std::wstring sASPNETPath;	

	// for Windows 2003 and before we get the DLL path from the metabase
	if (g_VerInfo.dwMajorVersion < 6)
	{
		CIISMetaBase MetaBase;
		CComPtr<IMSAdminBase> pAdminBase;
		if (MetaBase.Init() == false ||
			MetaBase.GetAdminBase(&pAdminBase) == false)
			return FALSE;

		std::vector<std::wstring> saKeys;
		if (MetaBase.EnumKeys(_T("/LM/W3SVC/Filters"), saKeys) == false)
			return FALSE;		

		int nKeys = (int) saKeys.size();
		int i = 0;
		for (; i < nKeys; i++)
		{
			std::wstring sKey = saKeys[i];

			if (wcsncmp(sKey.c_str(), L"ASP.NET", 7) == 0)
			{
				CComBSTR bsPath = L"/LM/W3SVC/Filters/";
				bsPath += saKeys[i].c_str();

				CIISMetaBaseData ASPNETFilterPath;
				if (ASPNETFilterPath.ReadData(pAdminBase, bsPath, MD_FILTER_IMAGE_PATH) == true)
				{					
					ASPNETFilterPath.GetAsString(sASPNETPath);					
				}

				break;
			}
		}
	}
	else
	{
		// get the configuration document
		CComPtr<IXMLDOMDocument> pDoc;
		if (CIIS7XMLConfigHelper::GetApplicationHostConfigDocument(&pDoc) == S_OK)
		{
			// get the path to the DLL registered to handle ASPX
			CComPtr<IXMLDOMNodeList> pASPNETFilterNodes;
			HRESULT hr = pDoc->selectNodes(
				L"/configuration/location[@path='']/system.webServer/handlers/add[@path='*.aspx']/@scriptProcessor", 
				&pASPNETFilterNodes);

			// if we have the node then get the path
			if (hr == S_OK && pASPNETFilterNodes != NULL)
			{
				LONG nNodes = 0;
				hr = pASPNETFilterNodes->get_length(&nNodes);

				for (LONG i = 0; i < nNodes; i++)
				{
					CComPtr<IXMLDOMNode> pASPNETFilterNode;
					hr = pASPNETFilterNodes->get_item(i, &pASPNETFilterNode);

					if (hr != S_OK || pASPNETFilterNode == NULL)
						continue;

					CComBSTR bsASPNETFilterPath;
					hr = pASPNETFilterNode->get_text(&bsASPNETFilterPath);

					if (hr == S_OK && bsASPNETFilterPath.Length() > 0)
					{
						// expand the environment strings in the path
						CIIS7XMLConfigHelper::ExpandEnvironmentStrings(bsASPNETFilterPath, bsASPNETFilterPath);

						// we've got the path now
						sASPNETPath = bsASPNETFilterPath;
						break;
					}
				}
			}
		}
	}

	BOOL bFound = FALSE;
	if (sASPNETPath.size() > 0)
	{
		sVersion.LoadString(IDS_ASPNET_VERSION_UNKNOWN);

		DWORD dwTemp = 0;
		DWORD dwVersionInfoSize = ::GetFileVersionInfoSize(CString(sASPNETPath.c_str()), &dwTemp);
		if (dwVersionInfoSize > 0)
		{
			BYTE* pbyVersionInfo = new BYTE[dwVersionInfoSize];
			if (pbyVersionInfo != NULL)					
			{
				if (::GetFileVersionInfo(CString(sASPNETPath.c_str()), 0, dwVersionInfoSize, pbyVersionInfo) == TRUE)
				{
					VS_FIXEDFILEINFO* pFixedFileInfo;		
					UINT nFixedFileInfoSize = 0;

					if (::VerQueryValue(pbyVersionInfo, _T("\\"), (void**) &pFixedFileInfo, &nFixedFileInfoSize) == TRUE)
					{	
						sVersion.Format(_T("%u.%u"), 
							HIWORD(pFixedFileInfo->dwProductVersionMS), 
							LOWORD(pFixedFileInfo->dwProductVersionMS));

						bFound = TRUE;
					}
				}

				delete[] pbyVersionInfo;
			}
		}						
	}

	return bFound;
}

BOOL CSystemPage::GetIISCPULoad(CString& sCPULoad)
{
	sCPULoad.Empty();

	DWORD dwProcessID = -1;

	// if 2003+ is running then it's IIS6+ so look for the worker service
	if ((g_VerInfo.dwMajorVersion >= 5 && g_VerInfo.dwMinorVersion >= 1) ||
		g_VerInfo.dwMajorVersion >= 6)
	{
		dwProcessID = GetProcessID_EP(_T("w3wp.exe"));
	}

	// if it's XP or earlier (or the W3WP service wasn't running) then get inetinfo
	if (dwProcessID == -1)
	{
		dwProcessID = GetProcessID_EPM(_T("inetinfo.exe"));
	}
	
	if (dwProcessID == -1)
		return FALSE;

	DWORD dwOpenProcessFlags = PROCESS_QUERY_INFORMATION;
	if (g_VerInfo.dwMajorVersion >= 6)
	{
		// we don't have access to PROCESS_QUERY_LIMITED_INFORMATION, so use the constant instead
		dwOpenProcessFlags = 0x1000;
	}

	HANDLE hIISProcess = ::OpenProcess(dwOpenProcessFlags, FALSE, dwProcessID);
	if (hIISProcess != NULL)
	{
		if (m_nLastIISCPUTimeTickCount == 0)
		{
			m_nLastIISCPUTimeTickCount = g_Timer.GetTimerCount();						

			FILETIME ftCreated;
			FILETIME ftExited;
			FILETIME ftKernel;
			FILETIME ftUser;
			::GetProcessTimes(hIISProcess, &ftCreated, &ftExited, &ftKernel, &ftUser);

			unsigned __int64 nCPUKernelTime;
			MAKE_INT64(nCPUKernelTime, ftKernel.dwHighDateTime, ftKernel.dwLowDateTime);

			unsigned __int64 nCPUUserTime;
			MAKE_INT64(nCPUUserTime, ftUser.dwHighDateTime, ftUser.dwLowDateTime);

			m_nLastIISCPUTime = nCPUKernelTime + nCPUUserTime;
			m_nLastIISCPUTime /= 10000;		// convert from 100ns to ms

			::Sleep(200);
		}

		unsigned __int64 nNow = g_Timer.GetTimerCount();

		FILETIME ftCreated;
		FILETIME ftExited;
		FILETIME ftKernel;
		FILETIME ftUser;
		::GetProcessTimes(hIISProcess, &ftCreated, &ftExited, &ftKernel, &ftUser);

		unsigned __int64 nCPUKernelTime;
		MAKE_INT64(nCPUKernelTime, ftKernel.dwHighDateTime, ftKernel.dwLowDateTime);

		unsigned __int64 nCPUUserTime;
		MAKE_INT64(nCPUUserTime, ftUser.dwHighDateTime, ftUser.dwLowDateTime);

		unsigned __int64 nIISCPUTime = nCPUKernelTime + nCPUUserTime;
		nIISCPUTime /= 10000;		// convert from 100ns to ms
		
		unsigned __int64 nDeltaCPUTime = nIISCPUTime - m_nLastIISCPUTime;

		__int64 nPeriod = nNow - m_nLastIISCPUTimeTickCount;

		unsigned __int64 nCPULoad = nDeltaCPUTime * 100;
		nCPULoad /= nPeriod;
		nCPULoad /= m_dwNumberOfProcessors;

		sCPULoad.Format(_T("%I64u"), nCPULoad);

		m_nLastIISCPUTime = nIISCPUTime;
		m_nLastIISCPUTimeTickCount = nNow;

		::CloseHandle(hIISProcess);
	}		

	return TRUE;
}

BOOL CSystemPage::GetIISMemoryUsage(CString& sMemoryUsage)
{
	sMemoryUsage.Empty();

	DWORD dwProcessID = -1;

	// if 2003+ is running then it's IIS6+ so look for the worker service
	if ((g_VerInfo.dwMajorVersion >= 5 && g_VerInfo.dwMinorVersion >= 1) ||
		g_VerInfo.dwMajorVersion >= 6)
	{
		dwProcessID = GetProcessID_EP(_T("w3wp.exe"));
	}

	// if it's XP or earlier (or the W3WP service wasn't running) then get inetinfo
	if (dwProcessID == -1)
	{
		dwProcessID = GetProcessID_EPM(_T("inetinfo.exe"));
	}
	
	if (dwProcessID == -1)
		return FALSE;

	DWORD dwOpenProcessFlags = PROCESS_QUERY_INFORMATION;
	if (g_VerInfo.dwMajorVersion >= 6)
	{
		// we don't have access to PROCESS_QUERY_LIMITED_INFORMATION, so use the constant instead
		dwOpenProcessFlags = 0x1000;
	}

	HANDLE hIISProcess = ::OpenProcess(dwOpenProcessFlags, FALSE, dwProcessID);
	if (hIISProcess != NULL)
	{
		PROCESS_MEMORY_COUNTERS MemCounters; 
		if (::GetProcessMemoryInfo(hIISProcess, &MemCounters, sizeof(MemCounters)) != FALSE)
		{
			sMemoryUsage.Format(_T("%u"), MemCounters.WorkingSetSize / 1024);
		}

		::CloseHandle(hIISProcess);
	}

	return TRUE;
}

BOOL CSystemPage::GetIISxpressCPULoad(CString& sCPULoad)
{
	sCPULoad.Empty();

	DWORD dwProcessID = -1;
		
	if ((g_VerInfo.dwMajorVersion >= 5 && g_VerInfo.dwMinorVersion >= 1) ||
		g_VerInfo.dwMajorVersion >= 6)
	{
		dwProcessID = GetProcessID_EP(_T("iisxpressserver.exe"));
	}
	
	if (dwProcessID == -1)
	{
		dwProcessID = GetProcessID_EPM(_T("iisxpressserver.exe"));
	}

	DWORD dwOpenProcessFlags = PROCESS_QUERY_INFORMATION;
	if (g_VerInfo.dwMajorVersion >= 6)
	{
		// we don't have access to PROCESS_QUERY_LIMITED_INFORMATION, so use the constant instead
		dwOpenProcessFlags = 0x1000;
	}

	HANDLE hIISxpressProcess = ::OpenProcess(dwOpenProcessFlags, FALSE, dwProcessID);
	if (hIISxpressProcess != NULL)
	{
		if (m_nLastIISxpressCPUTimeTickCount == 0)
		{
			m_nLastIISxpressCPUTimeTickCount = g_Timer.GetTimerCount();						

			FILETIME ftCreated;
			FILETIME ftExited;
			FILETIME ftKernel;
			FILETIME ftUser;
			::GetProcessTimes(hIISxpressProcess, &ftCreated, &ftExited, &ftKernel, &ftUser);

			unsigned __int64 nCPUKernelTime;
			MAKE_INT64(nCPUKernelTime, ftKernel.dwHighDateTime, ftKernel.dwLowDateTime);

			unsigned __int64 nCPUUserTime;
			MAKE_INT64(nCPUUserTime, ftUser.dwHighDateTime, ftUser.dwLowDateTime);

			m_nLastIISxpressCPUTime = nCPUKernelTime + nCPUUserTime;
			m_nLastIISxpressCPUTime /= 10000;		// convert from 100ns to ms

			::Sleep(200);
		}

		unsigned __int64 nNow = g_Timer.GetTimerCount();

		FILETIME ftCreated;
		FILETIME ftExited;
		FILETIME ftKernel;
		FILETIME ftUser;
		::GetProcessTimes(hIISxpressProcess, &ftCreated, &ftExited, &ftKernel, &ftUser);

		unsigned __int64 nCPUKernelTime;
		MAKE_INT64(nCPUKernelTime, ftKernel.dwHighDateTime, ftKernel.dwLowDateTime);

		unsigned __int64 nCPUUserTime;
		MAKE_INT64(nCPUUserTime, ftUser.dwHighDateTime, ftUser.dwLowDateTime);

		unsigned __int64 nIISCPUTime = nCPUKernelTime + nCPUUserTime;
		nIISCPUTime /= 10000;		// convert from 100ns to ms
		
		unsigned __int64 nDeltaCPUTime = nIISCPUTime - m_nLastIISxpressCPUTime;

		__int64 nPeriod = nNow - m_nLastIISxpressCPUTimeTickCount;

		unsigned __int64 nCPULoad = nDeltaCPUTime * 100;
		nCPULoad /= nPeriod;
		nCPULoad /= m_dwNumberOfProcessors;

		sCPULoad.Format(_T("%I64u"), nCPULoad);

		m_nLastIISxpressCPUTime = nIISCPUTime;
		m_nLastIISxpressCPUTimeTickCount = nNow;

		::CloseHandle(hIISxpressProcess);

		m_cCPULoadChart.AppendPoint((double) nCPULoad);
	}		

	return TRUE;
}

BOOL CSystemPage::GetIISxpressMemoryUsage(CString& sMemoryUsage)
{
	sMemoryUsage.Empty();

	DWORD dwProcessID = -1;
		
	if ((g_VerInfo.dwMajorVersion >= 5 && g_VerInfo.dwMinorVersion >= 1) ||
		g_VerInfo.dwMajorVersion >= 6)
	{
		dwProcessID = GetProcessID_EP(_T("iisxpressserver.exe"));
	}
	
	if (dwProcessID == -1)
	{
		dwProcessID = GetProcessID_EPM(_T("iisxpressserver.exe"));
	}

	DWORD dwOpenProcessFlags = PROCESS_QUERY_INFORMATION;
	if (g_VerInfo.dwMajorVersion >= 6)
	{
		// we don't have access to PROCESS_QUERY_LIMITED_INFORMATION, so use the constant instead
		dwOpenProcessFlags = 0x1000;
	}

	HANDLE hIISProcess = ::OpenProcess(dwOpenProcessFlags, FALSE, dwProcessID);
	if (hIISProcess != NULL)
	{
		PROCESS_MEMORY_COUNTERS MemCounters; 
		if (::GetProcessMemoryInfo(hIISProcess, &MemCounters, sizeof(MemCounters)) != FALSE)
		{
			sMemoryUsage.Format(_T("%u"), MemCounters.WorkingSetSize / 1024);
		}

		::CloseHandle(hIISProcess);
	}

	return TRUE;
}

void CSystemPage::ServerOnline(void)
{
}

void CSystemPage::ServerOffline(void)
{
}

void CSystemPage::Heartbeat(void)
{
	UpdateControls();
}

// GetProcessID_EP seems more reliable on NT/W2K/XP systems, the TH version will work on 9x tho
DWORD CSystemPage::GetProcessID(LPCTSTR pszBinaryName)
{
	if (g_pfnCreateToolhelp32Snapshot != NULL && g_pfnProcess32First == NULL && g_pfnProcess32Next == NULL)
	{		
		return GetProcessID_TH(pszBinaryName);
	}
	else
	{
		return GetProcessID_EPM(pszBinaryName);
	}
}

DWORD CSystemPage::GetProcessID_TH(LPCTSTR pszBinaryName)
{
	DWORD dwID = -1;

	// we can't proceed unless these have loaded up, so return false
	if (g_pfnCreateToolhelp32Snapshot == NULL || g_pfnProcess32First == NULL || g_pfnProcess32Next == NULL)
	{		
		return dwID;
	}

	HANDLE hSnapshot = g_pfnCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return dwID;
	}

	PROCESSENTRY32 Process;
	Process.dwSize = sizeof(Process);
	if (g_pfnProcess32First(hSnapshot, &Process) == TRUE)
	{
		do
		{
			_tcslwr_s(Process.szExeFile, _countof(Process.szExeFile));

			if (_tcsstr(Process.szExeFile, pszBinaryName) != 0)
			{
				dwID = Process.th32ProcessID;
				break;
			}
		} while (g_pfnProcess32Next(hSnapshot, &Process) == TRUE);
	}

	::CloseHandle(hSnapshot);	

	return dwID;
}

typedef DWORD WINAPI FNGETPROCESSIMAGEFILENAME(HANDLE, LPTSTR, DWORD);

// GetProcessID_EP will only work with XP and 2003
DWORD CSystemPage::GetProcessID_EP(LPCTSTR pszBinaryName)
{
	if (pszBinaryName == NULL)
		return -1;

	if (g_fnGetProcessImageFileName == NULL)
		return -1;

	// get the length of the binary name we are going to look for
	int nBinaryNameLength = (int) _tcslen(pszBinaryName);

	bool bFound = false;
	DWORD dwID = -1;

	DWORD dwProcesses = 0;
	DWORD dwProcessIds[1024];

	// get a list of all the system processes
	if (::EnumProcesses(dwProcessIds, sizeof(dwProcessIds), &dwProcesses) == FALSE)
	{
		return dwID;
	}

	// figure out how many process handles were returned
	dwProcesses /= sizeof(DWORD);

	DWORD dwOpenProcessFlags = PROCESS_QUERY_INFORMATION;
	if (g_VerInfo.dwMajorVersion >= 6)
	{
		// we don't have access to PROCESS_QUERY_LIMITED_INFORMATION, so use the constant instead
		dwOpenProcessFlags = 0x1000;
	}

	for (DWORD i = 0; i < dwProcesses; i++)
	{
		// open the process (with very low rights)
		HANDLE hProcess = ::OpenProcess(dwOpenProcessFlags, FALSE, dwProcessIds[i]);
		if (hProcess != NULL)
		{
			// get the filename from the process handle
			TCHAR szModuleName[512];
			if (g_fnGetProcessImageFileName(hProcess, szModuleName, sizeof(szModuleName) / sizeof(szModuleName[0])) != 0)
			{
				// look for the last \ in the path
				LPCTSTR pszProcessModuleName = _tcsrchr(szModuleName, '\\');
				if (pszProcessModuleName != NULL)
				{
					// skip the \ character
					pszProcessModuleName++;
				}
				else
				{
					// there wasn't a \, so assume the string is just the binary name alone
					pszProcessModuleName = szModuleName;
				}

				if (_tcsicmp(pszBinaryName, pszProcessModuleName) == 0)
				{
					dwID = dwProcessIds[i];
					bFound = true;
				}
			}		

			::CloseHandle(hProcess);

			if (bFound == true)
				break;
		}
	}

	return dwID;
}

DWORD CSystemPage::GetProcessID_EPM(LPCTSTR pszBinaryName)
{
	bool bFound = false;
	DWORD dwID = -1;

	DWORD dwProcessIds[1024];

	DWORD dwProcesses = 0;
	if (::EnumProcesses(dwProcessIds, sizeof(dwProcessIds), &dwProcesses) == FALSE)
		return dwID;

	dwProcesses /= sizeof(DWORD);

	for (DWORD i = 0; i < dwProcesses; i++)
	{
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessIds[i]);
		if (hProcess == NULL)
			continue;

		HMODULE hModule = NULL;
		DWORD dwModules = 0;
		if (::EnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwModules) == TRUE)
		{
			TCHAR szModuleName[512];
			if (::GetModuleBaseName(hProcess, hModule, szModuleName, sizeof(szModuleName) / sizeof(szModuleName[0])) != 0)
			{				
				if (_tcsicmp(pszBinaryName, szModuleName) == 0)
				{
					dwID = dwProcessIds[i];
					bFound = true;
				}
			}
		}

		::CloseHandle(hProcess);

		if (bFound == true)
			break;
	}

	return dwID;
}