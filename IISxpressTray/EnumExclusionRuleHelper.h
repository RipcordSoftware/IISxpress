#pragma once

class CEnumExclusionRuleHelper
{

private:

	CEnumExclusionRuleHelper(void);
	~CEnumExclusionRuleHelper(void);

public:

	static void DestroyExclusionRuleInfo(ExclusionRuleInfo& Info);
	static void CopyExclusionRuleInfo(const ExclusionRuleInfo& Source, ExclusionRuleInfo& Target);

};
