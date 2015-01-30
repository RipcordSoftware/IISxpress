#pragma once

#include "HTTPNotify.h"

namespace IISxpressAPI 
{
	/// <exclude/>
	public ref class ResponseHistory
	{

	public:

		ResponseHistory();

		!ResponseHistory();
		~ResponseHistory();

		array<ResponseDetails^>^ GetResponses();
		void ClearResponses();

	private:

		HRESULT GetHTTPRequest(IIISxpressHTTPRequest** ppHTTPRequest);		

		void AttachConnectionPoint();
		void DetachConnectionPoint();

		CComObject<CHTTPNotify>*	m_pNotifyObj;
		IConnectionPoint*			m_pHTTPNotifyCP;
		DWORD						m_dwCookie;

	};
}