#include "StdAfx.h"
#include ".\enumexclusionrulehelper.h"

CEnumExclusionRuleHelper::CEnumExclusionRuleHelper(void)
{
}

CEnumExclusionRuleHelper::~CEnumExclusionRuleHelper(void)
{
}

void CEnumExclusionRuleHelper::DestroyExclusionRuleInfo(ExclusionRuleInfo& Info)
{
	if (Info.bsRule != NULL)
	{
		::SysFreeString(Info.bsRule);
		Info.bsRule = NULL;
	}

	if (Info.bsInstance != NULL)
	{
		::SysFreeString(Info.bsInstance);
		Info.bsInstance = NULL;
	}

	for (int i = 0; i < Info.nUserAgents; i++)
	{
		if (Info.pbsUserAgentIDs[i] != NULL)
		{
			::SysFreeString(Info.pbsUserAgentIDs[i]);
			Info.pbsUserAgentIDs[i] = NULL;
		}
	}	

	Info.nUserAgents = 0;

	if (Info.pbsUserAgentIDs != NULL)
	{
		::CoTaskMemFree(Info.pbsUserAgentIDs);
		Info.pbsUserAgentIDs = NULL;
	}	
}

void CEnumExclusionRuleHelper::CopyExclusionRuleInfo(const ExclusionRuleInfo& Source, ExclusionRuleInfo& Target) 
{
	DestroyExclusionRuleInfo(Target);

	if (Source.nUserAgents > 0)
	{
		Target.pbsUserAgentIDs = (BSTR*) ::CoTaskMemAlloc(sizeof(BSTR) * Source.nUserAgents);	
		if (Target.pbsUserAgentIDs == NULL)
			return;
	}
	else
	{
		Target.pbsUserAgentIDs = 0;
	}

	if (Source.bsRule != NULL)
	{
		Target.bsRule = ::SysAllocString(Source.bsRule);
	}
	else
	{
		Target.bsRule = NULL;
	}

	if (Source.bsInstance != NULL)
	{
		Target.bsInstance = ::SysAllocString(Source.bsInstance);
	}
	else
	{
		Target.bsInstance = NULL;
	}

	Target.dwFlags = Source.dwFlags;

	Target.nUserAgents = Source.nUserAgents;
	for (int i = 0; i < Target.nUserAgents; i++)
	{
		Target.pbsUserAgentIDs[i] = ::SysAllocString(Source.pbsUserAgentIDs[i]);
	}
}