#include "stdafx.h"

#include "IISxpress.h"

IISxpressFilter filter;

BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
	return filter.GetFilterVersion(pVer);		
}

DWORD WINAPI HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD notificationType, LPVOID pvNotification)
{
	return filter.HttpFilterProc(pfc, notificationType, pvNotification);	
}

BOOL WINAPI TerminateFilter(DWORD dwFlags)
{
	return filter.TerminateFilter(dwFlags);	
}