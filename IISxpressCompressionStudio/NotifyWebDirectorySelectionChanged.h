#pragma once

#include "IISConfigHelper.h"

// CNotifyWebDirectorySelectionChanged command target

class CNotifyWebDirectorySelectionChanged : public CObject
{
	DECLARE_DYNAMIC(CNotifyWebDirectorySelectionChanged)

public:
	CNotifyWebDirectorySelectionChanged();
	virtual ~CNotifyWebDirectorySelectionChanged();

	CAtlArray<IISWebSiteBindings> Ports;
	CAtlArray<IISWebSiteBindings> SecurePorts;

	CString sPhysicalPath;
	CString sURI;
};


