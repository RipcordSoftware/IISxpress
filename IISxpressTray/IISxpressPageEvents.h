#pragma once

class IIISxpressPageEvents : public CObject
{

	DECLARE_DYNAMIC(IIISxpressPageEvents)

public:
	virtual void ServerOnline(void) = 0;
	virtual void ServerOffline(void) = 0;
	virtual void Heartbeat(void) = 0;
};