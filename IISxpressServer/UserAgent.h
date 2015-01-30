#pragma once

#include <string>
#include <vector>

typedef struct _tagUserAgentProduct
{
	_tagUserAgentProduct() { bNot = false;}

	_tagUserAgentProduct(const _tagUserAgentProduct& Product);

	void clear(void);	

	bool Compare(const _tagUserAgentProduct& Product) const;

	HRESULT ParseUserAgentString(LPCSTR pszUserAgent, int* pnSize);

	std::string					sName;
	std::vector<std::string>	Comments;
	bool						bNot;

} UserAgentProduct;

typedef struct _tagUserAgentProducts
{
	_tagUserAgentProducts() {}

	_tagUserAgentProducts(const _tagUserAgentProducts& UAProducts);	

	void clear(void);	

	bool Compare(const _tagUserAgentProducts& OtherProducts) const;

	HRESULT ParseUserAgentString(LPCSTR pszUserAgent);
	
	UserAgentProduct				RootProduct;
	std::vector<UserAgentProduct>	Products;

} UserAgentProducts;

typedef struct _tagRuleUserAgents
{	
	void clear(void);

	bool Compare(const UserAgentProducts& OtherUAProducts) const;	

	std::vector<UserAgentProducts>	UAProducts;

} RuleUserAgents;