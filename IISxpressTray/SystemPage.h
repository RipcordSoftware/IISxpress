#pragma once
#include "afxwin.h"

#include "OScopeCtrl.h"

// CSystemPage dialog

class CSystemPage : public CIISxpressPageBase
{
	DECLARE_DYNAMIC(CSystemPage)

public:
	CSystemPage();
	virtual ~CSystemPage();

// Dialog Data
	enum { IDD = IDD_PAGE_SYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);

	DECLARE_MESSAGE_MAP()

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	void UpdateControls(void);

	BOOL GetWindowsVersion(CString& sVersion);
	BOOL GetVistaVersion(CString& sVersion);	

	BOOL GetPhysicalMemory(CString& sMemory);
	BOOL GetCPUType(CString& sCPU);
	BOOL GetIISVersion(CString& sVersion);
	BOOL GetASPNetVersion(CString& sVersion);
	BOOL GetIISCPULoad(CString& sCPULoad);
	BOOL GetIISMemoryUsage(CString& sMemoryUsage);
	BOOL GetIISxpressCPULoad(CString& sCPULoad);
	BOOL GetIISxpressMemoryUsage(CString& sMemoryUsage);

	DWORD GetProcessID(LPCTSTR pszBinaryName);
	DWORD GetProcessID_TH(LPCTSTR pszBinaryName);
	DWORD GetProcessID_EP(LPCTSTR pszBinaryName);
	DWORD GetProcessID_EPM(LPCTSTR pszBinaryName);

	CEdit m_cWindowsVersion;
	CEdit m_cPhysicalMemory;
	CEdit m_cCPUType;
	CEdit m_cIISVersion;
	CEdit m_cASPNetVersion;
	CEdit m_cIISCPULoad;
	CEdit m_cIISMemoryUsage;
	CEdit m_cIISxpressCPULoad;
	CEdit m_cIISxpressMemoryUsage;
	CStatic m_cCPULoadChartPlaceholder;
	COScopeCtrl m_cCPULoadChart;

	unsigned __int64	m_nLastIISCPUTime;
	__int64				m_nLastIISCPUTimeTickCount;

	unsigned __int64	m_nLastIISxpressCPUTime;
	__int64				m_nLastIISxpressCPUTimeTickCount;

	DWORD				m_dwNumberOfProcessors;
};
