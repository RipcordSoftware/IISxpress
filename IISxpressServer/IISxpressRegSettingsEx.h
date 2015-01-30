#pragma once

#include "IISxpressRegSettings.h"

class CIISxpressRegSettingsEx :
	public CIISxpressRegSettings
{

public:

	CIISxpressRegSettingsEx(void);
	virtual ~CIISxpressRegSettingsEx(void);

	virtual void Init(LPCTSTR pszRegPath, bool bLoad, bool bRegisterChangeHandler = false);

	virtual bool Load(void);
	virtual bool Save(void);

	DWORD GetLicenseMode() { return m_dwLicenseMode; }
	bool SetLicenseMode(DWORD dwLicenseMode);

	BOOL GetCheckPhysicalMemory() { return m_dwCheckPhysicalMemory != 0; }
	BOOL GetCheckCPULoad() { return m_dwCheckCPULoad != 0; }
	BOOL GetCheckNeverRules() { return m_dwCheckNeverRules != 0; }
	BOOL GetCheckUserRules() { return m_dwCheckUserRules != 0; }
	BZIP2Mode GetBZIP2Mode() { return m_BZIP2Mode; }
	BOOL SetBZIP2Mode(BZIP2Mode mode);

	BOOL GetAHAEnabled() { return m_dwAHAEnabled != 0; }
	BOOL SetAHAEnabled(DWORD dwEnabled);

	BOOL GetAHADetectOverheat() { return m_dwAHADetectOverheat != 0; }
	BOOL SetAHADetectOverheat(DWORD dwDetectOverheat);

protected:

	volatile DWORD m_dwLicenseMode;
	volatile DWORD m_dwCheckPhysicalMemory;
	volatile DWORD m_dwCheckCPULoad;
	volatile DWORD m_dwCheckNeverRules;
	volatile DWORD m_dwCheckUserRules;
	volatile BZIP2Mode m_BZIP2Mode;

	volatile DWORD m_dwAHAEnabled;
	volatile DWORD m_dwAHADetectOverheat;
};
