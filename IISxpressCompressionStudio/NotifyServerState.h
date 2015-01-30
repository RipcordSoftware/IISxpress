#pragma once

// CNotifyServerState command target

class CNotifyServerState : public CObject
{
	DECLARE_DYNAMIC(CNotifyServerState)

public:
	CNotifyServerState(bool bServerOnline) : m_bServerOnline(bServerOnline) {}	

	bool ServerOnline() { return m_bServerOnline; }

private:

	bool m_bServerOnline;
};


