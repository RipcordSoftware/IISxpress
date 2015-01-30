#include "stdafx.h"

#include "ISAPIFilter.h"

namespace Ripcord { namespace IIS
{

ISAPIFilter::ISAPIFilter(void)
{
}

ISAPIFilter::~ISAPIFilter(void)
{
}

BOOL ISAPIFilter::GetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
	OnGetFilterVersion(pVer);
	return TRUE;
}

DWORD ISAPIFilter::HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD notificationType, LPVOID pvNotification)
{
	ISAPIFilterContent fc(pfc);

	switch (notificationType)
	{
		case SF_NOTIFY_URL_MAP:		
			return OnUrlMap(fc, (PHTTP_FILTER_URL_MAP) pvNotification);

		case SF_NOTIFY_SEND_RESPONSE:
			return OnSendResponse(fc, (PHTTP_FILTER_SEND_RESPONSE) pvNotification);

		case SF_NOTIFY_SEND_RAW_DATA:
			return OnSendRawData(fc, (PHTTP_FILTER_RAW_DATA) pvNotification);

		case SF_NOTIFY_END_OF_REQUEST:
			return OnEndOfRequest(fc);

		case SF_NOTIFY_END_OF_NET_SESSION:
			return OnEndOfNetSession(fc);

		default:					
			return SF_STATUS_REQ_NEXT_NOTIFICATION;
	}
}

BOOL ISAPIFilter::TerminateFilter(DWORD dwFlags)
{
	OnTerminateFilter();
	return FALSE;
}

}}