#pragma once

#include "stdafx.h"

#include "resource.h"
#include "afxwin.h"

#include "HTTPNotify.h"
#include "HTTPNotifyBase.h"

// CGeneralPropertyPage dialog

// we need a message for updating the controls
#define WM_UPDATEACTIVITYCONTROLS	(WM_USER + 1)

class CGeneralPropertyPage : public CIISxpressPageBase, public CHTTPNotifyBase
{
	DECLARE_DYNAMIC(CGeneralPropertyPage)

public:
	CGeneralPropertyPage();
	virtual ~CGeneralPropertyPage();

// Dialog Data
	enum { IDD = IDD_PAGE_GENERAL };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog(void);
	void OnDestroy(void);
	void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

protected:

	virtual HRESULT OnResponseStart(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwFilterContext);

	virtual HRESULT OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize);

	virtual HRESULT OnResponseEnd(DWORD dwFilterContext);

	virtual HRESULT OnResponseAbort(DWORD dwFilterContext);

	virtual HRESULT OnCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize);

protected:

	virtual void ServerOnline(void);
	virtual void ServerOffline(void);
	virtual void Heartbeat(void);

private:

	void UpdateIISState(void);
	void UpdateFilterState(void);
	void UpdateServerState(void);
	void UpdateActivityControls(void);

	// windows message handler version of UpdateActivityControls
	LRESULT UpdateActivityControls(WPARAM, LPARAM);

	bool RegisterConnectionPoint(void);
	bool UnregisterConnectionPoint(void);

	bool GetIIS7ModuleInstalledState(int bitness);

	CEdit m_cIISStatus;
	CEdit m_cFilterStatus;
	CEdit m_cServerStatus;
	CEdit m_cNumCompressedResponses;
	CEdit m_cCacheHits;
	CEdit m_cDataIn;
	CEdit m_cDataOut;
	CEdit m_cCompressionRatio;
	CStatic m_cDataInUnits;
	CStatic m_cDataOutUnits;

	//CComPtr<IConnectionPoint>		m_pHTTPNotifyCP;
	//CComObject<CHTTPNotify>*		m_pNotify;
	//DWORD							m_dwCookie;

	CString		m_sKBUnits;	
	CString		m_sMBUnits;	
};
