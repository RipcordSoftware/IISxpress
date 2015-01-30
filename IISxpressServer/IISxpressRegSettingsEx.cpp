#include "StdAfx.h"
#include "iisxpressregsettingsex.h"

CIISxpressRegSettingsEx::CIISxpressRegSettingsEx(void) :
	m_dwLicenseMode((DWORD) LicenseModeCommunity),
	m_dwCheckPhysicalMemory(1),
	m_dwCheckCPULoad(1),
	m_dwCheckNeverRules(1),
	m_dwCheckUserRules(1),
	m_BZIP2Mode(BZIP2ModeStaticOnly),
	m_dwAHAEnabled(1),
	m_dwAHADetectOverheat(1)
{	
}

CIISxpressRegSettingsEx::~CIISxpressRegSettingsEx(void)
{
}


void CIISxpressRegSettingsEx::Init(LPCTSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler)
{
	CIISxpressRegSettings::Init(pszRegPath, bLoad, bRegisterChangeHandler);
}

bool CIISxpressRegSettingsEx::Load(void)
{
	bool bStatus = CIISxpressRegSettings::Load();

	if (bStatus == true)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		CRegKey Config;
		if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_READ) == ERROR_SUCCESS)
		{								
			// get the license mode
			DWORD dwLicenseMode = (DWORD) LicenseModeCommunity;
			Config.QueryDWORDValue(IISXPRESS_SERVER_LICENSEMODE, dwLicenseMode);					
			::InterlockedExchange((LONG*) &m_dwLicenseMode, dwLicenseMode);

			// get the physical memory check
			DWORD dwCheckPhysicalMemory = 1;
			Config.QueryDWORDValue(IISXPRESS_SERVER_CHECKPHYSICALMEMORY, dwCheckPhysicalMemory);					
			::InterlockedExchange((LONG*) &m_dwCheckPhysicalMemory, dwCheckPhysicalMemory);

			// get the CPU load check
			DWORD dwCheckCPULoad = 1;
			Config.QueryDWORDValue(IISXPRESS_SERVER_CHECKCPULOAD, dwCheckCPULoad);					
			::InterlockedExchange((LONG*) &m_dwCheckCPULoad, dwCheckCPULoad);

			// get the never rules check
			DWORD dwCheckNeverRules = 1;
			Config.QueryDWORDValue(IISXPRESS_SERVER_CHECKNEVERRULES, dwCheckNeverRules);					
			::InterlockedExchange((LONG*) &m_dwCheckNeverRules, dwCheckNeverRules);

			// get the user rules check
			DWORD dwCheckUserRules = 1;
			Config.QueryDWORDValue(IISXPRESS_SERVER_CHECKUSERRULES, dwCheckUserRules);					
			::InterlockedExchange((LONG*) &m_dwCheckUserRules, dwCheckUserRules);

			DWORD dwBZIP2Mode = 1;
			Config.QueryDWORDValue(IISXPRESS_SERVER_BZIP2MODE, dwBZIP2Mode);					
			::InterlockedExchange((LONG*) &m_BZIP2Mode, dwBZIP2Mode);			

			// only read the AHA entries if we are in AHA mode
			if (m_dwLicenseMode == LicenseModeAHA)
			{
				// AHA is enabled as default
				DWORD dwAHAEnabled = 1;
				Config.QueryDWORDValue(IISXPRESS_SERVER_AHA_ENABLED, dwAHAEnabled);					
				::InterlockedExchange((LONG*) &m_dwAHAEnabled, dwAHAEnabled);			

				DWORD dwAHADetectOverheat = 1;
				Config.QueryDWORDValue(IISXPRESS_SERVER_AHA_DETECTOVERHEAT, dwAHADetectOverheat);					
				::InterlockedExchange((LONG*) &m_dwAHADetectOverheat, dwAHADetectOverheat);						
			}
		}
	}

	return bStatus;
}

bool CIISxpressRegSettingsEx::Save()
{
	bool bStatus = CIISxpressRegSettings::Save();

	if (bStatus == true)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csConfig);

		CRegKey Config;
		if (Config.Open(HKEY_LOCAL_MACHINE, m_sRegPath, KEY_WRITE) == ERROR_SUCCESS)
		{											
			Config.SetDWORDValue(IISXPRESS_SERVER_LICENSEMODE, m_dwLicenseMode);								
			Config.SetDWORDValue(IISXPRESS_SERVER_BZIP2MODE, (DWORD) m_BZIP2Mode);								

			// only write the AHA entries if we are in AHA mode
			if (m_dwLicenseMode == LicenseModeAHA)
			{
				Config.SetDWORDValue(IISXPRESS_SERVER_AHA_ENABLED, (DWORD) m_dwAHAEnabled);								
				Config.SetDWORDValue(IISXPRESS_SERVER_AHA_DETECTOVERHEAT, (DWORD) m_dwAHADetectOverheat);	
			}
		}
	}

	return bStatus;
}

bool CIISxpressRegSettingsEx::SetLicenseMode(DWORD dwLicenseMode)
{
	m_dwLicenseMode = dwLicenseMode;	
	return Save();
}

BOOL CIISxpressRegSettingsEx::SetBZIP2Mode(BZIP2Mode mode)
{
	m_BZIP2Mode = mode;
	return Save();
}

BOOL CIISxpressRegSettingsEx::SetAHAEnabled(DWORD dwEnabled) 
{ 
	m_dwAHAEnabled = dwEnabled; 
	return Save(); 
}

BOOL CIISxpressRegSettingsEx::SetAHADetectOverheat(DWORD dwAHADetectOverheat) 
{ 
	m_dwAHADetectOverheat = dwAHADetectOverheat; 
	return Save(); 
}