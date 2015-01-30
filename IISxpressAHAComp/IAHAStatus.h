#pragma once

namespace AHA
{

enum AHABoardInstallState : DWORD
{	
	Installed = 0,
	Reinstall = 1,
	FailedInstall = 2,
	FinishInstall = 3
};

struct AHABoardInfo
{
	AHABoardInfo()
	{
		enabled = true;
		szDescription[0] = '\0';
		InstallState = FinishInstall;
	}

	bool enabled;
	WCHAR szDescription[256];
	AHABoardInstallState InstallState;
};

class IAHAStatus
{
public:

	virtual bool GetStats(STATS& stats) = 0;
	virtual bool ClearStats() = 0;

	virtual bool GetTemps(DWORD& dwFPGA, DWORD& dwBoard) = 0;

	virtual bool GetBoardInfo(AHABoardInfo& info) = 0;
};

};