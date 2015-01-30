#include "StdAfx.h"
#include "PopupMenuHelper.h"

bool CPopupMenuHelper::CheckMenuItem(CMenu* pMenu, UINT nID, UINT nCheck)
{
	if (pMenu == NULL)
		return false;	

	int nItems = pMenu->GetMenuItemCount();
	for (int i = 0; i < nItems; i++)
	{
		UINT nItemID = pMenu->GetMenuItemID(i);
		if (nItemID == nID)
		{
			pMenu->CheckMenuItem(nID, MF_BYCOMMAND | nCheck);
			return true;
		}
		else if (nItemID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);
			if (pSubMenu != NULL)
			{
				if (CheckMenuItem(pSubMenu, nID, nCheck) == true)
					return true;
			}
		}
	}

	return false;
}

bool CPopupMenuHelper::EnableMenuItem(CMenu* pMenu, UINT nID, UINT uEnable)
{
	if (pMenu == NULL)
		return false;	

	int nItems = pMenu->GetMenuItemCount();
	for (int i = 0; i < nItems; i++)
	{
		UINT nItemID = pMenu->GetMenuItemID(i);
		if (nItemID == nID)
		{
			pMenu->EnableMenuItem(nID, MF_BYCOMMAND | uEnable);
			return true;
		}
		else if (nItemID == -1)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(i);
			if (pSubMenu != NULL)
			{
				if (EnableMenuItem(pSubMenu, nID, uEnable) == true)
					return true;
			}
		}
	}

	return false;
}