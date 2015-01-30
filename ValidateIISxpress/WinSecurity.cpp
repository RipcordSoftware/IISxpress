#include "StdAfx.h"
#include "WinSecurity.h"

RS::WinSecurity::WinSecurity::WinSecurity(void)
{
}

RS::WinSecurity::WinSecurity::~WinSecurity(void)
{
}

bool RS::WinSecurity::WinSecurity::GetAccountSid(LPCTSTR pszAccount, SmartSid& sid)
{
	DWORD dwSidAccountSize = 0;
	TCHAR szDomainName[256];
	DWORD dwDomainNameSize = _countof(szDomainName);
	SID_NAME_USE accountType = SidTypeUnknown;
	::LookupAccountName(NULL, pszAccount, NULL, &dwSidAccountSize, szDomainName, &dwDomainNameSize, &accountType);
	if (dwSidAccountSize == 0)
	{
		return false;
	}

	BYTE* pbyBuffer = (BYTE*) alloca(dwSidAccountSize);
	if (::LookupAccountName(NULL, pszAccount, (PSID) pbyBuffer, &dwSidAccountSize, szDomainName, &dwDomainNameSize, &accountType))
	{
		sid = SmartSid(pbyBuffer, dwSidAccountSize);
		return true;
	}
	else
	{
		return false;
	}
}

bool RS::WinSecurity::WinSecurity::CheckPermissons(SmartSecurityDescriptor& SD, SmartSid& sid, ACCESS_MASK mask)
{
	if (!SD.IsValid() || !sid.IsValid())
	{
		return false;
	}

	BOOL daclPresent = FALSE;
	PACL pAcl = NULL;
	BOOL daclDefaulted = FALSE;
	if (!::GetSecurityDescriptorDacl(SD, &daclPresent, &pAcl, &daclDefaulted) || !::IsValidAcl(pAcl))
	{
		return false;
	}

	bool access = false;

	PACE_HEADER pAce = NULL;
	int index = 0;
	while (::GetAce(pAcl, index, (LPVOID*) &pAce))
	{
		if (pAce->AceType == ACCESS_ALLOWED_ACE_TYPE)
		{
			PACCESS_ALLOWED_ACE pAccessAllowedAce = (PACCESS_ALLOWED_ACE) pAce;
			SID* pSid = (SID*) &pAccessAllowedAce->SidStart;
			DWORD sidLength = ::GetLengthSid(pSid);			

			if (sid.Compare(pSid, sidLength) && (pAccessAllowedAce->Mask & mask) == mask)
			{
				access = true;
				break;
			}
		}

		++index;
	}

	return access;
}

bool RS::WinSecurity::WinSecurity::GetKeySecurity(HKEY key, SmartSecurityDescriptor& SD)
{
	BYTE bySDBuffer[1024];
	DWORD dwSDBufferSize = sizeof(bySDBuffer);
	if (::RegGetKeySecurity(key, DACL_SECURITY_INFORMATION, (SECURITY_DESCRIPTOR*) bySDBuffer, &dwSDBufferSize) == ERROR_SUCCESS)
	{	
		dwSDBufferSize = ::GetSecurityDescriptorLength(bySDBuffer);
		SD = SmartSecurityDescriptor(bySDBuffer, dwSDBufferSize);
		return true;
	}
	else
	{
		return false;
	}
}

bool RS::WinSecurity::WinSecurity::GetFileSecurity(LPCTSTR pszFileName, SmartSecurityDescriptor& SD)
{
	BYTE bySDBuffer[1024];
	DWORD dwSDBufferSize = sizeof(bySDBuffer);
	if (::GetFileSecurity(pszFileName, DACL_SECURITY_INFORMATION, (SECURITY_DESCRIPTOR*) bySDBuffer, dwSDBufferSize, &dwSDBufferSize) == TRUE)
	{	
		dwSDBufferSize = ::GetSecurityDescriptorLength(bySDBuffer);
		SD = SmartSecurityDescriptor(bySDBuffer, dwSDBufferSize);
		return true;
	}
	else
	{
		return false;
	}
}