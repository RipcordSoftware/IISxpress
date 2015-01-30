#pragma once

#include "IISxpressHttpModule.h"

class CIISxpressModuleFactory : public IHttpModuleFactory
{

public:

	CIISxpressModuleFactory(IHttpServer* pHttpServer) : m_pIISxpressHttpModule(NULL), m_pHttpServer(pHttpServer) {}
	virtual ~CIISxpressModuleFactory(void) {}

	virtual HRESULT GetHttpModule(OUT CHttpModule** ppModule, IN IModuleAllocator*);
	virtual void Terminate();

private:
	
	CComAutoCriticalSection			m_csIISxpressHttpModule;
	CIISxpressHttpModule*			m_pIISxpressHttpModule;

	IHttpServer*					m_pHttpServer;
};
