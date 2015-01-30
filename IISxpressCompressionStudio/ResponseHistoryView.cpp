// ResponseHistoryView.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressCompressionStudio.h"
#include "ResponseHistoryView.h"

// CResponseHistoryView

IMPLEMENT_DYNCREATE(CResponseHistoryView, CColouredListView)

CResponseHistoryView::CResponseHistoryView() : m_pNotify(NULL), m_dwCookie(-1), m_bAutoScroll(true)
{
	BOOL bStatus = m_sCacheStatusYes.LoadString(IDS_CACHESTATUS_YES);
	ASSERT(bStatus);
	bStatus = m_sCacheStatusNo.LoadString(IDS_CACHESTATUS_NO);
	ASSERT(bStatus);
}

CResponseHistoryView::~CResponseHistoryView()
{
}

BEGIN_MESSAGE_MAP(CResponseHistoryView, CColouredListView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_MENUXP_MESSAGES()
	ON_COMMAND(ID_HISTORYCONTEXT_CLEAR, OnClear)
	ON_COMMAND(ID_HISTORYCONTEXT_AUTOSCROLL, OnAutoScroll)
	ON_COMMAND(ID_HISTORYCONTEXT_WRITETOCSV, OnWriteToCSV)
	ON_COMMAND(ID_HISTORYCONTEXT_EXPLORECONTAININGFOLDER, OnExploreContainingFolder)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISEXTENSION, OnHistoryContextExcludeExtn)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDEEXTN, OnHistoryContextExcludeExtn)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, OnHistoryContextExcludeCT)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDECT, OnHistoryContextExcludeCT)
	ON_COMMAND(ID_EXCLUDE_EXCLUDEMATCHINGCONTENTTYPES, OnHistoryContextExcludeMatchingCT)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDECT_MATCH, OnHistoryContextExcludeMatchingCT)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISDIRECTORY, OnHistoryContextExcludeDirectory)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDEDIR, OnHistoryContextExcludeDirectory)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISDIRECTORYTREE, OnHistoryContextExcludeDirectoryTree)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDEDIRTREE, OnHistoryContextExcludeDirectoryTree)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISWEBSITE, OnHistoryContextExcludeWebSite)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDEWEBSITE, OnHistoryContextExcludeWebSite)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISCLIENT, OnHistoryContextExcludeClient)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDECLIENTIP, OnHistoryContextExcludeClient)
	ON_COMMAND(ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, OnHistoryContextExcludeSubnet)
	ON_COMMAND(IDC_BUTTON_TOOLBAR_EXCLUDENETWORK, OnHistoryContextExcludeSubnet)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_URI, OnHeaderContextURI)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_HOSTNAME, OnHeaderContextHostname)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_PORT, OnHeaderContextPort)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_QUERYSTRING, OnHeaderContextQueryString)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_CACHED, OnHeaderContextCached)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_ORIGINALSIZE, OnHeaderContextOriginalSize)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_COMPRESSEDSIZE, OnHeaderContextCompressedSize)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_RATIO, OnHeaderContextRatio)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_CONTENTTYPE, OnHeaderContextContentType)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_CLIENTIP, OnHeaderContextClientIP)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_LOCATION, OnHeaderContextLocation)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_BROWSER, OnHeaderContextBrowser)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_USERAGENT, OnHeaderContextUserAgent)
	ON_COMMAND(ID_HISTORYHEADERCONTEXT_COMPRESSIONTIME, OnHeaderContextCompressionTime)
	
	ON_UPDATE_COMMAND_UI(ID_HISTORYCONTEXT_CLEAR, OnCmdUIClear)
	ON_UPDATE_COMMAND_UI(ID_HISTORYCONTEXT_AUTOSCROLL, OnCmdUIAutoScroll)
	ON_UPDATE_COMMAND_UI(ID_HISTORYCONTEXT_WRITETOCSV, OnCmdUIWriteToCSV)
	ON_UPDATE_COMMAND_UI(ID_HISTORYCONTEXT_EXPLORECONTAININGFOLDER, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDEEXTN, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISEXTENSION, OnCmdUIListViewHasSelectedItem)	
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDECT, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDECT_MATCH, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDEMATCHINGCONTENTTYPES, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDEDIR, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISDIRECTORY, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDEDIRTREE, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISDIRECTORYTREE, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDEWEBSITE, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISWEBSITE, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDECLIENTIP, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISCLIENT, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TOOLBAR_EXCLUDENETWORK, OnCmdUIListViewHasSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, OnCmdUIListViewHasSelectedItem)
END_MESSAGE_MAP()

IMPLEMENT_MENUXP(CResponseHistoryView, CColouredListView);

IMPLEMENT_HANDLECONTEXTMENUMESSAGES(CResponseHistoryView)

// CResponseHistoryView diagnostics

#ifdef _DEBUG
void CResponseHistoryView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CResponseHistoryView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CResponseHistoryView message handlers

BOOL CResponseHistoryView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER;

	return CListView::PreCreateWindow(cs);
}

int CResponseHistoryView::OnCreate(LPCREATESTRUCT lpcs)
{
	CListView::OnCreate(lpcs);

	CListCtrl& cListCtrl = GetListCtrl();	

	cListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP | LVS_EX_HEADERDRAGDROP);

	CreateColumns();

	RegisterCallback(&CResponseHistoryView::OnCustomDrawCallback);		

	if (m_pNotify == NULL)
	{
		CComObject<CHTTPNotify <CHTTPNotifyBase> >::CreateInstance(&m_pNotify);
		m_pNotify->AddRef();
		m_pNotify->Init(this);
	}

	RegisterConnectionPoint();	

	m_HeaderPopupMenu.LoadMenu(IDR_MENU_HISTORYHEADERCONTEXT);
	m_ListPopupMenu.LoadMenu(IDR_MENU_HISTORYCONTEXT);

	CMenuXP::SetXPLookNFeel(this);

	if (CPNGHelper::LoadPNG(_T("FOLDEROPEN16.PNG"), m_bmpFolderOpen) == true &&
		CPNGHelper::LoadPNG(_T("NETWORKGROUP16.PNG"), m_bmpNetworkGroup) == true &&
		CPNGHelper::LoadPNG(_T("GENMIXEDMEDIADOC16.PNG"), m_bmpMixedMedia) == true &&
		CPNGHelper::LoadPNG(_T("IDRDLL16.PNG"), m_bmpIDRDLL) == true)
	{		
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISDIRECTORY, &m_bmpFolderOpen);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISCONTENTTYPE, &m_bmpMixedMedia);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISCLIENTSUBNET, &m_bmpNetworkGroup);
		CMenuXP::SetMenuItemImage(ID_EXCLUDE_EXCLUDETHISEXTENSION, &m_bmpIDRDLL);
	}	

	return 0;
}

void CResponseHistoryView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pHint == NULL)
	{
		return;
	}

	CNotifyServerState* pServerState = dynamic_cast<CNotifyServerState*>(pHint);
	if (pServerState != NULL)
	{
		if (pServerState->ServerOnline() == true)
		{
			OnServerOnline();
		}
		else
		{
			OnServerOffline();
		}
	}
}

void CResponseHistoryView::OnDestroy()
{
	UnregisterConnectionPoint();

	// release the notify object
	if (m_pNotify != NULL)
	{
		m_pNotify->Release();
	}

	// free any entries in the list control
	FreeEntries();

	CColouredListView::OnDestroy();
}

HRESULT CResponseHistoryView::OnResponseStart(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwFilterContext)
{
	m_csActiveResponses.Lock();
	CIISResponseData* pData = CIISResponseData::Create(pIISInfo, pRequestInfo, pResponseInfo);
	if (pData != NULL)
	{
		pData->m_IISxpressResponseInfo.m_nStartCompression = HiResTimer::GetTicks();
		m_ActiveResponses[dwFilterContext] = pData;
	}
	m_csActiveResponses.Unlock();

	return S_OK;
}

HRESULT CResponseHistoryView::OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
{
	m_csActiveResponses.Lock();

	std::map<DWORD, CIISResponseData*>::const_iterator iter = m_ActiveResponses.find(dwFilterContext);
	if (iter != m_ActiveResponses.end() && iter->second != NULL)
	{
		CIISResponseData* pData = iter->second;

		// the raw size may have changed since the original handler was invoked
		pData->m_ResponseInfo.m_dwContentLength = dwRawSize;

		pData->m_IISxpressResponseInfo.m_nEndCompression = HiResTimer::GetTicks();
		pData->m_IISxpressResponseInfo.m_bCacheHit = FALSE;
		pData->m_IISxpressResponseInfo.m_bCompressed = TRUE;		
		pData->m_IISxpressResponseInfo.m_dwCompressedSize = dwCompressedSize;

		CreateItem(pData);

		m_ActiveResponses[dwFilterContext] = NULL;
	}

	// when we have more than 100 active responses in the map try to clean them up
	if (m_ActiveResponses.size() > 100)
	{
		std::map<DWORD, CIISResponseData*> temp;

		std::map<DWORD, CIISResponseData*>::const_iterator iter = m_ActiveResponses.begin();
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

HRESULT CResponseHistoryView::OnCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize)
{
	if (pIISInfo == NULL || pRequestInfo == NULL || pResponseInfo == NULL)
		return E_POINTER;

	CIISResponseData* pData = CIISResponseData::Create(pIISInfo, pRequestInfo, pResponseInfo, TRUE, TRUE, dwCompressedSize);

	CreateItem(pData);

	ShrinkEntries();

	return S_OK;
}

HRESULT CResponseHistoryView::OnResponseEnd(DWORD dwFilterContext)
{
	return S_OK;
}

HRESULT CResponseHistoryView::OnResponseAbort(DWORD dwFilterContext)
{
	m_csActiveResponses.Lock();

	// we must free the response data from the active response map since it has aborted and
	// will never complete
	CIISResponseData* pResponseData = m_ActiveResponses[dwFilterContext];
	if (pResponseData != NULL)
	{
		delete pResponseData;

		m_ActiveResponses[dwFilterContext] = NULL;
	}	

	m_csActiveResponses.Unlock();

	return S_OK;
}

bool CResponseHistoryView::RegisterConnectionPoint(void)
{
	if (m_pNotify == NULL)
		return false;

	UnregisterConnectionPoint();
	m_pHTTPNotifyCP.Release();

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return false;
	}

	CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
	if (pDoc->GetHTTPRequest(&pHTTPRequest) == S_OK)
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

bool CResponseHistoryView::UnregisterConnectionPoint(void)
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

	return true;
}

void CResponseHistoryView::FreeEntries(void)
{
	CListCtrl& cListCtrl = GetListCtrl();

	int nItems = cListCtrl.GetItemCount();

	for (int i = 0; i < nItems; i++)
	{
		CIISResponseData* pIISResponseData = (CIISResponseData*) cListCtrl.GetItemData(i);
		if (pIISResponseData != NULL)
		{
			cListCtrl.SetItemData(i, NULL);
			delete pIISResponseData;
		}
	}
}

void CResponseHistoryView::ShrinkEntries(int nLeave, int nDeleteBlock)
{
	CListCtrl& cListCtrl = GetListCtrl();

	int nItems = cListCtrl.GetItemCount();
	if (nItems < nLeave)
		return;	

	int nRemove = nItems - nLeave;
	if (nRemove < nDeleteBlock)
		nRemove = nDeleteBlock;

	// switch off the redraw since the control will panic a bit otherwise
	cListCtrl.SetRedraw(FALSE);

	for (int i = 0; i < nRemove; i++)
	{
		CIISResponseData* pIISResponseData = (CIISResponseData*) cListCtrl.GetItemData(0);
		if (pIISResponseData != NULL)
		{			
			delete pIISResponseData;
		}

		cListCtrl.DeleteItem(0);
	}

	// since we have removed items from the list the chances are the select item (if there is one)
	// will not be visible any more, if the user has switched off auto scroll then the user will most 
	// probably want to see it so let's scroll it into view
	if (cListCtrl.GetSelectedCount() > 0 && m_bAutoScroll == false)
	{
		POSITION pos = cListCtrl.GetFirstSelectedItemPosition();
		int nSel = cListCtrl.GetNextSelectedItem(pos);

		cListCtrl.EnsureVisible(nSel, FALSE);
	}

	// switch on the redraw and invalidate and redraw the control
	cListCtrl.SetRedraw(TRUE);
	cListCtrl.Invalidate();
	cListCtrl.UpdateWindow();
}

void CResponseHistoryView::OnCustomDrawCallback(CWnd* pWnd, NMHDR* pNmHdr, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*) pNmHdr;

	DWORD dwDrawStage = pDraw->nmcd.dwDrawStage;
	if ((dwDrawStage & CDDS_ITEM) != 0)
	{
		if (pDraw->nmcd.lItemlParam != NULL && pWnd != NULL && ::IsWindowEnabled(pWnd->m_hWnd) == TRUE)
		{
			CIISResponseData* pData = (CIISResponseData*) pDraw->nmcd.lItemlParam;
			if (pData->m_IISxpressResponseInfo.m_bCacheHit == TRUE)
			{
				pDraw->clrText = ::GetSysColor(COLOR_HOTLIGHT);
				pDraw->clrTextBk = ::GetSysColor(COLOR_INFOBK);				

				if ((pDraw->nmcd.dwItemSpec & 1) != 0)
				{
					COLORREF color = pDraw->clrTextBk;

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
					pDraw->clrTextBk = color;
				}
			}
		}
	}
}

void CResponseHistoryView::OnServerOnline()
{
	RegisterConnectionPoint();
}

void CResponseHistoryView::OnServerOffline()
{
	UnregisterConnectionPoint();
}

void CResponseHistoryView::OnContextMenu(CWnd* pWnd, CPoint pos)
{	
	CListCtrl& cListCtrl = GetListCtrl();

	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader != NULL)
	{
		CRect rcHeader;
		pcHeader->GetWindowRect(rcHeader);

		if (rcHeader.PtInRect(pos) == TRUE)
		{
			OnHeaderContextMenu(pWnd, pos);
			return;
		}
	}

	OnListContextMenu(pWnd, pos);
}

void CResponseHistoryView::OnClear()
{
	FreeEntries();

	CListCtrl& cListCtrl = GetListCtrl();
	cListCtrl.DeleteAllItems();	
}

void CResponseHistoryView::OnAutoScroll()
{
	m_bAutoScroll = !m_bAutoScroll;	
}

void CResponseHistoryView::OnWriteToCSV()
{
	CListCtrl& cListCtrl = GetListCtrl();
	CDumpListCtrlToCSV::Dump(cListCtrl, GetParent());
}

void CResponseHistoryView::OnExploreContainingFolder()
{
	CListCtrl& cListCtrl = GetListCtrl();

	int nSelected = cListCtrl.GetSelectedCount();
	if (nSelected != 1)
	{
		return;
	}

	// get the selected item
	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);

	CIISResponseData* pIISResponseData = (CIISResponseData*) cListCtrl.GetItemData(nSelectedItem);
	if (pIISResponseData == NULL)
	{
		return;
	}

	CPathT<CString> PhysicalPath(CString(pIISResponseData->m_IISInfo.m_sURLMapPath));
	PhysicalPath.RemoveFileSpec();
	::ShellExecute(::GetDesktopWindow(), _T("open"), PhysicalPath, NULL, PhysicalPath, SW_SHOWNORMAL);	
}

void CResponseHistoryView::OnHistoryContextExcludeExtn()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CPathT<CStringA> path(pData->m_IISInfo.m_sURLMapPath);
	CStringA sExtension = path.GetExtension();
	if (sExtension.GetLength() <= 0)
		return;

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddExtensionRule(sExtension, "", "", 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_EXTENSIONEXCLUSIONADDED, CString(sExtension));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeCT()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
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

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddContentTypeRule(sContentType, "", "", 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_CONTENTTYPEEXCLUSIONADDED, CString(sContentType));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeMatchingCT()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
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

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddContentTypeRule(sMatchingCT, "", "", 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_CONTENTTYPEEXCLUSIONADDED, CString(sMatchingCT));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeDirectory()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CStringA sUri = pData->m_RequestInfo.m_sURI;

	int nLastSlash = sUri.ReverseFind('/');
	if (nLastSlash < 0)
		return;

	nLastSlash++;

	sUri = sUri.Left(nLastSlash);

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(pData->m_IISInfo.m_sInstanceId, sUri, RULEFLAGS_FOLDER_EXACTMATCH, 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_URIEXCLUSIONRULEADDED, CString(sUri));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeDirectoryTree()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	CStringA sUri = pData->m_RequestInfo.m_sURI;

	int nLastSlash = sUri.ReverseFind('/');
	if (nLastSlash < 0)
		return;

	nLastSlash++;

	sUri = sUri.Left(nLastSlash);

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(pData->m_IISInfo.m_sInstanceId, sUri, RULEFLAGS_FOLDER_WILDCARDMATCH, 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_URIEXCLUSIONRULEADDED, CString(sUri));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeWebSite()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;		

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddFolderRule(pData->m_IISInfo.m_sInstanceId, "/", RULEFLAGS_FOLDER_WILDCARDMATCH, 0);
	if (hr == S_OK)
	{
		pDoc->ResetResponseCache();

		CString sMsg;
		sMsg.Format(IDS_URIEXCLUSIONRULEADDED, _T("/"));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeClient()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
	if (pItemData == NULL)
		return;

	CIISResponseData* pData = (CIISResponseData*) pItemData;

	// we don't support IPv6 rules
	if (pData->m_RequestInfo.m_sRemoteAddress.Find(':') >= 0)
		return;

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddIPRule(pData->m_RequestInfo.m_sRemoteAddress, 0);	
	if (hr == S_OK)
	{
		CString sMsg;
		sMsg.Format(IDS_IPEXCLUSIONRULEADDED, CString(pData->m_RequestInfo.m_sRemoteAddress));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::OnHistoryContextExcludeSubnet()
{
	CListCtrl& cListCtrl = GetListCtrl();

	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);
	if (nSelectedItem < 0)
		return;

	DWORD_PTR pItemData = cListCtrl.GetItemData(nSelectedItem);
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

	CIISxpressCompressionStudioDoc* pDoc = dynamic_cast<CIISxpressCompressionStudioDoc*>(GetDocument());
	if (pDoc == NULL)
	{
		return;
	}		

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pDoc->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager == NULL)
		return;

	CComQIPtr<INeverCompressRules> pNeverRules = pCompressionManager;
	if (pNeverRules == NULL)
		return;

	HRESULT hr = pNeverRules->AddIPRule(sSubnet, 0);	
	if (hr == S_OK)
	{

		CString sMsg;
		sMsg.Format(IDS_IPEXCLUSIONRULEADDED, CString(sSubnet));

		::AfxMessageBox(sMsg, MB_ICONINFORMATION);
	}
	else if (hr == S_FALSE)
	{				
		::AfxMessageBox(IDS_EXCLUSIONRULEALREADYPRESENT, MB_ICONWARNING);
	}
	else 
	{		
		::AfxMessageBox(IDS_ERRORADDINGEXCLUSIONRULE, MB_ICONEXCLAMATION);
	}
}

void CResponseHistoryView::CreateColumns()
{
	CreateColumn(IDS_HISTORYVIEW_COLUMN_TIME);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_URI);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_HOSTNAME);	
	CreateColumn(IDS_HISTORYVIEW_COLUMN_CACHED);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_ORIGSIZE);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_COMPSIZE);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_RATIO);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_COMPTIME);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_CONTENTTYPE);
	CreateColumn(IDS_HISTORYVIEW_COLUMN_CLIENTIP);
}

void CResponseHistoryView::CreateColumn(int nId)
{
	int columnOrder[] = { 
		IDS_HISTORYVIEW_COLUMN_TIME, IDS_HISTORYVIEW_COLUMN_URI, IDS_HISTORYVIEW_COLUMN_HOSTNAME,
		IDS_HISTORYVIEW_COLUMN_QUERYSTRING, IDS_HISTORYVIEW_COLUMN_PORT, IDS_HISTORYVIEW_COLUMN_CACHED, 
		IDS_HISTORYVIEW_COLUMN_ORIGSIZE, IDS_HISTORYVIEW_COLUMN_COMPSIZE, IDS_HISTORYVIEW_COLUMN_RATIO,
		IDS_HISTORYVIEW_COLUMN_COMPTIME, IDS_HISTORYVIEW_COLUMN_CONTENTTYPE, IDS_HISTORYVIEW_COLUMN_CLIENTIP, 
		IDS_HISTORYVIEW_COLUMN_LOCATION, IDS_HISTORYVIEW_COLUMN_BROWSER, IDS_HISTORYVIEW_COLUMN_USERAGENT  };
	
	int nFormat = 0;
	int nWidth = 0;

	switch (nId)
	{
	case IDS_HISTORYVIEW_COLUMN_TIME: nFormat = LVCFMT_LEFT; nWidth = 70; break;
	case IDS_HISTORYVIEW_COLUMN_URI: nFormat = LVCFMT_LEFT; nWidth = 210; break;	
	case IDS_HISTORYVIEW_COLUMN_HOSTNAME: nFormat = LVCFMT_LEFT; nWidth = 120; break;
	case IDS_HISTORYVIEW_COLUMN_QUERYSTRING: nFormat = LVCFMT_LEFT; nWidth = 140; break;
	case IDS_HISTORYVIEW_COLUMN_PORT: nFormat = LVCFMT_RIGHT; nWidth = 50; break;
	case IDS_HISTORYVIEW_COLUMN_CACHED: nFormat = LVCFMT_LEFT; nWidth = 65; break;
	case IDS_HISTORYVIEW_COLUMN_ORIGSIZE: nFormat = LVCFMT_RIGHT; nWidth = 90; break;
	case IDS_HISTORYVIEW_COLUMN_COMPSIZE: nFormat = LVCFMT_RIGHT; nWidth = 90; break;
	case IDS_HISTORYVIEW_COLUMN_RATIO: nFormat = LVCFMT_RIGHT; nWidth = 50; break;
	case IDS_HISTORYVIEW_COLUMN_CONTENTTYPE: nFormat = LVCFMT_LEFT; nWidth = 120; break;
	case IDS_HISTORYVIEW_COLUMN_CLIENTIP: nFormat = LVCFMT_RIGHT; nWidth = 90; break;
	case IDS_HISTORYVIEW_COLUMN_LOCATION: nFormat = LVCFMT_LEFT; nWidth = 110; break;
	case IDS_HISTORYVIEW_COLUMN_BROWSER: nFormat = LVCFMT_LEFT; nWidth = 90; break;
	case IDS_HISTORYVIEW_COLUMN_USERAGENT: nFormat = LVCFMT_LEFT; nWidth = 110; break;	
	case IDS_HISTORYVIEW_COLUMN_COMPTIME: nFormat = LVCFMT_RIGHT; nWidth = 90; break;	
	}

	int nColumn = 0;
	for (int i = 0; i < _countof(columnOrder); i++)
	{
		if (nId == columnOrder[i])
		{
			break;
		}

		if (IsColumnVisible(columnOrder[i]) == TRUE)
		{
			++nColumn;
		}
	}

	if (nColumn >= 0)
	{
		InsertColumn(nColumn, nId, nFormat, nWidth);
	}
}

void CResponseHistoryView::InsertColumn(int nColumn, int nId, int nFormat, int nWidth)
{
	CListCtrl& cListCtrl = GetListCtrl();	

	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();	

	CString sName;
	sName.LoadString(nId);					

	nColumn = cListCtrl.InsertColumn(nColumn, sName, nFormat, nWidth);	
	if (pcHeader != NULL && nColumn >= 0)
	{
		HDITEM item;
		item.mask = HDI_LPARAM;
		item.lParam = nId;
		pcHeader->SetItem(nColumn, &item);
	}
}

int CResponseHistoryView::GetColumnId(int nColumn)
{
	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader == NULL)
	{
		return -1;
	}

	HDITEM item;
	item.mask = HDI_LPARAM;
	if (pcHeader->GetItem(nColumn, &item) == FALSE)
	{
		return -1;
	}

	return (int) item.lParam;
}

void CResponseHistoryView::GetColumnIndexMap(std::map<int, int>& indexes)
{
	indexes.clear();

	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();	
	if (pcHeader == NULL)
	{
		return;
	}

	int nColumns = pcHeader->GetItemCount();
	for (int i = 0; i < nColumns; i++)
	{
		HDITEM item;
		item.mask = HDI_LPARAM;
		pcHeader->GetItem(i, &item);

		indexes[(int)item.lParam] = i;
	}
}

BOOL CResponseHistoryView::IsColumnVisible(int nId)
{
	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader == NULL)
	{
		return FALSE;
	}

	int nColumns = pcHeader->GetItemCount();
	for (int i = 0; i < nColumns; i++)
	{
		HDITEM item;
		item.mask = HDI_LPARAM;
		pcHeader->GetItem(i, &item);

		if (nId == item.lParam)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CResponseHistoryView::ToggleColumn(int nId)
{
	CListCtrl& cListCtrl = GetListCtrl();	
	CHeaderCtrl* pcHeader = cListCtrl.GetHeaderCtrl();
	if (pcHeader == NULL)
	{
		return;
	}

	std::map<int, int> ColumnMap;
	GetColumnIndexMap(ColumnMap);

	std::map<int, int>::const_iterator iter = ColumnMap.find(nId);
	if (iter == ColumnMap.end())
	{				
		CreateColumn(nId);
		
		int nItems = cListCtrl.GetItemCount();
		for (int i = 0; i < nItems; i++)
		{
			CIISResponseData* pData = CListCtrlHelper::GetItemData<CIISResponseData>(cListCtrl, i);
			if (pData != NULL)
			{
				UpdateItem(i, *pData);
			}
		}
	} 
	else	
	{
		cListCtrl.DeleteColumn(iter->second);
	}	
}

void CResponseHistoryView::CreateItem(const CIISResponseData* pData)
{
	if (pData == NULL)
	{
		return;
	}

	SYSTEMTIME stTime;
	::GetLocalTime(&stTime);

	CListCtrl& cListCtrl = GetListCtrl();

	CString sTime;		
	CInternationalHelper::Format24hTime(stTime, sTime);
	int nItem = cListCtrl.InsertItem(cListCtrl.GetItemCount(), sTime);

	cListCtrl.SetItemData(nItem, (DWORD_PTR) pData);

	UpdateItem(nItem, *pData);

	if (m_bAutoScroll == true)
	{
		cListCtrl.EnsureVisible(nItem, FALSE);
	}
}

void CResponseHistoryView::UpdateItem(int nItem, const CIISResponseData& data)
{
	CListCtrl& cListCtrl = GetListCtrl();

	std::map<int, int> ColumnMap;
	GetColumnIndexMap(ColumnMap);

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_URI) != ColumnMap.end())
	{
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_URI], CString(data.m_RequestInfo.m_sURI));
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_HOSTNAME) != ColumnMap.end())
	{
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_HOSTNAME], CString(data.m_RequestInfo.m_sHostname));
	}
	
	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_CACHED) != ColumnMap.end())
	{
		if (data.m_IISxpressResponseInfo.m_bCacheHit == TRUE)
		{
			cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_CACHED], m_sCacheStatusYes);
		}
		else
		{
			cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_CACHED], m_sCacheStatusNo);
		}
	}
	
	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_ORIGSIZE) != ColumnMap.end())
	{
		CString sOriginalSize;
		sOriginalSize.Format(_T("%u"), data.m_ResponseInfo.m_dwContentLength);
		CInternationalHelper::FormatIntegerNumber(sOriginalSize);
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_ORIGSIZE], sOriginalSize);
	}
	
	DWORD dwCompressedSize = data.m_IISxpressResponseInfo.m_dwCompressedSize;

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_COMPSIZE) != ColumnMap.end())
	{		
		CString sCompressedSize;	
		sCompressedSize.Format(_T("%u"), dwCompressedSize);
		CInternationalHelper::FormatIntegerNumber(sCompressedSize);
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_COMPSIZE], sCompressedSize);
	}
	
	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_RATIO) != ColumnMap.end())
	{
		float fRatio = (float) dwCompressedSize;
		fRatio /= data.m_ResponseInfo.m_dwContentLength;
		fRatio = 1.0f - fRatio;
		fRatio *= 100.0f;

		// if the ratio is negative then it means that the server hasn't been able to make the data
		// smaller and has returned the original response, so rather than show negative values
		// set the ratio to zero to denote no success
		if (fRatio < 0.0f)
			fRatio = 0.0f;

		CString sRatio;
		sRatio.Format(_T("%.0f%%"), fRatio);
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_RATIO], sRatio);
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_CONTENTTYPE) != ColumnMap.end())
	{
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_CONTENTTYPE], CString(data.m_ResponseInfo.m_sContentType));
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_CLIENTIP) != ColumnMap.end())
	{
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_CLIENTIP], CString(data.m_RequestInfo.m_sRemoteAddress));	
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_LOCATION) != ColumnMap.end())
	{
		LPCTSTR pszCountry = CGeoIPHelper::LookupCountry(data.m_RequestInfo.m_sRemoteAddress);
		pszCountry = pszCountry == NULL ? _T("") : pszCountry;
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_LOCATION], pszCountry);
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_BROWSER) != ColumnMap.end())
	{
		CString sUserAgentName;
		if (g_UserAgentLookup.GetUserAgentName(data.m_RequestInfo.m_sUserAgent, sUserAgentName) == false)
		{
			sUserAgentName = data.m_RequestInfo.m_sUserAgent;
		}
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_BROWSER], sUserAgentName);
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_USERAGENT) != ColumnMap.end())
	{		
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_USERAGENT], CString(data.m_RequestInfo.m_sUserAgent));
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_QUERYSTRING) != ColumnMap.end())
	{		
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_QUERYSTRING], CString(data.m_RequestInfo.m_sQueryString));
	}
	
	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_PORT) != ColumnMap.end())
	{		
		cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_PORT], CString(data.m_IISInfo.m_sServerPort));
	}

	if (ColumnMap.find(IDS_HISTORYVIEW_COLUMN_COMPTIME) != ColumnMap.end())
	{		
		if (data.m_IISxpressResponseInfo.m_nStartCompression > 0 && data.m_IISxpressResponseInfo.m_nEndCompression > 0)
		{
			__int64 nCompressionTime = data.m_IISxpressResponseInfo.m_nEndCompression - data.m_IISxpressResponseInfo.m_nStartCompression;
			nCompressionTime *= 1000;
			nCompressionTime /= HiResTimer::GetFrequency();

			CString sCompressionTime;
			sCompressionTime.Format(_T("%ld ms"), nCompressionTime);

			cListCtrl.SetItemText(nItem, ColumnMap[IDS_HISTORYVIEW_COLUMN_COMPTIME], sCompressionTime);
		}
	}
}

void CResponseHistoryView::OnHeaderContextMenu(CWnd* pWnd, CPoint pos)
{
	std::map<int, int> ColumnMap;
	GetColumnIndexMap(ColumnMap);

	CMenu* pSubMenu = m_HeaderPopupMenu.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;	

	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_URI, ID_HISTORYHEADERCONTEXT_URI);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_HOSTNAME, ID_HISTORYHEADERCONTEXT_HOSTNAME);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_PORT, ID_HISTORYHEADERCONTEXT_PORT);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_QUERYSTRING, ID_HISTORYHEADERCONTEXT_QUERYSTRING);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_CACHED, ID_HISTORYHEADERCONTEXT_CACHED);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_ORIGSIZE, ID_HISTORYHEADERCONTEXT_ORIGINALSIZE);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_COMPSIZE, ID_HISTORYHEADERCONTEXT_COMPRESSEDSIZE);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_RATIO, ID_HISTORYHEADERCONTEXT_RATIO);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_CONTENTTYPE, ID_HISTORYHEADERCONTEXT_CONTENTTYPE);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_CLIENTIP, ID_HISTORYHEADERCONTEXT_CLIENTIP);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_LOCATION, ID_HISTORYHEADERCONTEXT_LOCATION);	
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_BROWSER, ID_HISTORYHEADERCONTEXT_BROWSER);	
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_USERAGENT, ID_HISTORYHEADERCONTEXT_USERAGENT);
	SetHeaderContextMenuItemState(*pSubMenu, ColumnMap, IDS_HISTORYVIEW_COLUMN_COMPTIME, ID_HISTORYHEADERCONTEXT_COMPRESSIONTIME);
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 		
			pos.x, pos.y, this);
}

void CResponseHistoryView::SetHeaderContextMenuItemState(CMenu& SubMenu, const std::map<int, int>& ColumnMap, int nColumnId, int nCmd)
{
	int nCheck = MF_CHECKED;
	if (ColumnMap.find(nColumnId) == ColumnMap.end())
	{
		nCheck = 0;				
	}
	CPopupMenuHelper::CheckMenuItem(&SubMenu, nCmd, nCheck);
}

void CResponseHistoryView::OnListContextMenu(CWnd* pWnd, CPoint pos)
{
	CListCtrl& cListCtrl = GetListCtrl();

	int nSelected = cListCtrl.GetSelectedCount();

	// store the original position
	CPoint menuPos = pos;

	// get the list control window rect
	CRect rcHistory;
	cListCtrl.GetWindowRect(rcHistory);	

	// get the mouse pos
	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	// get the selected item
	POSITION selectedPos = cListCtrl.GetFirstSelectedItemPosition();
	int nSelectedItem = cListCtrl.GetNextSelectedItem(selectedPos);

	// convert the incoming pos to something useful if it came from the keyboard
	if (pos.x < 0 && pos.y < 0)
	{						
		if (nSelectedItem >= 0)
		{
			// get the selected item's rect
			CRect rcItem;
			cListCtrl.GetItemRect(nSelectedItem, &rcItem, LVIR_LABEL);

			// map the selected item's rect to screen coords
			CPoint ptItem(rcItem.CenterPoint());
			cListCtrl.ClientToScreen(&ptItem);
			pos = ptItem;
		}		
	}	

	CMenu* pSubMenu = m_ListPopupMenu.GetSubMenu(0);
	ASSERT(pSubMenu != NULL);
	if (pSubMenu == NULL)
		return;

	CPoint ptClient(pos);
	cListCtrl.ScreenToClient(&ptClient);

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
		pos.x, pos.y, ::AfxGetMainWnd());
}

void CResponseHistoryView::OnHeaderContextURI()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_URI);
}

void CResponseHistoryView::OnHeaderContextHostname()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_HOSTNAME);
}

void CResponseHistoryView::OnHeaderContextPort()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_PORT);
}

void CResponseHistoryView::OnHeaderContextQueryString()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_QUERYSTRING);
}

void CResponseHistoryView::OnHeaderContextCached()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_CACHED);
}

void CResponseHistoryView::OnHeaderContextOriginalSize()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_ORIGSIZE);
}

void CResponseHistoryView::OnHeaderContextCompressedSize()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_COMPSIZE);
}

void CResponseHistoryView::OnHeaderContextRatio()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_RATIO);
}

void CResponseHistoryView::OnHeaderContextContentType()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_CONTENTTYPE);
}

void CResponseHistoryView::OnHeaderContextClientIP()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_CLIENTIP);
}

void CResponseHistoryView::OnHeaderContextLocation()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_LOCATION);
}

void CResponseHistoryView::OnHeaderContextBrowser()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_BROWSER);
}

void CResponseHistoryView::OnHeaderContextUserAgent()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_USERAGENT);
}

void CResponseHistoryView::OnHeaderContextCompressionTime()
{
	ToggleColumn(IDS_HISTORYVIEW_COLUMN_COMPTIME);
}

void CResponseHistoryView::OnCmdUIListViewHasSelectedItem(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	UINT nSelected = cListCtrl.GetSelectedCount();
	if (nSelected == 1)
	{
		pCmdUI->Enable(TRUE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);	
	}
}

void CResponseHistoryView::OnCmdUIAutoScroll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);

	if (m_bAutoScroll == TRUE)
	{		
		pCmdUI->SetCheck(1);
	}	
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CResponseHistoryView::OnCmdUIWriteToCSV(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nItems = (int) cListCtrl.GetItemCount();
	if (nItems > 0)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CResponseHistoryView::OnCmdUIClear(CCmdUI* pCmdUI)
{
	CListCtrl& cListCtrl = GetListCtrl();
	int nItems = (int) cListCtrl.GetItemCount();
	if (nItems > 0)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}