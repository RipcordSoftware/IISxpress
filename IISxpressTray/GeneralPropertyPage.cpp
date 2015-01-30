// GeneralPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "IISxpressTray.h"
#include "GeneralPropertyPage.h"

#include "HTTPNotify.h"

#include "InternationalHelper.h"

#include "resource.h"

#define KB_MB_THRESHOLD		(50 * 1024)

// the time between activity area updates
#define ACTIVITY_UPDATE_TIME	1000

// CGeneralPropertyPage dialog

IMPLEMENT_DYNAMIC(CGeneralPropertyPage, CIISxpressPageBase)

CGeneralPropertyPage::CGeneralPropertyPage()
	: CIISxpressPageBase(CGeneralPropertyPage::IDD)
{
	//m_pNotify = NULL;
	//m_dwCookie = -1;
}

CGeneralPropertyPage::~CGeneralPropertyPage()
{
}

void CGeneralPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IISSTATUS, m_cIISStatus);
	DDX_Control(pDX, IDC_EDIT_FILTER_STATUS, m_cFilterStatus);
	DDX_Control(pDX, IDC_EDIT_SERVER_STATUS, m_cServerStatus);
	DDX_Control(pDX, IDC_EDIT_NUM_COMPRESSED, m_cNumCompressedResponses);
	DDX_Control(pDX, IDC_EDIT_DATAIN_VALUE, m_cDataIn);
	DDX_Control(pDX, IDC_EDIT_DATAOUT_VALUE, m_cDataOut);
	DDX_Control(pDX, IDC_EDIT_COMPRESSION_RATIO, m_cCompressionRatio);
	DDX_Control(pDX, IDC_STATIC_DATAIN_UNITS, m_cDataInUnits);
	DDX_Control(pDX, IDC_STATIC_DATAOUT_UNITS, m_cDataOutUnits);
	DDX_Control(pDX, IDC_EDIT_NUM_CACHEHITS, m_cCacheHits);
}


BEGIN_MESSAGE_MAP(CGeneralPropertyPage, CIISxpressPageBase)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UPDATEACTIVITYCONTROLS, UpdateActivityControls)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CGeneralPropertyPage message handlers

BOOL CGeneralPropertyPage::OnInitDialog(void)
{
	BOOL bStatus = CIISxpressPageBase::OnInitDialog();
	
	m_sKBUnits.LoadString(IDS_KB_UNITS);		
	m_sMBUnits.LoadString(IDS_MB_UNITS);

	//if (m_pNotify == NULL)
	//{
	//	CComObject<CHTTPNotify>::CreateInstance(&m_pNotify);
	//	m_pNotify->AddRef();
	//	m_pNotify->Init(this);
	//}

	UpdateIISState();
	UpdateFilterState();
	UpdateServerState();
	UpdateActivityControls();

	// setup the timer for the activity area
	SetTimer(::GetTickCount(), ACTIVITY_UPDATE_TIME, NULL);

	return bStatus;
}

HRESULT CGeneralPropertyPage::OnResponseStart(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwFilterContext)
{
	return S_OK;
}

HRESULT CGeneralPropertyPage::OnResponseComplete(DWORD dwFilterContext, BOOL bCompressed, DWORD dwRawSize, DWORD dwCompressedSize)
{
	if (bCompressed == TRUE)
	{
		// don't directly call anymore, post a message instead
		//UpdateActivityControls();
		PostMessage(WM_UPDATEACTIVITYCONTROLS);
	}

	return S_OK;
}

HRESULT CGeneralPropertyPage::OnResponseEnd(DWORD dwFilterContext)
{
	return S_OK;
}

HRESULT CGeneralPropertyPage::OnResponseAbort(DWORD dwFilterContext)
{
	return S_OK;
}

HRESULT CGeneralPropertyPage::OnCacheHit(
		IISInfo* pIISInfo,
		RequestInfo* pRequestInfo, 
		ResponseInfo* pResponseInfo,
		DWORD dwCompressedSize)
{
	PostMessage(WM_UPDATEACTIVITYCONTROLS);

	return S_OK;
}

bool CGeneralPropertyPage::RegisterConnectionPoint(void)
{
	//if (m_pNotify == NULL)
	//	return false;

	//m_pHTTPNotifyCP.Release();

	//CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	//CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
	//if (pSheet->GetHTTPRequest(&pHTTPRequest) == S_OK)
	//{
	//	CComQIPtr<IConnectionPointContainer> pConnPointCont = pHTTPRequest;
	//	if (pConnPointCont != NULL)
	//	{				
	//		if (pConnPointCont != NULL)
	//		{					
	//			HRESULT hr = pConnPointCont->FindConnectionPoint(IID_IIISxpressHTTPNotify, &m_pHTTPNotifyCP);
	//			if (m_pHTTPNotifyCP != NULL)
	//			{
	//				CComPtr<IUnknown> pUnk;
	//				m_pNotify->QueryInterface(IID_IUnknown, (void**) &pUnk);

	//				m_dwCookie = 0;
	//				hr = m_pHTTPNotifyCP->Advise(pUnk, &m_dwCookie);
	//			}
	//		}
	//	}
	//}

	return true;
}

bool CGeneralPropertyPage::UnregisterConnectionPoint(void)
{
	//if (m_pHTTPNotifyCP == NULL || m_dwCookie == -1)
	//	return false;

	//if (m_pHTTPNotifyCP->Unadvise(m_dwCookie) != S_OK)
	//{
	//	// now panic since the remote will not be releasing the ref count it has been holding,
	//	// so get rid of all ref counts except 1
	//	if (m_pNotify != NULL)
	//	{
	//		m_pNotify->Lock();

	//		while (m_pNotify->m_dwRef > 1)
	//		{
	//			m_pNotify->Release();
	//		}

	//		m_pNotify->Unlock();
	//	}
	//}

	//m_dwCookie = -1;

	return true;
}

void CGeneralPropertyPage::OnDestroy(void)
{
	UnregisterConnectionPoint();

	// release the notify object
	//if (m_pNotify != NULL)
	//{
	//	m_pNotify->Release();
	//}
}

void CGeneralPropertyPage::ServerOnline(void)
{
	UpdateActivityControls();
	//RegisterConnectionPoint();
}

void CGeneralPropertyPage::ServerOffline(void)
{
	UpdateActivityControls();

	UnregisterConnectionPoint();
}

void CGeneralPropertyPage::Heartbeat(void)
{
	UpdateIISState();
	UpdateFilterState();
	UpdateServerState();
}

LRESULT CGeneralPropertyPage::UpdateActivityControls(WPARAM, LPARAM)
{
	// call the standard update function
	UpdateActivityControls();
	return 0;
}

void CGeneralPropertyPage::UpdateActivityControls(void)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CString sCompressedResponses;
	CString sCacheHits;
	CString sDataIn;
	CString sDataOut;
	CString sCompressionRatio;

	CString sDataInUnits = m_sKBUnits;	
	CString sDataOutUnits = m_sKBUnits;	

	CComPtr<ICompressionRuleManager> pCompressionManager;
	pSheet->GetCompressionManager(&pCompressionManager);
	if (pCompressionManager != NULL)
	{
		CComQIPtr<ICompressionStats> pStats = pCompressionManager;
		if (pStats != NULL)
		{
			DWORD dwCompressedResponses = 0;
			DWORD dwRawSize = 0;
			DWORD dwCompressedSize = 0;
			if (pStats->GetStats(&dwCompressedResponses, &dwRawSize, &dwCompressedSize) == S_OK)
			{
				sCompressedResponses.Format(_T("%u"), dwCompressedResponses);
				CInternationalHelper::FormatIntegerNumber(sCompressedResponses);

				// only do ratio calculation if dwRawSize > 0 to get around divide by zero problem
				if (dwRawSize > 0)
				{
					float fCompressionRatio = (float) dwCompressedSize;
					fCompressionRatio /= (float) dwRawSize;
					fCompressionRatio = 1.0f - fCompressionRatio;
					fCompressionRatio *= 100.0f;

					// if the compression ratio is somehow less than zero set it to zero!
					if (fCompressionRatio < 0.0f)
						fCompressionRatio = 0.0f;

					sCompressionRatio.Format(_T("%.1f"), fCompressionRatio);	
					CInternationalHelper::FormatFloatNumber(sCompressionRatio, 1);
				}

				if (dwRawSize >= KB_MB_THRESHOLD)
				{
					// turn into Mb
					dwRawSize >>= 10;
					sDataInUnits = m_sMBUnits;
				}

				sDataIn.Format(_T("%u"), dwRawSize);
				CInternationalHelper::FormatIntegerNumber(sDataIn);

				if (dwCompressedSize >= KB_MB_THRESHOLD)
				{
					// turn into Mb
					dwCompressedSize >>= 10;
					sDataOutUnits = m_sMBUnits;
				}

				sDataOut.Format(_T("%u"), dwCompressedSize);
				CInternationalHelper::FormatIntegerNumber(sDataOut);
			}

			CComQIPtr<ICompressionStats2> pStats2 = pCompressionManager;
			if (pStats2 != NULL)
			{
				DWORD dwCacheHits = 0;
				DWORD dwCacheRawSize = 0;
				DWORD dwCacheCompressedSize = 0;
				if (pStats2->GetCacheStats(&dwCacheHits, &dwCacheRawSize, &dwCacheCompressedSize) == S_OK)
				{
					sCacheHits.Format(_T("%u"), dwCacheHits);
					CInternationalHelper::FormatIntegerNumber(sCacheHits);
				}				
			}
		}
	}

	m_cNumCompressedResponses.SetWindowText(sCompressedResponses);
	m_cNumCompressedResponses.SetSel(-1, 0);

	m_cCacheHits.SetWindowText(sCacheHits);
	m_cCacheHits.SetSel(-1, 0);

	m_cDataIn.SetWindowText(sDataIn);
	m_cDataIn.SetSel(-1, 0);

	m_cDataOut.SetWindowText(sDataOut);
	m_cDataOut.SetSel(-1, 0);

	m_cCompressionRatio.SetWindowText(sCompressionRatio);
	m_cCompressionRatio.SetSel(-1, 0);

	m_cDataInUnits.SetWindowText(sDataInUnits);

	m_cDataOutUnits.SetWindowText(sDataOutUnits);
}

void CGeneralPropertyPage::UpdateIISState(void)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CString sIISState;
	sIISState.LoadString(IDS_IIS_SERVERSTATE_UNKNOWN);

	// if an NT based platform we deal with the service manager
	if (g_VerInfo.dwPlatformId >= VER_PLATFORM_WIN32_NT)
	{
		SC_HANDLE hSMStatus = pSheet->GetServiceManager();

		if (hSMStatus != NULL)
		{
			SC_HANDLE hW3SVC = ::OpenService(hSMStatus, IIS_SERVICENAME, SERVICE_QUERY_STATUS);
			if (hW3SVC != NULL)
			{
				SERVICE_STATUS Status;
				if (::QueryServiceStatus(hW3SVC, &Status) == TRUE)
				{
					if (Status.dwCurrentState == SERVICE_RUNNING)
					{						
						sIISState.LoadString(IDS_IIS_SERVERSTATE_RUNNING);						
					}
					else if (Status.dwCurrentState == SERVICE_STOPPED)
					{
						sIISState.LoadString(IDS_IIS_SERVERSTATE_STOPPED);						
					}
					else if (Status.dwCurrentState == SERVICE_START_PENDING)
					{
						sIISState.LoadString(IDS_IIS_SERVERSTATE_STARTING);						
					}
					else if (Status.dwCurrentState == SERVICE_STOP_PENDING)
					{
						sIISState.LoadString(IDS_IIS_SERVERSTATE_STOPPING);						
					}
					else if (Status.dwCurrentState == SERVICE_PAUSED || Status.dwCurrentState == SERVICE_PAUSE_PENDING)
					{
						sIISState.LoadString(IDS_IIS_SERVERSTATE_PAUSED);						
					}
					else if (Status.dwCurrentState == SERVICE_CONTINUE_PENDING)
					{
						sIISState.LoadString(IDS_IIS_SERVERSTATE_CONTINUING);						
					}
				}

				::CloseServiceHandle(hW3SVC);
			}
		}
	}
	else
	{
		// it's an old style Windows 9x platform, use the metabase
		int nWebServers = 0;
		bool bActiveServer = false;

		CIISMetaBase MetaBase;
		CComPtr<IMSAdminBase> pAdminBase;
		if (MetaBase.Init() == true &&
			MetaBase.GetAdminBase(&pAdminBase) == true)
		{
			// enum the keys under W3SVC
			std::vector<std::wstring> Keys;
			MetaBase.EnumKeys(_T("/LM/W3SVC"), Keys);

			// loop round the keys looking for web servers
			int nKeys = (int) Keys.size();
			for (int i = 0; i < nKeys; i++)
			{
				CComBSTR bsPath = L"/LM/W3SVC/";
				bsPath += Keys[i].c_str();

				CIISMetaBaseData KeyType;
				std::wstring sKeyType;
				if (KeyType.ReadData(pAdminBase, bsPath, MD_KEY_TYPE) == true &&
					KeyType.GetAsString(sKeyType) == true && 
					sKeyType == L"IIsWebServer")
				{				
					CIISMetaBaseData ServerState;
					if (ServerState.ReadData(pAdminBase, bsPath, MD_SERVER_STATE) == true)
					{
						// we have found a web server
						nWebServers++;

						DWORD dwServerState = 0;
						ServerState.GetAsDWORD(&dwServerState);

						if (dwServerState == MD_SERVER_STATE_STARTED)
						{
							sIISState.LoadString(IDS_IIS_SERVERSTATE_RUNNING);
							bActiveServer = true;
							break;
						}						
					}
				}
			}
		}

		if (nWebServers > 0 && bActiveServer == false)
		{
			sIISState.LoadString(IDS_IIS_SERVERSTATE_STOPPED);
		}
	}

	m_cIISStatus.SetWindowText(sIISState);
	m_cIISStatus.SetSel(-1, 0);
}

void CGeneralPropertyPage::UpdateFilterState(void)
{
	CString sFilterState;
	sFilterState.LoadString(IDS_FILTERSTATE_UNKNOWN);

	// Windows 2003 and earlier
	if (g_VerInfo.dwMajorVersion < 6)
	{
		CIISMetaBase MetaBase;	
		CComPtr<IMSAdminBase> pAdminBase;

		CIISMetaBaseData FilterLoadOrder;	
		if (MetaBase.Init() == true &&
			MetaBase.GetAdminBase(&pAdminBase) == true &&
			FilterLoadOrder.ReadData(pAdminBase, L"/LM/W3SVC/Filters", MD_FILTER_LOAD_ORDER) == true)
		{
			std::wstring sLoadOrder;
			if (FilterLoadOrder.GetAsString(sLoadOrder) == true &&
				sLoadOrder.find(L"IISxpress") != std::wstring::npos)
			{
				CIISMetaBaseData IISxpressFilterState;
				if (IISxpressFilterState.ReadData(pAdminBase, L"/LM/W3SVC/Filters/IISxpress", MD_FILTER_STATE) == true)
				{
					DWORD dwFilterState = 0;
					IISxpressFilterState.GetAsDWORD(&dwFilterState);

					if (dwFilterState == MD_FILTER_STATE_LOADED)
					{
						sFilterState.LoadString(IDS_FILTERSTATE_RUNNING);
					}
					else
					{
						sFilterState.LoadString(IDS_FILTERSTATE_INSTALLED);
					}
				}
				else if (IISxpressFilterState.ReadData(pAdminBase, L"/LM/W3SVC/Filters/IISxpress", MD_FILTER_IMAGE_PATH) == true)
				{
					sFilterState.LoadString(IDS_FILTERSTATE_INSTALLED);
				}
				else
				{
					sFilterState.LoadString(IDS_FILTERSTATE_NOTINSTALLED);
				}
			}
			else
			{
				sFilterState.LoadString(IDS_FILTERSTATE_NOTINSTALLED);
			}
		}
	}
	else
	{
		// detect the state of the primary IIS 7 module
		int bitness = sizeof(PVOID) * 8;
		if (GetIIS7ModuleInstalledState(bitness))
		{
			sFilterState.LoadString(IDS_FILTERSTATE_RUNNING);
		}		
	}

	m_cFilterStatus.SetWindowText(sFilterState);
	m_cFilterStatus.SetSel(-1, 0);
}

bool CGeneralPropertyPage::GetIIS7ModuleInstalledState(int bitness)
{
	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<IXMLDOMDocument> pDoc;
	HRESULT hr = pSheet->GetIIS7ConfigDocument(&pDoc);	
	if (hr == S_OK && pDoc != NULL)
	{
		CAtlStringW sGlobalModuleQuery;
		sGlobalModuleQuery.Format(L"/configuration/system.webServer/globalModules/add[@name='IISxpressNativeModule%d']", bitness);

		CComPtr<IXMLDOMNode> pIISxpressModuleNode;
		hr = pDoc->selectSingleNode(
				CComBSTR(sGlobalModuleQuery),
				&pIISxpressModuleNode);

		if (hr == S_OK && pIISxpressModuleNode != NULL)
		{
			CAtlStringW sLocationModuleQuery;
			sLocationModuleQuery.Format(L"/configuration/location[@path='']/system.webServer/modules/add[@name='IISxpressNativeModule%d']", bitness);

			CComPtr<IXMLDOMNode> pIISxpressModuleLoadedNode;
			hr = pDoc->selectSingleNode(
				CComBSTR(sLocationModuleQuery),
				&pIISxpressModuleLoadedNode);

			if (hr == S_OK && pIISxpressModuleLoadedNode != NULL)
			{
				return true;
			}
		}		
	}

	return false;
}

void CGeneralPropertyPage::UpdateServerState(void)
{
	CString sServerState;
	sServerState.LoadString(IDS_SERVERSTATE_STOPPED);

	CIISxpressTrayPropertySheet* pSheet = (CIISxpressTrayPropertySheet*) GetParent();

	CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
	if (pSheet->GetHTTPRequest(&pHTTPRequest) == S_OK)
	{
		sServerState.LoadString(IDS_SERVERSTATE_RUNNING);
	}

	m_cServerStatus.SetWindowText(sServerState);
	m_cServerStatus.SetSel(-1, 0);
}

void CGeneralPropertyPage::OnTimer(UINT_PTR nIDEvent)
{
	UpdateActivityControls();	
}