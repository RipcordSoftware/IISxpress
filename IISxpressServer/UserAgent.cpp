#include "stdafx.h"

#include "UserAgent.h"

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

_tagUserAgentProduct::_tagUserAgentProduct(const _tagUserAgentProduct& Product)
{
	sName = Product.sName;
	Comments = Product.Comments;
	bNot = Product.bNot;
}

void _tagUserAgentProduct::clear(void)
{
	sName.clear();
	Comments.clear();
	bNot = false;
}

bool _tagUserAgentProduct::Compare(const _tagUserAgentProduct& Product) const
{
	if (strncmp(Product.sName.c_str(), sName.c_str(), sName.size()) != 0)
	{		
		return false;
	}

	int nComments = (int) Comments.size();
	for (int i = 0; i < nComments; i++)
	{
		bool bMatch = false;

		const char* pszComment = Comments[i].c_str();
		int nCommentSize = (int) Comments[i].size();

		int nOtherComments = (int) Product.Comments.size();
		for (int j = 0; j < nOtherComments; j++)
		{
			const char* pszOtherComment = Product.Comments[j].c_str();
			
			if (strncmp(pszComment, pszOtherComment, nCommentSize) == 0)
			{
				bMatch = true;
				break;					
			}
		}

		if (bMatch == false)
		{
			return false;
		}
	}

	return true;
}

HRESULT _tagUserAgentProduct::ParseUserAgentString(LPCSTR pszUserAgent, int* pnSize)
{
	if (pszUserAgent == NULL || pnSize == NULL)
		return E_POINTER;

	*pnSize = 0;

	clear();

	const char* pszTempUserAgent = pszUserAgent;

	// skip any initial whitespace
	while (pszTempUserAgent[0] == ' ')
	{
		pszTempUserAgent++;
		(*pnSize)++;
	}

	// if the first non-whitespace is a '(' then it's an error
	if (pszTempUserAgent[0] == '(')
	{
		return E_INVALIDARG;
	}

	// find the next space character
	const char* pszWhitespace = strchr(pszTempUserAgent, ' ');

	// there is no more whitespace, the remaining text must be a product
	if (pszWhitespace == NULL)
	{
		sName = pszTempUserAgent;		
		(*pnSize) += (int) sName.size();
	}
	else
	{
		int nProductNameSize = (int) (pszWhitespace - pszTempUserAgent);
		(*pnSize) += nProductNameSize;

		// assign the product name
		sName.assign(pszTempUserAgent, nProductNameSize);

		// skip any following whitespace
		while (pszWhitespace[0] == ' ')
		{
			pszWhitespace++;
			(*pnSize)++;
		}

		// get the comments if there are any
		if (pszWhitespace[0] == '(')
		{
			pszWhitespace++;
			(*pnSize)++;

			bool bError = false;
			bool bTerm = false;

			// loop until a) the end of the string, or b) the closing bracket
			while (bError == false && bTerm == false)
			{
				// skip any following whitespace
				while (pszWhitespace[0] == ' ')
				{
					pszWhitespace++;
					(*pnSize)++;
				}

				pszTempUserAgent = pszWhitespace;

				while (1)
				{
					pszWhitespace++;
					(*pnSize)++;

					if (pszWhitespace[0] == '\0')
					{
						// the string has finished, this is bad
						bError = true;
						break;
					}

					if (pszWhitespace[0] == ')')
					{
						// the comment block has ended
						bTerm = true;
						break;
					}

					if (pszWhitespace[0] == ';')
					{
						// the current comment is ended
						break;
					}					
				}			

				if (bError == true)
				{
					return E_INVALIDARG;
				}

				std::string sComment(pszTempUserAgent, pszWhitespace - pszTempUserAgent);

				Comments.push_back(sComment);

				// skip the break character
				pszWhitespace++;
				(*pnSize)++;
			}
		}
	}

	return S_OK;
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

_tagUserAgentProducts::_tagUserAgentProducts(const _tagUserAgentProducts& UAProducts)
{
	RootProduct = UAProducts.RootProduct;
	Products = UAProducts.Products;
}

void _tagUserAgentProducts::clear(void)
{
	RootProduct.clear();
	Products.clear();
}

bool _tagUserAgentProducts::Compare(const _tagUserAgentProducts& OtherProducts) const
{
	if (RootProduct.Compare(OtherProducts.RootProduct) == false)
		return false;	
	
	int nProducts = (int) Products.size();
	for (int i = 0; i < nProducts; i++)
	{		
		bool bMatch = false;

		int nOtherProducts = (int) OtherProducts.Products.size();
		for (int j = 0; j < nOtherProducts; j++)
		{
			if (Products[i].Compare(OtherProducts.Products[j]) == true)
			{
				bMatch = true;
				break;
			}
		}

		// if the product has 'not' specified then we need to take the following action
		// a) if the product has been found then it's definately not a match so return
		// b) if the product has NOT been found then since it's a not condition pretend we actually found it
		if (Products[i].bNot == true)
		{
			if (bMatch == true)
				return false;
			else
				bMatch = true;
		}		

		if (bMatch == false)
			return false;				
	}
	
	return true;
}

HRESULT _tagUserAgentProducts::ParseUserAgentString(LPCSTR pszUserAgent)
{
	if (pszUserAgent == NULL)
		return E_POINTER;

	clear();

	const char* pszTempUserAgent = pszUserAgent;

	int nSize = 0;
	RootProduct.ParseUserAgentString(pszTempUserAgent, &nSize);	
	
	while (pszTempUserAgent[nSize] != '\0')
	{
		pszTempUserAgent += nSize;

		nSize = 0;

		UserAgentProduct Product;
		HRESULT hr = Product.ParseUserAgentString(pszTempUserAgent, &nSize);

		// if we couldn't parse the string then clean up and return fail
		if (FAILED(hr) == TRUE)
		{
			clear();
			return E_FAIL;
		}

		Products.push_back(Product);
	}

	return S_OK;
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void _tagRuleUserAgents::clear(void)
{
	int nUAProducts = (int) UAProducts.size();
	for (int i = 0; i < nUAProducts; i++)
	{
		UAProducts[i].clear();
	}
}

bool _tagRuleUserAgents::Compare(const UserAgentProducts& OtherUAProducts) const
{
	int nUAProducts = (int) UAProducts.size();
	for (int i = 0; i < nUAProducts; i++)
	{					
		if (UAProducts[i].Compare(OtherUAProducts) == true)
		{				
			return true;
		}			
	}

	return false;
}