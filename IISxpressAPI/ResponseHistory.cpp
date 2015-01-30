#include "stdafx.h"

#include "ResponseHistory.h"

IISxpressAPI::ResponseHistory::ResponseHistory() : 
	m_pNotifyObj(nullptr), m_dwCookie(0), m_pHTTPNotifyCP(nullptr)
{
	CComObject<CHTTPNotify>* pNotifyObj;
	HRESULT hr = CComObject<CHTTPNotify>::CreateInstance(&pNotifyObj);
	if (hr != S_OK || pNotifyObj == NULL)
	{
		throw gcnew IISxpressAPI::IISxpressAPIException(hr);
	}	

	m_pNotifyObj = pNotifyObj;
	m_pNotifyObj->AddRef();

	AttachConnectionPoint();
}

void IISxpressAPI::ResponseHistory::AttachConnectionPoint()
{
	m_dwCookie = 0;
	m_pHTTPNotifyCP = nullptr;

	if (m_pNotifyObj == nullptr)
	{
		// TODO: a better error message here
		throw gcnew System::Exception();
	}

	CComPtr<IIISxpressHTTPRequest> pHTTPRequest;
	HRESULT hr = GetHTTPRequest(&pHTTPRequest);
	if (hr != S_OK)
	{
		throw gcnew IISxpressAPI::IISxpressAPIException(hr);
	}

	CComQIPtr<IConnectionPointContainer> pConnPointCont = pHTTPRequest;
	if (pConnPointCont != NULL)
	{				
		if (pConnPointCont != NULL)
		{					
			CComPtr<IConnectionPoint> pHTTPNotifyCP;
			HRESULT hr = pConnPointCont->FindConnectionPoint(IID_IIISxpressHTTPNotify, &pHTTPNotifyCP);
			if (pHTTPNotifyCP != NULL)
			{
				CComPtr<IUnknown> pUnk;
				m_pNotifyObj->QueryInterface(IID_IUnknown, (void**) &pUnk);
		
				DWORD dwCookie = 0;
				hr = pHTTPNotifyCP->Advise(pUnk, &dwCookie);

				m_pHTTPNotifyCP = pHTTPNotifyCP.Detach();
				m_dwCookie = dwCookie;
			}
		}
	}
}

void IISxpressAPI::ResponseHistory::DetachConnectionPoint()
{
	if (m_pHTTPNotifyCP != nullptr)
	{			
		m_pHTTPNotifyCP->Unadvise(m_dwCookie);
		m_pHTTPNotifyCP->Release();
		m_pHTTPNotifyCP = nullptr;
	}

	m_dwCookie = -1;	
}

IISxpressAPI::ResponseHistory::!ResponseHistory()
{
	DetachConnectionPoint();

	if (m_pNotifyObj != nullptr)
	{
		m_pNotifyObj->Release();		
		m_pNotifyObj = nullptr;
	}
}

IISxpressAPI::ResponseHistory::~ResponseHistory()
{
	DetachConnectionPoint();

	if (m_pNotifyObj != nullptr)
	{
		m_pNotifyObj->Release();		
		m_pNotifyObj = nullptr;
	}
}

HRESULT IISxpressAPI::ResponseHistory::GetHTTPRequest(IIISxpressHTTPRequest** ppHTTPRequest)
{
	if (ppHTTPRequest == NULL)
		return E_POINTER;

	// get the active HTTPRequestUnk interface
	CComPtr<IUnknown> pHTTPRequestUnk;
	HRESULT hr = ::GetActiveObject(CLSID_IISxpressHTTPRequest, NULL, (IUnknown**) &pHTTPRequestUnk);
	if (hr != S_OK)
	{
		return hr;	
	}

	// get the active HTTPRequest interface
	CComQIPtr<IIISxpressHTTPRequest> pHTTPRequest = pHTTPRequestUnk;
	if (pHTTPRequest == NULL)
	{
		return E_FAIL;
	}

	*ppHTTPRequest = pHTTPRequest.Detach();

	return hr;
}

array<IISxpressAPI::ResponseDetails^>^ IISxpressAPI::ResponseHistory::GetResponses()
{
	return m_pNotifyObj->GetResponses();
}

void IISxpressAPI::ResponseHistory::ClearResponses()
{
	m_pNotifyObj->ClearResponses();
}