#include "stdafx.h"

#include "HandleMonitorCommand.h"

#include "IISxpressConsoleNotify.h"

class CCtrlCHandler
{

public:

	CCtrlCHandler(void)
	{
		VERIFY(::SetConsoleCtrlHandler(CtrlCHandler, TRUE));

		m_hCancel = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		ASSERT(m_hCancel != NULL);
	}

	~CCtrlCHandler(void)
	{
		VERIFY(::SetConsoleCtrlHandler(CtrlCHandler, FALSE));

		VERIFY(::CloseHandle(m_hCancel));
		m_hCancel = NULL;
	}

	static BOOL WINAPI CtrlCHandler(DWORD dwCtrlType);

	static HANDLE m_hCancel;

};

void HandleMonitorCommand(int argc, TCHAR* argv[])
{	
	// create a monitor object
	CComObject<CIISxpressConsoleNotify>* pNotify = NULL;
	HRESULT hr = CComObject<CIISxpressConsoleNotify>::CreateInstance(&pNotify);
	if (hr != S_OK)
	{		
		throw ProcessingException(hr, IDS_ERROR_MONITORFAILED);		
	}

	// we need an Unknown notify object (this also provides the ref count)
	CComPtr<IUnknown> pNotifyUnk;
	hr = pNotify->QueryInterface(IID_IUnknown, (void**) &pNotifyUnk);
	if (hr != S_OK)
	{		
		throw ProcessingException(hr, IDS_ERROR_MONITORFAILED);		
	}

	// get the active HTTPRequestUnk interface
	CComPtr<IUnknown> pHTTPRequestUnk;
	hr = ::GetActiveObject(CLSID_IISxpressHTTPRequest, NULL, (IUnknown**) &pHTTPRequestUnk);
	if (hr != S_OK)
	{
		throw ProcessingException(hr, IDS_ERROR_NORUNNINGIISXPRESS);				
	}

	// we need the real HTTP request interface since we want to connect to it
	CComQIPtr<IIISxpressHTTPRequest> pHTTPRequest = pHTTPRequestUnk;
	ASSERT(pHTTPRequest != NULL);

	// get the connection point container
	CComQIPtr<IConnectionPointContainer> pConnPointCont = pHTTPRequestUnk;
	if (pConnPointCont == NULL)
	{
		throw ProcessingException(E_NOINTERFACE, IDS_ERROR_MONITORFAILED);	
	}
	
	// locate the connection point
	CComPtr<IConnectionPoint> pHTTPNotifyCP;
	hr = pConnPointCont->FindConnectionPoint(IID_IIISxpressHTTPNotify, &pHTTPNotifyCP);
	if (pHTTPNotifyCP == NULL)
	{
		throw ProcessingException(hr, IDS_ERROR_MONITORFAILED);	
	}

	// ask the connection point to tell us about activity
	DWORD dwCookie = 0;
	hr = pHTTPNotifyCP->Advise(pNotifyUnk, &dwCookie);
	if (hr != S_OK)
	{
		throw ProcessingException(hr, IDS_ERROR_MONITORFAILED);				
	}

	// we need to handle CTRL-C
	CCtrlCHandler CtrlCHandler;

	// we need a message pump to allow COM to work - also look for CTRL-C
	while (::MsgWaitForMultipleObjects(1, &CtrlCHandler.m_hCancel, FALSE, 5000, QS_ALLINPUT) != WAIT_OBJECT_0)
	{		
		// pump any messages
		MSG msg;
		while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		// make sure the server is still alive
		DWORD dwTemp = 0;
		hr = pHTTPRequest->IsAlive(&dwTemp);
		if (hr != S_OK)
			break;
	}

	// tell the connection point that we are out of the game
	hr = pHTTPNotifyCP->Unadvise(dwCookie);
}

BOOL WINAPI CCtrlCHandler::CtrlCHandler(DWORD dwCtrlType)
{
	// signal that we have a ctrl-c
	::SetEvent(m_hCancel);

	// tell the console that we have it under control
	return TRUE;
}

HANDLE CCtrlCHandler::m_hCancel = NULL;