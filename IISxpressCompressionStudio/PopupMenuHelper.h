#pragma once

class CPopupMenuHelper
{
private:
	CPopupMenuHelper(void) {}
	
public:

	static bool CheckMenuItem(CMenu* pMenu, UINT nID, UINT nCheck);
	static bool EnableMenuItem(CMenu* pMenu, UINT nID, UINT uEnable);
};
