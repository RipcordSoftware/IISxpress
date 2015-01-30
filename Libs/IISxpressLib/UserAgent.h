#pragma once

#include <vector>
#include <map>

namespace Ripcord { namespace IISxpress { namespace HttpUserAgent {

// this class represents a simple user agent product+comment pair
template <typename T> class UserAgentProduct
{
public:
	UserAgentProduct() : not(false) { }

	void Clear(void);

	bool Compare(const UserAgentProduct<T>& Product) const;

	HRESULT ParseUserAgentString(const typename T::value_type* pszUserAgent, int& nSize);

	bool Not() const { return not; } 

private:
	T				name;
	std::vector<T>	comments;
	bool			not;
};

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

// this class represents a collection of user agent product+comment pairs
template <typename T> class UserAgentProducts
{
public:
	UserAgentProducts() {}

	void Clear(void);	

	bool Compare(const UserAgentProducts<T>& OtherProducts) const;
	bool Compare(const UserAgentProducts<T>& OtherProducts, bool& matchedNot) const;

	HRESULT ParseUserAgentString(const typename T::value_type* pszUserAgent);

private:	
	UserAgentProduct<T>						rootProduct;
	std::vector<UserAgentProduct<T> >		products;
};

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

// this class represents a collection of complex user agents ordered by significance
template <typename T> class RuleUserAgents
{	
public:
	void Clear(void);

	bool Compare(const UserAgentProducts<T>& OtherUAProducts) const;	

	void AddUserAgent(UserAgentProducts<T>& userAgent);

private:
	std::vector<UserAgentProducts<T> >	UAProducts;
};

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

template <typename T> class UserAgentCache
{
public:

	UserAgentCache(size_t maxSize) : cookie(0), maxSize(maxSize) {}

	bool AddUserAgentState(DWORD cookie, const typename T::value_type* pszUserAgent, bool excluded);
	bool GetUserAgentState(DWORD cookie, const typename T::value_type* pszUserAgent, bool& excluded);		

private:

	void Clear() 
	{ 
		CComCritSecLock<CComAutoCriticalSection> cs(m_csLookup); 
		userAgentLookup.clear(); 
	}

	typedef std::map<T, bool>	CollType;
	CollType					userAgentLookup;
	volatile DWORD				cookie;

	size_t						maxSize;

	CComAutoCriticalSection		m_csLookup;
};

template <typename T> void UserAgentProduct<T>::Clear(void)
{
	name.clear();
	comments.clear();
	not = false;
}

template <typename T> bool UserAgentProduct<T>::Compare(const UserAgentProduct& Product) const
{
	if (Product.name.compare(0, name.size(), name) != 0)
	{		
		return false;
	}		

	int nComments = (int) comments.size();
	for (int i = 0; i < nComments; i++)
	{
		bool bMatch = false;

		const T& comment = comments[i];

		int nOtherComments = (int) Product.comments.size();
		for (int j = 0; j < nOtherComments; j++)
		{
			const T& productComment = Product.comments[j];
			if (productComment.compare(0, comment.size(), comment) == 0)
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

template <typename T> HRESULT UserAgentProduct<T>::ParseUserAgentString(const typename T::value_type* pszUserAgent, int& nSize)
{
	if (pszUserAgent == NULL)
		return E_POINTER;

	nSize = 0;

	Clear();

	const typename T::value_type* pszTempUserAgent = pszUserAgent;

	// skip any initial whitespace
	while (pszTempUserAgent[0] == ' ')
	{
		pszTempUserAgent++;
		nSize++;
	}

	// get the not status
	if (pszTempUserAgent[0] == '!')
	{
		not = true;
		pszTempUserAgent++;
		nSize++;
	}

	// skip any following whitespace
	while (pszTempUserAgent[0] == ' ')
	{
		pszTempUserAgent++;
		nSize++;
	}	

	// if the first non-whitespace is a '(' or ')' then it's an error
	if (pszTempUserAgent[0] == '(' || pszTempUserAgent[0] == ')')
	{
		return E_INVALIDARG;
	}

	// find the next space character	
	const typename T::value_type* pszWhitespace = pszTempUserAgent;
	while (*pszWhitespace != '\0')
	{
		if (*pszWhitespace == ' ' || *pszWhitespace == '(' || *pszWhitespace == ')')
		{
			break;
		}

		pszWhitespace++;
	}

	// there is no more whitespace, the remaining text must be a product
	if (pszWhitespace == '\0')
	{
		name = pszTempUserAgent;		
		nSize += (int) name.length();
	}
	else
	{
		int nProductNameSize = (int) (pszWhitespace - pszTempUserAgent);
		nSize += nProductNameSize;

		// assign the product name
		name.assign(pszTempUserAgent, nProductNameSize);

		// skip any following whitespace
		while (pszWhitespace[0] == ' ')
		{
			pszWhitespace++;
			nSize++;
		}

		// get the comments if there are any
		if (pszWhitespace[0] == '(')
		{
			pszWhitespace++;
			nSize++;

			bool bError = false;
			bool bTerm = false;

			// loop until a) the end of the string, or b) the closing bracket
			while (bError == false && bTerm == false)
			{
				// skip any following whitespace
				while (pszWhitespace[0] == ' ')
				{
					pszWhitespace++;
					nSize++;
				}

				pszTempUserAgent = pszWhitespace;

				while (1)
				{				
					if (pszWhitespace[0] == '\0')
					{
						// the string has finished, this is bad
						bError = true;
						break;
					}

					if (pszWhitespace[0] == '(')
					{
						// we have found another opening bracket, this is bad
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

					pszWhitespace++;
					nSize++;
				}			

				if (bError == true)
				{
					return E_INVALIDARG;
				}

				T sComment(pszTempUserAgent, pszWhitespace - pszTempUserAgent);

				if (sComment.size() > 0)
				{
					comments.push_back(sComment);
				}

				// skip the break character
				pszWhitespace++;
				nSize++;
			}
		}
	}

	return S_OK;
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

template <typename T> void UserAgentProducts<T>::Clear(void)
{
	rootProduct.Clear();
	products.clear();
}

template <typename T> bool UserAgentProducts<T>::Compare(const UserAgentProducts<T>& OtherProducts) const
{	
	bool not;
	return Compare(OtherProducts, not);
}

template <typename T> bool UserAgentProducts<T>::Compare(const UserAgentProducts<T>& OtherProducts, bool& matchedNot) const
{
	matchedNot = false;

	if (rootProduct.Compare(OtherProducts.rootProduct) == false)
	{		
		return false;
	}	

	matchedNot |= rootProduct.Not();
	
	int nProducts = (int) products.size();
	for (int i = 0; i < nProducts; i++)
	{		
		bool bMatch = false;

		int nOtherProducts = (int) OtherProducts.products.size();
		for (int j = 0; j < nOtherProducts; j++)
		{
			if (products[i].Compare(OtherProducts.products[j]) == true)
			{
				bMatch = true;
				break;
			}
		}

		if (!bMatch)
		{
			return false;
		}

		matchedNot |= products[i].Not();
	}
	
	return true;
}

template <typename T> HRESULT UserAgentProducts<T>::ParseUserAgentString(const typename T::value_type* pszUserAgent)
{
	if (pszUserAgent == NULL)
		return E_POINTER;

	Clear();

	const T::value_type* pszTempUserAgent = pszUserAgent;

	int nSize = 0;
	HRESULT hr = rootProduct.ParseUserAgentString(pszTempUserAgent, nSize);	
	
	while (hr == S_OK && pszTempUserAgent[nSize] != '\0')
	{
		pszTempUserAgent += nSize;

		nSize = 0;

		UserAgentProduct<T> Product;
		hr = Product.ParseUserAgentString(pszTempUserAgent, nSize);
		
		if (hr == S_OK)
		{
			products.push_back(Product);				
		}			
	}

	if (FAILED(hr))
	{
		Clear();
	}

	return hr;
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

template <typename T> void RuleUserAgents<T>::Clear(void)
{
	int nUAProducts = (int) UAProducts.size();
	for (int i = 0; i < nUAProducts; i++)
	{
		UAProducts[i].Clear();
	}

	UAProducts.clear();
}

template <typename T> bool RuleUserAgents<T>::Compare(const UserAgentProducts<T>& OtherUAProducts) const
{
	int nUAProducts = (int) UAProducts.size();
	for (int i = 0; i < nUAProducts; i++)
	{					
		bool matchedNot = false;
		if (UAProducts[i].Compare(OtherUAProducts, matchedNot))
		{				
			return !matchedNot;
		}			
	}

	return false;
}

template <typename T> void RuleUserAgents<T>::AddUserAgent(UserAgentProducts<T>& userAgent)
{
	UAProducts.push_back(userAgent);
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

template <typename T> bool UserAgentCache<T>::AddUserAgentState(DWORD cookie, const typename T::value_type* pszUserAgent, bool excluded)
{
	if (pszUserAgent == NULL || this->cookie != cookie)
	{
		return false;
	}

	CComCritSecLock<CComAutoCriticalSection> cs(m_csLookup);

	// simple culling mechanism
	if (maxSize > 0 && userAgentLookup.size() >= maxSize)
	{
		Clear();
	}

	CollType::iterator iter = userAgentLookup.find(pszUserAgent);
	if (iter == userAgentLookup.end())
	{
		userAgentLookup[pszUserAgent] = excluded;
	}		
	else
	{
		iter->second = excluded;
	}

	return true;
}

template <typename T> bool UserAgentCache<T>::GetUserAgentState(DWORD cookie, const typename T::value_type* pszUserAgent, bool& excluded)
{
	if (pszUserAgent == NULL)
	{
		return false;
	}

	bool status = false;

	CComCritSecLock<CComAutoCriticalSection> cs(m_csLookup);

	if (this->cookie != cookie)
	{
		Clear();
		this->cookie = cookie;
	}
	else
	{
		CollType::const_iterator iter = userAgentLookup.find(pszUserAgent);
		if (iter != userAgentLookup.end())
		{
			excluded = iter->second;
			status = true;			
		}
	}
	
	return status;
}

}}}