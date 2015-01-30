// HistoryPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "HistoryPage.h"
#include "InternationalHelper.h"

#include "RGBtoHSV.h"

#include "resource.h"

// CHistoryPage dialog

IMPLEMENT_DYNAMIC(CHistoryPage, CIISxpressPageBase)

CHistoryPage::CHistoryPage()
	: CIISxpressPageBase(CHistoryPage::IDD)
{
	m_pNotify = NULL;
	m_dwCookie = -1;
	m_bAutoScroll = true;
	m_bEnabled = TRUE;
}

CHistoryPage::~CHistoryPage()
{
}

void CHistoryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_cHistory);	
	DDX_Control(pDX, IDC_EDIT_HOSTNAME, m_cHostname);
	DDX_Control(pDX, IDC_EDIT_CLIENTIP, m_cClientIP);
	DDX_Control(pDX, IDC_EDIT_CONTENTTYPE, m_cContentType);
	DDX_Control(pDX, IDC_STATIC_CLIENTBROWSER, m_cClientBrowser);
	DDX_Control(pDX, IDC_EDIT_URI, m_cURI);
	DDX_Control(pDX, IDC_EDIT_QUERYSTRING, m_cQueryString);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_cLocation);
}


BEGIN_MESSAGE_MAP(CHistoryPage, CIISxpressPageBase)
	ON_WM_DESTROY()	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_HISTORY, OnLvnItemChangedListHistory)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HISTORYCONTEXT_AUTOSCROLL, OnHistoryContextAutoScroll)
	ON_COMMAND(ID_HISTORYCONTEXT_CLEAR, OnHistoryContextClear)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISEXTENSION, OnHistoryContextExcludeExtn)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, OnHistoryContextExcludeCT)
	ON_COMMAND(ID_EXCLUDE_EXCLUDEMATCHINGCONTENTTYPES, OnHistoryContextExcludeMatchingCT)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISDIRECTORY, OnHistoryContextExcludeDirectory)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISDIRECTORYTREE, OnHistoryContextExcludeDirectoryTree)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISWEBSITE, OnHistoryContextExcludeWebSite)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISCLIENT, OnHistoryContextExcludeClient)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, OnHistoryContextExcludeSubnet)
	ON_COMMAND(ID_HISTORYCONTEXT_ENABLEHISTORY, OnHistoryContextEnableHistory)
	ON_MENUXP_MESSAGES()
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CHistoryPage, CIISxpressPageBase);

// CHistoryPage message handlers

BOOL CHistoryPage::OnInitDialog(void)
{
	BOOL bStatus = CIISxpressPageBase::OnInitDialog();

	m_cHistory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);

	CString sTimeColumn;
	sTimeColumn.LoadString(IDS_TIMECOLUMN);
	m_cHistory.InsertColumn(0, sTimeColumn, LVCFMT_LEFT, 65);

	CString sURIColumn;
	sURIColumn.LoadString(IDS_URICOLUMN);
	m_cHistory.InsertColumn(1, sURIColumn, LVCFMT_LEFT, 180);

	CString sOrigSizeColumn;
	sOrigSizeColumn.LoadString(IDS_ORIGSIZECOLUMN);
	m_cHistory.InsertColumn(2, sOrigSizeColumn, LVCFMT_RIGHT, 70);

	CString sCompSizeColumn;
	sCompSizeColumn.LoadString(IDS_COMPSIZECOLUMN);
	m_cHistory.InsertColumn(3, sCompSizeColumn, LVCFMT_RIGHT, 70);

	CString sRatioColumn;
	sRatioColumn.LoadString(IDS_RATIOCOLUMN);
	m_cHistory.InsertColumn(4, sRatioColumn, LVCFMT_RIGHT, 45);

	m_cHistory.RegisterCallback(CHistoryPage::OnCustomDrawCallback);		

	// create tooltip control and attach it to the edit controls
	m_cTips.Create(this);
	m_cTips.AddTool(&m_cURI);
	m_cTips.AddTool(&m_cQueryString);
	m_cTips.AddTool(&m_cHostname);
	m_cTips.AddTool(&m_cClientIP);
	m_cTips.AddTool(&m_cContentType);
	m_cTips.AddTool(&m_cClientBrowser);
	m_cTips.SetDelayTime(TTDT_INITIAL, 2000);
	m_cTips.SetDelayTime(TTDT_AUTOPOP, 8000);
	m_cTips.SetDelayTime(TTDT_RESHOW, 2000);	
	m_cTips.Activate(TRUE);

	if (m_pNotify == NULL)
	{
		CComObject<CHTTPNotify <CHTTPNotifyBase> >::CreateInstance(&m_pNotify);
		m_pNotify->AddRef();
		m_pNotify->Init(this);
	}

	RegisterConnectionPoint();	

	UpdateDetails();

	if (::LoadPNG(_T("FOLDEROPEN16.PNG"), m_bmpFolderOpen) == true &&
		::LoadPNG(_T("NETWORKGROUP16.PNG"), m_bmpNetworkGroup) == true &&
		::LoadPNG(_T("GENMIXEDMEDIADOC16.PNG"), m_bmpMixedMedia) == true &&
		::LoadPNG(_T("IDRDLL16.PNG"), m_bmpIDRDLL) == true)
	{
		CMenuXP::SetXPLookNFeel(this);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISDIRECTORY, &m_bmpFolderOpen);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, &m_bmpMixedMedia);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, &m_bmpNetworkGroup);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISEXTENSION, &m_bmpIDRDLL);

		m_PopupMenu.LoadMenu(IDR_MENU_HISTORYCONTEXT);
	}

	// recover the prevous history enabled state
	CIISxpressTrayApp* pApp = (CIISxpressTrayApp*) ::AfxGetApp();	
	m_bEnabled = pApp->GetProfileInt(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_HISTORYENABLED, TRUE);	

	return bStatus;
}

BOOL CHistoryPage::PreTranslateMessage(MSG* pMsg)
{
	// intercept mouse messages since the tooltip control needs to know about them
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	{		
		m_cTips.RelayEvent(pMsg);
	}

	return CIISxpressPageBase::PreTranslateMessage(pMsg);
}

BOOL CHistoryPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// intercept the tooltip's notification
	NMHDR* pNmHdr = (NMHDR*) lParam;
	if (pNmHdr->code == TTN_NEEDTEXT && pNmHdr->hwndFrom == m_cTips)
	{
		NMTTDISPINFO* pDispInfo = (NMTTDISPINFO*) lParam;

		HWND hwndControl = NULL;
		if ((pDispInfo->uFlags & TTF_IDISHWND) == 0)
		{			
			CToolInfo ToolInfo;
			if (m_cTips.GetToolInfo(ToolInfo, this, pDispInfo->hdr.idFrom) != TRUE ||
				(ToolInfo.uFlags & TTF_IDISHWND) == 0)
			{
				// we can't go any further
				return FALSE;
			}

			hwndControl = (HWND) ToolInfo.uId;
		}
		else
		{
			hwndControl = (HWND) pDispInfo->hdr.idFrom;
		}

		// get the text from the control into a static buffer
		static TCHAR szTooltipBuffer[512] = _T("\0");
		int nTipLength = ::GetWindowText(hwndControl, szTooltipBuffer, sizeof(szTooltipBuffer) / sizeof(szTooltipBuffer[0]));
		if (nTipLength <= 0)
			return FALSE;

		// get the first character index
		int nNearestChar = (int) ::SendMessage(hwndControl, EM_CHARFROMPOS, 0, MAKELONG(0, 0));
		if (nNearestChar < 0)
			return FALSE;

		// if the nearest char is zero then we have not scrolled
		if (nNearestChar == 0)
		{
			// we need to know the size of the control
			CRect rcControl;
			::GetClientRect(hwndControl, &rcControl);		

			// find out what index the char at the right of the control is at (take account of a border)
			nNearestChar = (int) ::SendMessage(hwndControl, EM_CHARFROMPOS, 0, 
				MAKELONG(rcControl.Width() - 1, 0));

			if (nNearestChar < 0 || nNearestChar == nTipLength)
				return FALSE;
		}

		// we need to display the tip
		pDispInfo->lpszText = szTooltipBuffer;		

		// nothing else should handle this, so return
		return TRUE;
	}

	return CIISxpressPageBase::OnNotify(wParam, lParam, pResult);
}

void CHistoryPage::OnDestroy(void)
{
	UnregisterConnectionPoint();

	// release the notify object
	if (m_pNotify != NULL)
	{
		m_pNotify->Release();
	}

	// free the active responses
	FreeActiveResponses();

	// free any entries in the history control
	FreeEntries();

	// store the history enabled state
	CIISxpressTrayApp* pApp = (CIISxpressTrayApp*) ::AfxGetApp();	
	pApp->WriteProfileInt(IISXPRESSTRAY_SETTINGS, IISXPRESSTRAY_HISTORYENABLED, m_bEnabled);	
}

HRESULT CHistoryPage::OnResponseStart(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwFilterContext)
{
	if (m_bEnabled == TRUE)
	{		
		m_csActiveResponses.Lock();	

		ActiveResponseColl::const_iterator iter = m_ActiveResponses.find(dwFilterContext);
		if (iter == m_ActiveResponses.end() || iter->second == NULL)
		{
			CIISResponseData* pData = CIISResponseData::Create(pIISInfo, pRequestInfo, pResponseInfo);
			pData->m_IISxpressResponseInfo.m_nStartCompression = Ripcord::Utils::HiResTimer::GetTicks();

			m_ActiveResponses[dwFilterContext] = pData;	
		}	

		m_csActiveResponses.Unlock();
	}

	return S_OK;
}

HRESULT CHistoryPage::OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
{
	if (m_ActiveResponses.size() == 0)
	{
		return S_OK;
	}

	m_csActiveResponses.Lock();

	ActiveResponseColl::const_iterator iter = m_ActiveResponses.find(dwFilterContext);
	if (iter != m_ActiveResponses.end() && iter->second != NULL)
	{
		// TODO: review this
		iter->second->m_IISxpressResponseInfo.m_nEndCompression = Ripcord::Utils::HiResTimer::GetTicks();
		iter->second->m_IISxpressResponseInfo.m_bCacheHit = FALSE;
		iter->second->m_IISxpressResponseInfo.m_bCompressed = TRUE;
		iter->second->m_IISxpressResponseInfo.m_dwCompressedSize = dwCompressedSize;

		SYSTEMTIME stTime;
		::GetLocalTime(&stTime);
		stTime.wMilliseconds = 0;

		CString sTime;
		if (!m_TimeStringCache.GetEntry(stTime, sTime))
		{				
			CInternationalHelper::Format24hTime(stTime, sTime);
			m_TimeStringCache.AddEntry(stTime, sTime);
		}

		int nItem = m_cHistory.InsertItem(m_cHistory.GetItemCount(), sTime);

		m_cHistory.SetItemText(nItem, 1, CString(iter->second->m_RequestInfo.m_sURI));

		CString sOriginalSize;
		if (!m_NumberStringCache.GetEntry(dwRawSize, sOriginalSize))
		{		
			sOriginalSize.Format(_T("%u"), dwRawSize);
			CInternationalHelper::FormatIntegerNumber(sOriginalSize);
			m_NumberStringCache.AddEntry(dwRawSize, sOriginalSize);
		}
		m_cHistory.SetItemText(nItem, 2, sOriginalSize);

		CString sCompressedSize;
		if (!m_NumberStringCache.GetEntry(dwCompressedSize, sCompressedSize))
		{		
			sCompressedSize.Format(_T("%u"), dwCompressedSize);
			CInternationalHelper::FormatIntegerNumber(sCompressedSize);
			m_NumberStringCache.AddEntry(dwCompressedSize, sCompressedSize);
		}
		m_cHistory.SetItemText(nItem, 3, sCompressedSize);

		float fRatio = (float) dwCompressedSize;
		fRatio /= dwRawSize;
		fRatio = 1.0f - fRatio;
		fRatio *= 100.0f;

		// if the ratio is negative then it means that the server hasn't been able to make the data
		// smaller and has returned the original response, so rather than show negative values
		// set the ratio to zero to denote no success
		if (fRatio < 0.0f)
			fRatio = 0.0f;

		CString sRatio;
		sRatio.Format(_T("%.0f%%"), fRatio);
		m_cHistory.SetItemText(nItem, 4, sRatio);

		if (m_bAutoScroll == true)
		{
			m_cHistory.EnsureVisible(nItem, FALSE);
		}
		
		m_cHistory.SetItemData(nItem, (DWORD_PTR) iter->second);

		m_ActiveResponses[dwFilterContext] = NULL;
	}

	// when we have more than 100 active responses in the map try to clean them up
	if (m_ActiveResponses.size() > 100)
	{
		ActiveResponseColl temp;

		ActiveResponseColl::const_iterator iter = m_ActiveResponses.begin();
		for (; iter != m_ActiveResponses.end(); iter++)
		{
			if (iter->second != NULL)
				temp[iter->first] = iter->second;
		}

		m_ActiveResponses.swap(temp);
	}

	m_csActiveResponses.Unlock();

	ShrinkEntries();

	return S_OK;
}

HRESULT CHistoryPage::OnCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize)
{
	if (m_bEnabled == FALSE)
	{
		return S_OK;
	}

	if (pIISInfo == NULL || pRequestInfo == NULL || pResponseInfo == NULL)
		return E_POINTER;

	SYSTEMTIME stTime;
	::GetLocalTime(&stTime);
	stTime.wMilliseconds = 0;

	CString sTime;
	if (!m_TimeStringCache.GetEntry(stTime, sTime))
	{				
		CInternationalHelper::Format24hTime(stTime, sTime);
		m_TimeStringCache.AddEntry(stTime, sTime);
	}

	int nItem = m_cHistory.InsertItem(m_cHistory.GetItemCount(), sTime);

	m_cHistory.SetItemText(nItem, 1, CString(pRequestInfo->pszURI));

	CString sOriginalSize;
	if (!m_NumberStringCache.GetEntry(pResponseInfo->dwContentLength, sOriginalSize))
	{		
		sOriginalSize.Format(_T("%u"), pResponseInfo->dwContentLength);
		CInternationalHelper::FormatIntegerNumber(sOriginalSize);
		m_NumberStringCache.AddEntry(pResponseInfo->dwContentLength, sOriginalSize);
	}
	m_cHistory.SetItemText(nItem, 2, sOriginalSize);

	CString sCompressedSize;
	if (!m_NumberStringCache.GetEntry(dwCompressedSize, sCompressedSize))
	{		
		sCompressedSize.Format(_T("%u"), dwCompressedSize);
		CInternationalHelper::FormatIntegerNumber(sCompressedSize);
		m_NumberStringCache.AddEntry(dwCompressedSize, sCompressedSize);
	}
	m_cHistory.SetItemText(nItem, 3, sCompressedSize);

	float fRatio = (float) dwCompressedSize;
	fRatio /= pResponseInfo->dwContentLength;
	fRatio = 1.0f - fRatio;
	fRatio *= 100.0f;

	// if the ratio is negative then it means that the server hasn't been able to make the data
	// smaller and has returned the original response, so rather than show negative values
	// set the ratio to zero to denote no success
	if (fRatio < 0.0f)
		fRatio = 0.0f;

	CString sRatio;
	sRatio.Format(_T("%.0f%%"), fRatio);
	m_cHistory.SetItemText(nItem, 4, sRatio);

	if (m_bAutoScroll == true)
	{
		m_cHistory.EnsureVisible(nItem, FALSE);
	}
	
	m_cHistory.SetItemData(nItem, 
		(DWORD_PTR) CIISResponseData::Create(pIISInfo, pRequestInfo, pResponseInfo, TRUE, TRUE, dwCompressedSize));	

	ShrinkEntries();

	return S_OK;
}

HRESULT CHistoryPage::OnResponseEnd(DWORD dwFilterContext)
{
	if (m_ActiveResponses.size() == 0)
	{
		return S_OK;
	}

	m_csActiveResponses.Lock();

	// we must free the response data from the active response map since it has ended
	CIISResponseData* pResponseData = m_ActiveResponses[dwFilterContext];
	if (pResponseData != NULL)
	{		
		CIISResponseData::Free(pResponseData);

		m_ActiveResponses[dwFilterContext] = NULL;
	}	

	m_csActiveResponses.Unlock();

	return S_OK;
}

HRESULT CHistoryPage::OnResponseAbort(DWORD dwFilterContext)
{
	return OnResponseEnd(dwFilterContext);
}

void CHistoryPage::ServerOnline(void)
{	
	RegisterConnectionPoint();
}

void CHistoryPage::ServerOffline(void)
{	
	UnregisterConnectionPoint();
}

void CHistoryPage::Heartbeat(void)
{	
}

bool CHistoryPage::RegisterConnectionPoint(void)
{
	if (m_pNotify == NULL)
		return false;

	UnregisterConnectionPoint();
	m_pHTTPNotifyCP.Release();

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
	if (pSheet->GetHTTPRequest(&pHTTPRequest) == S_OK)
	{
		CComQIPtr<IConnectionPointContainer> pConnPointCont = pHTTPRequest;
		if (pConnPointCont != NULL)
		{				
			if (pConnPointCont != NULL)
			{					
				HRESULT hr = pConnPointCont->FindConnectionPoint(IID_IIISxpressHTTPNotify, &m_pHTTPNotifyCP);
				if (m_pHTTPNotifyCP != NULL)
				{
					CComPtr<IUnknown> pUnk;
					m_pNotify->QueryInterface(IID_IUnknown, (void**) &pUnk);

					m_dwCookie = 0;
					hr = m_pHTTPNotifyCP->Advise(pUnk, &m_dwCookie);
				}
			}
		}
	}

	return true;
}

bool CHistoryPage::UnregisterConnectionPoint(void)
{
	if (m_pHTTPNotifyCP == NULL || m_dwCookie == -1)
		return false;

	HRESULT hr = m_pHTTPNotifyCP->Unadvise(m_dwCookie);
	if (hr != S_OK)
	{
		// now panic since the remote will not be releasing the ref count it has been holding,
		// so get rid of all ref counts except 1
		if (m_pNotify != NULL)
		{
			m_pNotify->Lock();

			while (m_pNotify->m_dwRef > 1)
			{
				m_pNotify->Release();
			}

			m_pNotify->Unlock();
		}
	}

	m_dwCookie = -1;

	// we can free the active responses now
	FreeActiveResponses();

	return true;
}

void CHistoryPage::UpdateDetails(void)
{
	CString sURI;
	CString sQueryString;
	CString sHostname;	
	CString sClientIP;
	CString sLocation;
	CString sContentType;
	CString sClientBrowser;

	POSITION pos = m_cHistory.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_cHistory.GetNextSelectedItem(pos);

		CIISResponseData* pIISResponseData = (CIISResponseData*) m_cHistory.GetItemData(nItem);
		if (pIISResponseData != NULL)
		{
			sURI = pIISResponseData->m_RequestInfo.m_sURI;
			sQueryString = pIISResponseData->m_RequestInfo.m_sQueryString;
			sHostname = pIISResponseData->m_RequestInfo.m_sHostname;			
			sContentType = pIISResponseData->m_ResponseInfo.m_sContentType;

			sClientIP = pIISResponseData->m_RequestInfo.m_sRemoteAddress;			
					
			// only locate IPv4 addresses						
			if (sClientIP.Find(_T(".")) > 0)
			{
				int nStart = 0;
				CString Octets[4];
				Octets[0] = sClientIP.Tokenize(_T("."), nStart);
				Octets[1] = sClientIP.Tokenize(_T("."), nStart);
				Octets[2] = sClientIP.Tokenize(_T("."), nStart);
				Octets[3] = sClientIP.Tokenize(_T(""), nStart);

				DWORD dwFirstOctet = _ttoi(Octets[0]);

				DWORD dwAddress = dwFirstOctet; dwAddress <<= 8;
				dwAddress |= _ttoi(Octets[1]); dwAddress <<= 8;
				dwAddress |= _ttoi(Octets[2]); dwAddress <<= 8;
				dwAddress |= _ttoi(Octets[3]);

				int nStartSearch = g_nFirstOctetOffsets[dwFirstOctet];
				if (nStartSearch >= 0)
				{					
					for (int i = nStartSearch; i < _countof(g_IPData); i++)
					{
						DWORD dwMaskedAddress = dwAddress & g_IPData[i].dwMask;
						if (dwMaskedAddress == g_IPData[i].dwAddress)
						{
							int nCountry = g_IPData[i].nCountry;							
							sLocation = g_pszCountryNames[nCountry];

							break;
						}
					}
				}
			}

			// if we don't find the location of the client then set it to unknown
			if (sLocation.GetLength() <= 0)
			{
				sLocation.LoadString(IDS_UNKNOWNLOCATION);
			}			

			// if it's IPv6 then apply special formatting rules - first find a tell tale :0: sequence
			int nOffset = sClientIP.Find(_T(":0:"));
			if (nOffset > 0)
			{	
				// remove the 0 from the :0:, this will leave ::
				sClientIP.Delete(nOffset + 1, 1);				
				
				// loop until all sequential ::0: sequences have been reduced
				while ((nOffset = sClientIP.Find(_T("::0:"))) > 0)
				{					
					sClientIP.Delete(nOffset + 2, 2);									
				}

				// if the address starts with 0:: then reduce that to :: as well
				if (sClientIP.Find(_T("0::")) == 0)
				{
					sClientIP.Delete(0, 1);
				}
			}			

			if (g_UserAgentLookup.GetUserAgentName(pIISResponseData->m_RequestInfo.m_sUserAgent, sClientBrowser) == false)
			{
				sClientBrowser = pIISResponseData->m_RequestInfo.m_sUserAgent;
			}			
		}
	}

	m_cURI.SetWindowText(sURI);
	m_cURI.SetSel(-1, 0);

	m_cQueryString.SetWindowText(sQueryString);
	m_cQueryString.SetSel(-1, 0);

	m_cHostname.SetWindowText(sHostname);
	m_cHostname.SetSel(-1, 0);

	m_cClientIP.SetWindowText(sClientIP);
	m_cClientIP.SetSel(-1, 0);

	m_cLocation.SetWindowText(sLocation);
	m_cLocation.SetSel(-1, 0);

	m_cContentType.SetWindowText(sContentType);
	m_cContentType.SetSel(-1, 0);

	m_cClientBrowser.SetWindowText(sClientBrowser);
	m_cClientBrowser.SetSel(-1, 0);
}

void CHistoryPage::OnLvnItemChangedListHistory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	
	if ((pNMLV->uNewState & LVIS_SELECTED) != 0)
	{
		UpdateDetails();
	}

	*pResult = 0;
}

void CHistoryPage::FreeEntries(void)
{
	int nItems = m_cHistory.GetItemCount();

	for (int i = 0; i < nItems; i++)
	{
		CIISResponseData* pIISResponseData = (CIISResponseData*) m_cHistory.GetItemData(i);
		if (pIISResponseData != NULL)
		{
			m_cHistory.SetItemData(i, NULL);			
			CIISResponseData::Free(pIISResponseData);
		}
	}
}

void CHistoryPage::ShrinkEntries(int nLeave, int nDeleteBlock)
{
	int nItems = m_cHistory.GetItemCount();
	if (nItems < nLeave)
		return;	

	int nRemove = nItems - nLeave;
	if (nRemove < nDeleteBlock)
		nRemove = nDeleteBlock;

	// switch off the redraw since the control will panic a bit otherwise
	m_cHistory.SetRedraw(FALSE);

	for (int i = 0; i < nRemove; i++)
	{
		CIISResponseData* pIISResponseData = (CIISResponseData*) m_cHistory.GetItemData(0);
		if (pIISResponseData != NULL)
		{						
			CIISResponseData::Free(pIISResponseData);
		}

		m_cHistory.DeleteItem(0);
	}

	// since we have removed items from the list the chances are the select item (if there is one)
	// will not be visible any more, if the user has switched off auto scroll then the user will most 
	// probably want to see it so let's scroll it into view
	if (m_cHistory.GetSelectedCount() > 0 && m_bAutoScroll == false)
	{
		POSITION pos = m_cHistory.GetFirstSelectedItemPosition();
		int nSel = m_cHistory.GetNextSelectedItem(pos);

		m_cHistory.EnsureVisible(nSel, FALSE);
	}

	// switch on the redraw and invalidate and redraw the control
	m_cHistory.SetRedraw(TRUE);
	m_cHistory.Invalidate();
	m_cHistory.UpdateWindow();
}

void CHistoryPage::OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult)
{
	static COLORREF clrText = ::GetSysColor(COLOR_HOTLIGHT);
	static COLORREF clrTextBk = ::GetSysColor(COLOR_INFOBK);
	static COLORREF clsAltTextBk = GetAlternateCacheLineColour(clrTextBk);

	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*) pNmHdr;

	DWORD dwDrawStage = pDraw->nmcd.dwDrawStage;
	if ((dwDrawStage & CDDS_ITEM) != 0)
	{
		if (pDraw->nmcd.lItemlParam != NULL && pWnd != NULL && ::IsWindowEnabled(pWnd->m_hWnd) == TRUE)
		{
			CIISResponseData* pData = (CIISResponseData*) pDraw->nmcd.lItemlParam;
			if (pData->m_IISxpressResponseInfo.m_bCacheHit == TRUE)
			{
				pDraw->clrText = clrText;
				pDraw->clrTextBk = clrTextBk;				

				if ((pDraw->nmcd.dwItemSpec & 1) != 0)
				{
					pDraw->clrTextBk = clsAltTextBk;
				}
			}
		}
	}
}

COLORREF CHistoryPage::GetAlternateCacheLineColour(COLORREF color)
{	
	// calculate the rgb values
	float r = (float) (color & 0xff);
	r /= 255;

	float g = (float) ((color >> 8) & 0xff);
	g /= 255;

	float b = (float) ((color >> 16) & 0xff);					
	b /= 255;

	// convert to HSV
	float h, s, v;
	RGBtoHSV(r, g, b, h, s, v);

	// reduce the brightness by 1%
	v *= 0.99f;

	// convert back to rgb
	HSVtoRGB(r, g, b, h, s, v);

	r *= 255;
	g *= 255;
	b *= 255;

	// make the new COLORREF
	color = RGB(r, g, b);
	return color;
}

void CHistoryPage::OnContextMenu(CWnd* pWnd, CPoint pos)
{	
	// only handle the content menu from the list control
	if (pWnd->GetSafeHwnd() != m_cHistory.GetSafeHwnd())
		return;

	// store the original position
	CPoint menuPos = pos;

	// get the list control window rect
	CRect rcHistory;
	m_cHistory.GetWindowRect(rcHistory);	

	// get the mouse pos
	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	// get the selected item
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);

	// convert the incoming pos to something useful if it came from the keyboard
	if (pos.x < 0 && pos.y < 0)
	{						
		if (nSelectedItem >= 0)
		{
			// get the selected item's rect
			CRect rcItem;
			m_cHistory.GetItemRect(nSelectedItem, &rcItem, LVIR_LABEL);

			// map the selected item's rect to screen coords
			CPoint ptItem(rcItem.CenterPoint());
			m_cHistory.ClientToScreen(&ptItem);
			pos = ptItem;
		}		
	}	

	CMenu* pSubMenu = m_PopupMenu.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;

	if (m_bEnabled == TRUE)
	{
		CheckMenuItem(pSubMenu, ID_HISTORYCONTEXT_ENABLEHISTORY, MF_CHECKED);

		EnableMenuItem(pSubMenu, ID_HISTORYCONTEXT_AUTOSCROLL, MF_ENABLED);
		EnableMenuItem(pSubMenu, ID_HISTORYCONTEXT_CLEAR, MF_ENABLED);

		if (m_bAutoScroll == TRUE)
		{
			CheckMenuItem(pSubMenu, ID_HISTORYCONTEXT_AUTOSCROLL, MF_CHECKED);
		}	
		else
		{
			CheckMenuItem(pSubMenu, ID_HISTORYCONTEXT_AUTOSCROLL, MF_UNCHECKED);
		}

		CPoint ptClient(pos);
		m_cHistory.ScreenToClient(&ptClient);

		UINT nHitTestFlags = 0;
		int nItem = m_cHistory.HitTest(ptClient, &nHitTestFlags);
		if ((nHitTestFlags & LVHT_ONITEM) != 0 && nItem >= 0)
		{
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISEXTENSION, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDEMATCHINGCONTENTTYPES, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISDIRECTORY, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISDIRECTORYTREE, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISWEBSITE, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCLIENT, MF_ENABLED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, MF_ENABLED);
		}
		else
		{
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISEXTENSION, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDEMATCHINGCONTENTTYPES, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISDIRECTORY, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISDIRECTORYTREE, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISWEBSITE, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCLIENT, MF_GRAYED);
			EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, MF_GRAYED);
		}
	}	
	else
	{
		CheckMenuItem(pSubMenu, ID_HISTORYCONTEXT_ENABLEHISTORY, MF_UNCHECKED);

		EnableMenuItem(pSubMenu, ID_HISTORYCONTEXT_AUTOSCROLL, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_HISTORYCONTEXT_CLEAR, MF_GRAYED);

		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISEXTENSION, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDEMATCHINGCONTENTTYPES, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISDIRECTORY, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISDIRECTORYTREE, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISWEBSITE, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCLIENT, MF_GRAYED);
		EnableMenuItem(pSubMenu, ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, MF_GRAYED);
	}	

	if (pos.x < 0 && pos.y < 0)
	{
		// there isn't a selection, just pick the top left
		pos = rcHistory.TopLeft();
	}

	// if the cursor is inside the list control rect then just display at that position
	if (rcHistory.PtInRect(ptCursor) == TRUE)
	{
		pos = ptCursor;
	}

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 		
			pos.x, pos.y, this);
}

bool CHistoryPage::CheckMenuItem(CMenu* pMenu, UINT nID, UINT nCheck)
{
	if (pMenu == NULL)
		return false;	

	int nItems = pMenu->GetMenuItemCount();
	for (int i = 0; i < nItems; i++)
	{
		UINT nItemID = pMenu->GetMenuItemID(i);
		if (nItemID == nID)
		{
			pMenu->CheckMenuItem(nID, MF_BYCOMMAND | nCheck);
			return true;
		}
		else if (nItemID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);
			if (pSubMenu != NULL)
			{
				if (CheckMenuItem(pSubMenu, nID, nCheck) == true)
					return true;
			}
		}
	}

	return false;
}

bool CHistoryPage::EnableMenuItem(CMenu* pMenu, UINT nID, UINT nEnable)
{
	if (pMenu == NULL)
		return false;	

	int nItems = pMenu->GetMenuItemCount();
	for (int i = 0; i < nItems; i++)
	{
		UINT nItemID = pMenu->GetMenuItemID(i);
		if (nItemID == nID)
		{
			pMenu->EnableMenuItem(nID, MF_BYCOMMAND | nEnable);
			return true;
		}
		else if (nItemID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);
			if (pSubMenu != NULL)
			{
				if (EnableMenuItem(pSubMenu, nID, nEnable) == true)
					return true;
			}
		}
	}

	return false;
}

void CHistoryPage::OnHistoryContextAutoScroll()
{
	m_bAutoScroll = !m_bAutoScroll;	
}

void CHistoryPage::OnHistoryContextClear()
{
	FreeEntries();
	m_cHistory.DeleteAllItems();
	UpdateDetails();
}

void CHistoryPage::OnHistoryContextExcludeExtn()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CPathT<CStringA> path(pData->m_IISInfo.m_sURLMapPath);
	CStringA sExtension = path.GetExtension();
	if (sExtension.GetLength() <= 0)
		return;

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddExtensionRule(sExtension, "", "", 0);
	if (hr == S_OK)
	{
		pSheet->ResetResponseCache();

		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_EXTENSIONEXCLUSIONADDED, CString(sExtension));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeCT()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CStringA sContentType = pData->m_ResponseInfo.m_sContentType;

	// get rid of the stuff after the semi-colon if there is one
	int nSemiColon = sContentType.Find(';');
	if (nSemiColon > 0)		
	{		
		sContentType = sContentType.Left(nSemiColon);
	}

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddContentTypeRule(sContentType, "", "", 0);
	if (hr == S_OK)
	{
		pSheet->ResetResponseCache();

		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_CONTENTTYPEEXCLUSIONADDED, CString(sContentType));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeMatchingCT()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CStringA sMatchingCT = pData->m_ResponseInfo.m_sContentType;

	int nFirstSlash = sMatchingCT.Find('/');
	if (nFirstSlash < 0)
		return;

	nFirstSlash++;

	sMatchingCT = sMatchingCT.Left(nFirstSlash);
	sMatchingCT += '*';

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddContentTypeRule(sMatchingCT, "", "", 0);
	if (hr == S_OK)
	{
		pSheet->ResetResponseCache();

		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_CONTENTTYPEEXCLUSIONADDED, CString(sMatchingCT));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeDirectory()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CStringA sUri = pData->m_RequestInfo.m_sURI;

	int nLastSlash = sUri.ReverseFind('/');
	if (nLastSlash < 0)
		return;

	nLastSlash++;

	sUri = sUri.Left(nLastSlash);

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(pData->m_IISInfo.m_sInstanceId, sUri, RULEFLAGS_FOLDER_EXACTMATCH, 0);
	if (hr == S_OK)
	{
		pSheet->ResetResponseCache();

		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_URIEXCLUSIONRULEADDED, CString(sUri));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeDirectoryTree()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CStringA sUri = pData->m_RequestInfo.m_sURI;

	int nLastSlash = sUri.ReverseFind('/');
	if (nLastSlash < 0)
		return;

	nLastSlash++;

	sUri = sUri.Left(nLastSlash);

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(pData->m_IISInfo.m_sInstanceId, sUri, RULEFLAGS_FOLDER_WILDCARDMATCH, 0);
	if (hr == S_OK)
	{
		pSheet->ResetResponseCache();

		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_URIEXCLUSIONRULEADDED, CString(sUri));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeWebSite()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;		

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(pData->m_IISInfo.m_sInstanceId, "/", RULEFLAGS_FOLDER_WILDCARDMATCH, 0);
	if (hr == S_OK)
	{
		pSheet->ResetResponseCache();

		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_URIEXCLUSIONRULEADDED, _T("/"));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeClient()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	// we don't support IPv6 rules
	if (pData->m_RequestInfo.m_sRemoteAddress.Find(':') >= 0)
		return;

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddIPRule(pData->m_RequestInfo.m_sRemoteAddress, 0);	
	if (hr == S_OK)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_IPEXCLUSIONRULEADDED, CString(pData->m_RequestInfo.m_sRemoteAddress));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextExcludeSubnet()
{
	POSITION selectedPos = m_cHistory.GetFirstSelectedItemPosition();
	int nSelectedItem = m_cHistory.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = m_cHistory.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	// we don't support IPv6 rules
	if (pData->m_RequestInfo.m_sRemoteAddress.Find(':') >= 0)
		return;

	CStringA sSubnet = pData->m_RequestInfo.m_sRemoteAddress;
	int LastOctet = sSubnet.ReverseFind('.');
	if (LastOctet < 0)
		return;

	sSubnet = sSubnet.Left(LastOctet);

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddIPRule(sSubnet, 0);	
	if (hr == S_OK)
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.Format(IDS_IPEXCLUSIONRULEADDED, CString(sSubnet));

		MessageBox(sMsg, sCaption, MB_ICONINFORMATION);
	}
	else 
	{
		CString sCaption;
		sCaption.LoadString(IDS_IISXPRESS_APPNAME);

		CString sMsg;
		sMsg.LoadString(IDS_ERRORADDINGEXCLUSIONRULE);

		MessageBox(sMsg, sCaption, MB_ICONEXCLAMATION);
	}
}

void CHistoryPage::OnHistoryContextEnableHistory()
{
	m_bEnabled = !m_bEnabled;

	if (m_bEnabled == FALSE)
	{
		FreeActiveResponses();
		FreeEntries();
		m_cHistory.DeleteAllItems();
		UpdateDetails();
	}
}

void CHistoryPage::FreeActiveResponses()
{
	m_csActiveResponses.Lock();

	ActiveResponseColl::iterator iter = m_ActiveResponses.begin();
	for (; iter != m_ActiveResponses.end(); iter++)
	{
		if (iter->second != NULL)
		{
			CIISResponseData::Free(iter->second);			
		}
	}

	m_ActiveResponses.clear();

	m_csActiveResponses.Unlock();
}