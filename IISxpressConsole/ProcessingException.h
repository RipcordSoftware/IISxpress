#pragma once

typedef struct _tagProcessingException
{

public:

	_tagProcessingException(HRESULT hr, LPCTSTR pszError)
	{
		m_hrError = hr;
		m_sError = pszError;
	}

	_tagProcessingException(HRESULT hr, UINT nErrorId)
	{
		m_hrError = hr;
		VERIFY(m_sError.LoadString(nErrorId));
	}

	HRESULT		m_hrError;
	CString		m_sError;

} ProcessingException;