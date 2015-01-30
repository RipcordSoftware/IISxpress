// ComIISxpressRegSettings.cpp : Implementation of CComIISxpressRegSettings

#include "stdafx.h"
#include "ComIISxpressRegSettings.h"


// CComIISxpressRegSettings

bool CComIISxpressRegSettings::Init(LPCTSTR pszRegPath)
{
	m_RegSettings.Init(pszRegPath, true, false);
	return true;
}

STDMETHODIMP CComIISxpressRegSettings::get_Enabled(BOOL* pbEnabled)
{
	if (pbEnabled == NULL)
		return E_POINTER;	

	bool bEnabled = m_RegSettings.GetEnabled();
	if (bEnabled == true)
	{
		*pbEnabled = TRUE;
	}
	else
	{
		*pbEnabled = FALSE;
	}

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::put_Enabled(BOOL bEnabled)
{
	m_RegSettings.SetEnabled(bEnabled == TRUE ? true : false);
	return S_OK;	
}

STDMETHODIMP CComIISxpressRegSettings::get_DebugEnabled(BOOL* pbEnabled)
{
	if (pbEnabled == NULL)
		return E_POINTER;

	bool bEnabled = m_RegSettings.GetDebugEnabled();
	if (bEnabled == true)
	{
		*pbEnabled = TRUE;
	}
	else
	{
		*pbEnabled = FALSE;
	}

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::put_DebugEnabled(BOOL bEnabled)
{
	/*m_RegSettings.SetDebugEnabled(bEnabled == TRUE ? true : false);
	return S_OK;	*/
	return E_NOTIMPL;
}

STDMETHODIMP CComIISxpressRegSettings::get_LoggingLevel(DWORD* pdwLevel)
{
	if (pdwLevel == NULL)
		return E_POINTER;	

	*pdwLevel = m_RegSettings.GetLoggingLevel();
	return S_OK;	
}

STDMETHODIMP CComIISxpressRegSettings::put_LoggingLevel(DWORD dwLevel)
{
	m_RegSettings.SetLoggingLevel(dwLevel);
	return S_OK;	
}

STDMETHODIMP CComIISxpressRegSettings::get_CheckPhysicalMemory(BOOL* pbCheck)
{
	if (pbCheck == NULL)
		return E_POINTER;

	*pbCheck = m_RegSettings.GetCheckPhysicalMemory();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_CheckCPULoad(BOOL* pbCheck)
{
	if (pbCheck == NULL)
		return E_POINTER;

	*pbCheck = m_RegSettings.GetCheckCPULoad();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_CheckNeverRules(BOOL* pbCheck)
{
	if (pbCheck == NULL)
		return E_POINTER;

	*pbCheck = m_RegSettings.GetCheckNeverRules();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_CheckUserRules(BOOL* pbCheck)
{
	if (pbCheck == NULL)
		return E_POINTER;

	*pbCheck = m_RegSettings.GetCheckUserRules();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_LicenseMode(LicenseMode* pMode)
{
	if (pMode == NULL)
		return E_POINTER;

	*pMode = (LicenseMode) m_RegSettings.GetLicenseMode();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::put_LicenseMode(LicenseMode Mode)
{
	m_RegSettings.SetLicenseMode((DWORD) Mode);
	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_BZIP2Mode(BZIP2Mode* pMode)
{
	if (pMode == NULL)
		return E_POINTER;

	*pMode = m_RegSettings.GetBZIP2Mode();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::put_BZIP2Mode(BZIP2Mode mode)
{
	m_RegSettings.SetBZIP2Mode(mode);
	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_AHAEnabled(DWORD* pdwEnabled)
{
	if (pdwEnabled == NULL)
		return E_POINTER;

	*pdwEnabled = m_RegSettings.GetAHAEnabled();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::put_AHAEnabled(DWORD dwEnabled)
{
	m_RegSettings.SetAHAEnabled(dwEnabled);
	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::get_AHADetectOverheat(DWORD* pdwDetectOverheat)
{
	if (pdwDetectOverheat == NULL)
		return E_POINTER;

	*pdwDetectOverheat = m_RegSettings.GetAHADetectOverheat();

	return S_OK;
}

STDMETHODIMP CComIISxpressRegSettings::put_AHADetectOverheat(DWORD dwDetectOverheat)
{
	m_RegSettings.SetAHADetectOverheat(dwDetectOverheat);
	return S_OK;
}