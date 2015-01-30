#include "StdAfx.h"

#include "IStaticFileRulesImpl.h"
#include "EnumStaticFileInfo.h"

#define IISXPRESS_STATICFILERULES	_T("StaticFileRules")

#define CUSTOMCOMPRESSIONENABLED		_T("customCompEnabled")
#define CUSTOMCOMPRESSIONMODE			_T("customCompMode")
#define CUSTOMCOMPRESSIONLEVEL			_T("customCompLevel")
#define CUSTOMCOMPRESSIONWINDOWBITS		_T("customCompWindowBits")
#define CUSTOMCOMPRESSIONMEMORYLEVEL	_T("customCompMemoryLevel")

IStaticFileRulesImpl::IStaticFileRulesImpl(void)
{
}

IStaticFileRulesImpl::~IStaticFileRulesImpl(void)
{
}

STDMETHODIMP IStaticFileRulesImpl::Init(IStorage* pStorage)
{
	HRESULT hr = E_FAIL;
	
	CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

	try
	{
		THROW_HR_ON_NULL(pStorage, hr, E_POINTER);

		hr = m_pDOM.CoCreateInstance(L"MSXML.DOMDocument");
		THROW_HR_ON_FAIL(hr);

		m_pConfigStorage = pStorage; 

		hr = Load();
		THROW_HR_ON_FAIL(hr);
	}
	catch (HRESULT)
	{		
	}	

	return hr;
}

STDMETHODIMP IStaticFileRulesImpl::IsKnownFile(LPCSTR pszFullFilePath)
{
	CComPtr<IXMLDOMNode> pFileNode;
	return GetFileNode(pszFullFilePath, &pFileNode);
}

HRESULT IStaticFileRulesImpl::GetFileNode(LPCSTR pszFullFilePath, IXMLDOMNode** ppFileNode)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);
		THROW_HR_ON_NULL(ppFileNode, hr, E_POINTER);
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		CStringA sFullFilePath = pszFullFilePath;
		sFullFilePath.Replace("\\", "\\\\");

		CString sQuery;
		sQuery.Format(_T("/files/file[fullpath='%hs']"), sFullFilePath);
		
		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);
		
		hr = m_pDOM->selectSingleNode(CComBSTR(sQuery), ppFileNode);		

		if (*ppFileNode == NULL)
			hr = S_FALSE;
		else
			hr = S_OK;
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT IStaticFileRulesImpl::EnsureFileNode(LPCSTR pszFullFilePath, IXMLDOMNode** ppFileNode)
{
	HRESULT hr = E_FAIL;

	THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);
	THROW_HR_ON_NULL(ppFileNode, hr, E_POINTER);
	THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

	CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

	hr = GetFileNode(pszFullFilePath, ppFileNode);
	if (hr != S_OK)
	{
		hr = CreateFileNode(pszFullFilePath, ppFileNode);
	}

	return hr;
}

HRESULT IStaticFileRulesImpl::CreateFileNode(LPCSTR pszFullFilePath, IXMLDOMNode** ppFileNode)
{
	HRESULT hr = E_FAIL;

	THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);
	THROW_HR_ON_NULL(ppFileNode, hr, E_POINTER);
	THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

	CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

	CComPtr<IXMLDOMNode> pFilesNode;
	hr = m_pDOM->selectSingleNode(CComBSTR(L"/files"), &pFilesNode);
	THROW_HR_ON_FAIL(hr);

	// ******************************************************************			
	CPathT<CStringA> Name(pszFullFilePath);
	Name.StripPath();

	CPathT<CStringA> Path(pszFullFilePath);
	Path.RemoveFileSpec();
	CStringA sPath = Path;			

	// ******************************************************************			
	CComPtr<IXMLDOMNode> pFileNode;
	hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"file"), CComBSTR(), &pFileNode);
	THROW_HR_ON_FAIL(hr);

	CComPtr<IXMLDOMNode> pNameNode;
	hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"name"), CComBSTR(), &pNameNode);
	THROW_HR_ON_FAIL(hr);

	hr = pNameNode->put_text(CComBSTR(Name));
	THROW_HR_ON_FAIL(hr);

	CComPtr<IXMLDOMNode> pPathNode;
	hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"path"), CComBSTR(), &pPathNode);
	THROW_HR_ON_FAIL(hr);

	hr = pPathNode->put_text(CComBSTR(sPath));
	THROW_HR_ON_FAIL(hr);

	CComPtr<IXMLDOMNode> pFullPathNode;
	hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"fullpath"), CComBSTR(), &pFullPathNode);
	THROW_HR_ON_FAIL(hr);

	hr = pFullPathNode->put_text(CComBSTR(pszFullFilePath));
	THROW_HR_ON_FAIL(hr);			

	// ******************************************************************			

	hr = pFileNode->appendChild(pNameNode, NULL);
	THROW_HR_ON_FAIL(hr);

	hr = pFileNode->appendChild(pPathNode, NULL);
	THROW_HR_ON_FAIL(hr);

	hr = pFileNode->appendChild(pFullPathNode, NULL);
	THROW_HR_ON_FAIL(hr);

	hr = pFilesNode->appendChild(pFileNode, ppFileNode);
	THROW_HR_ON_FAIL(hr);			

	return hr;
}

STDMETHODIMP IStaticFileRulesImpl::AddFileRule(LPCSTR pszFullFilePath, int nUserCompress, int nSystemCompress)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);		
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNode> pFileNode;
		hr = GetFileNode(pszFullFilePath, &pFileNode);

		if (pFileNode == NULL)
		{
			hr = CreateFileNode(pszFullFilePath, &pFileNode);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMAttribute> pUserCompressionAttrib;
			hr = m_pDOM->createAttribute(CComBSTR(L"usercompression"), &pUserCompressionAttrib);
			THROW_HR_ON_FAIL(hr);

			CComBSTR bsUserCompress;
			StateToCompressionString(nUserCompress, bsUserCompress);			 

			hr = pUserCompressionAttrib->put_text(bsUserCompress);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMAttribute> pSystemCompressionAttrib;
			hr = m_pDOM->createAttribute(CComBSTR(L"systemcompression"), &pSystemCompressionAttrib);
			THROW_HR_ON_FAIL(hr);

			CComBSTR bsSystemCompress;
			StateToCompressionString(nSystemCompress, bsSystemCompress);			

			hr = pSystemCompressionAttrib->put_text(bsSystemCompress);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNamedNodeMap> pAttribs;
			hr = pFileNode->get_attributes(&pAttribs);
			THROW_HR_ON_FAIL(hr);

			hr = pAttribs->setNamedItem(pUserCompressionAttrib, NULL);
			THROW_HR_ON_FAIL(hr);

			hr = pAttribs->setNamedItem(pSystemCompressionAttrib, NULL);
			THROW_HR_ON_FAIL(hr);			

			// ******************************************************************									

			hr = Save();
			THROW_HR_ON_FAIL(hr);
		}
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP IStaticFileRulesImpl::RemoveFileRule(LPCSTR pszFullFilePath)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);		
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNode> pFileNode;
		GetFileNode(pszFullFilePath, &pFileNode);

		if (pFileNode != NULL)
		{						
			CComPtr<IXMLDOMNode> pFilesNode;
			hr = m_pDOM->selectSingleNode(CComBSTR(L"/files"), &pFilesNode);
			THROW_HR_ON_FAIL(hr);

			if (hr == S_OK)
			{				
				hr = pFilesNode->removeChild(pFileNode, NULL);
				THROW_HR_ON_FAIL(hr);
				
				hr = Save();
			}			
		}
		else
		{
			hr = S_FALSE;
		}
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP IStaticFileRulesImpl::GetFileRule(LPCSTR pszFullFilePath, int* pnUserCompress, int* pnSystemCompress)
{
	return GetFileRule(pszFullFilePath, pnUserCompress, pnSystemCompress, NULL);
}

STDMETHODIMP IStaticFileRulesImpl::GetFileRule(LPCSTR pszFullFilePath, int* pnUserCompress, int* pnSystemCompress, CustomCompressionSettings* pCustomCompressionSettings)
{
	HRESULT hr = E_FAIL;

	try
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNode> pFileNode;
		hr = GetFileNode(pszFullFilePath, &pFileNode);
		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{
			CComPtr<IXMLDOMNamedNodeMap> pAttribs;
			hr = pFileNode->get_attributes(&pAttribs);
			THROW_HR_ON_FAIL(hr);

			if (pnUserCompress != NULL)
			{
				CComPtr<IXMLDOMNode> pUserCompressNode;
				hr = pAttribs->getNamedItem(CComBSTR(L"usercompression"), &pUserCompressNode);
				THROW_HR_ON_NOTOK(hr);

				CComBSTR bsUserCompress;
				hr = pUserCompressNode->get_text(&bsUserCompress);
				THROW_HR_ON_FAIL(hr);

				*pnUserCompress = _wtoi(bsUserCompress);
			}

			if (pnSystemCompress != NULL)
			{
				CComPtr<IXMLDOMNode> pSystemCompressNode;
				hr = pAttribs->getNamedItem(CComBSTR(L"systemcompression"), &pSystemCompressNode);
				THROW_HR_ON_NOTOK(hr);

				CComBSTR bsSystemCompress;
				hr = pSystemCompressNode->get_text(&bsSystemCompress);
				THROW_HR_ON_FAIL(hr);			

				*pnSystemCompress = _wtoi(bsSystemCompress);						
			}

			if (pCustomCompressionSettings != NULL)
			{
				pCustomCompressionSettings->bEnabled = FALSE;
				pCustomCompressionSettings->mode = CompressionModeNone;
				pCustomCompressionSettings->nLevel = -1;
				pCustomCompressionSettings->nWindowBits = -1;
				pCustomCompressionSettings->nMemoryLevel = -1;

				CComPtr<IXMLDOMNode> pCompressionLevelNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONLEVEL), &pCompressionLevelNode);
				if (hr == S_OK)
				{				
					CComBSTR bsCompressionLevel;
					hr = pCompressionLevelNode->get_text(&bsCompressionLevel);
					if (hr == S_OK)
					{
						pCustomCompressionSettings->nLevel = _wtoi(bsCompressionLevel);	
					}
				}

				CComPtr<IXMLDOMNode> pCompressionWindowBitsNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONWINDOWBITS), &pCompressionWindowBitsNode);
				if (hr == S_OK)
				{
					CComBSTR bsCompressionWindowBits;
					hr = pCompressionWindowBitsNode->get_text(&bsCompressionWindowBits);
					if (hr == S_OK)
					{
						pCustomCompressionSettings->nWindowBits = _wtoi(bsCompressionWindowBits);	
					}
				}				
					
				CComPtr<IXMLDOMNode> pCompressionMemoryLevelNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONMEMORYLEVEL), &pCompressionMemoryLevelNode);
				if (hr == S_OK)
				{
					CComBSTR bsCompressionMemoryLevel;
					hr = pCompressionMemoryLevelNode->get_text(&bsCompressionMemoryLevel);
					if (hr == S_OK)
					{
						pCustomCompressionSettings->nMemoryLevel = _wtoi(bsCompressionMemoryLevel);	
					}
				}

				CComPtr<IXMLDOMNode> pCompressionModeNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONMODE), &pCompressionModeNode);
				if (hr == S_OK)
				{				
					CComBSTR bsCompressionMode;
					hr = pCompressionModeNode->get_text(&bsCompressionMode);
					if (hr == S_OK)
					{
						pCustomCompressionSettings->mode = (CompressionMode) _wtoi(bsCompressionMode);
					}
				}				

				CComPtr<IXMLDOMNode> pCompressionEnabledNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONENABLED), &pCompressionEnabledNode);
				if (hr == S_OK)
				{				
					CComBSTR bsCompressionEnabled;
					hr = pCompressionEnabledNode->get_text(&bsCompressionEnabled);
					if (hr == S_OK)
					{
						pCustomCompressionSettings->bEnabled = (_wtoi(bsCompressionEnabled) > 0 ? TRUE : FALSE);
					}
				}				
			}
		}
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP IStaticFileRulesImpl::SetFileRule(LPCSTR pszFullFilePath, int nUserCompress, int nSystemCompress)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);		

		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNode> pFileNode;
		hr = GetFileNode(pszFullFilePath, &pFileNode);
		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{
			CComPtr<IXMLDOMNamedNodeMap> pAttribs;
			hr = pFileNode->get_attributes(&pAttribs);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNode> pUserCompressNode;
			hr = pAttribs->getNamedItem(CComBSTR(L"usercompression"), &pUserCompressNode);
			THROW_HR_ON_FAIL(hr);

			CComBSTR bsUserCompress;
			StateToCompressionString(nUserCompress, bsUserCompress);

			hr = pUserCompressNode->put_text(bsUserCompress);
			THROW_HR_ON_FAIL(hr);

			CComPtr<IXMLDOMNode> pSystemCompressNode;
			hr = pAttribs->getNamedItem(CComBSTR(L"systemcompression"), &pSystemCompressNode);
			THROW_HR_ON_FAIL(hr);

			CComBSTR bsSystemCompress;
			StateToCompressionString(nSystemCompress, bsSystemCompress);			

			hr = pSystemCompressNode->put_text(bsSystemCompress);
			THROW_HR_ON_FAIL(hr);

			hr = Save();
			THROW_HR_ON_FAIL(hr);
		}
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

STDMETHODIMP IStaticFileRulesImpl::EnumByPath(LPCSTR pszPath, IEnumStaticFileInfo** ppEnum)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);
		THROW_HR_ON_NULL(pszPath, hr, E_POINTER);
		THROW_HR_ON_NULL(ppEnum, hr, E_POINTER);

		CPathT<CStringA> Path(pszPath);
		Path.RemoveBackslash();

		CStringA sPath(Path);
		sPath.Replace("\\", "\\\\");

		CString sQuery;
		sQuery.Format(_T("/files/file[path='%hs']"), sPath);
		
		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNodeList> pFileNodes;
		hr = m_pDOM->selectNodes(CComBSTR(sQuery), &pFileNodes);
		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{
			long nFiles = 0;
			pFileNodes->get_length(&nFiles);

			CComObject<CEnumStaticFileInfo>* pEnumStaticFileInfoObject;
			hr = CComObject<CEnumStaticFileInfo>::CreateInstance(&pEnumStaticFileInfoObject);
			THROW_HR_ON_FAIL(hr);

			for (int i = 0; i < nFiles; i++)
			{
				CComPtr<IXMLDOMNode> pFileNode;
				pFileNodes->get_item(i, &pFileNode);

				if (pFileNode == NULL)
					continue;

				CComPtr<IXMLDOMNode> pNameNode;
				hr = pFileNode->selectSingleNode(CComBSTR(L"name"), &pNameNode);
				THROW_HR_ON_FAIL(hr);

				CComBSTR bsName;
				hr = pNameNode->get_text(&bsName);
				THROW_HR_ON_FAIL(hr);								

				CComPtr<IXMLDOMNamedNodeMap> pAttribs;
				hr = pFileNode->get_attributes(&pAttribs);
				THROW_HR_ON_FAIL(hr);

				CComPtr<IXMLDOMNode> pUserCompressionAttrib;
				hr = pAttribs->getNamedItem(CComBSTR(L"usercompression"), &pUserCompressionAttrib);
				THROW_HR_ON_FAIL(hr);

				CComBSTR bsUserCompress;
				hr = pUserCompressionAttrib->get_text(&bsUserCompress);
				THROW_HR_ON_FAIL(hr);

				int nUserCompress = _wtoi(bsUserCompress);

				CComPtr<IXMLDOMNode> pSystemCompressionAttrib;
				hr = pAttribs->getNamedItem(CComBSTR(L"systemcompression"), &pSystemCompressionAttrib);
				THROW_HR_ON_FAIL(hr);

				CComBSTR bsSystemCompress;
				hr = pSystemCompressionAttrib->get_text(&bsSystemCompress);
				THROW_HR_ON_FAIL(hr);

				BOOL bSystemCompress = FALSE;
				int nSystemCompress = _wtoi(bsSystemCompress);
				if (nSystemCompress > 0)
				{
					bSystemCompress = TRUE;
				}				

				hr = pEnumStaticFileInfoObject->AddElement(CComBSTR(pszPath), bsName, nUserCompress, bSystemCompress);
				THROW_HR_ON_FAIL(hr);
			}

			CComQIPtr<IEnumStaticFileInfo> pEnum = pEnumStaticFileInfoObject;
			*ppEnum = pEnum.Detach();
		}
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

HRESULT IStaticFileRulesImpl::Load(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pConfigStorage, hr, E_FAIL);
		THROW_HR_ON_NULL(m_pDOM, hr, E_FAIL);

		CComPtr<IStream> pStream;
		hr = m_pConfigStorage->OpenStream(CComBSTR(IISXPRESS_STATICFILERULES), NULL, 
			STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, &pStream);

		if (hr != S_OK)
		{
			hr = m_pConfigStorage->CreateStream(CComBSTR(IISXPRESS_STATICFILERULES), 
				STGM_CREATE | STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, NULL, &pStream);

			// if the stream has been created then commit the parent storage item to make sure
			// it won't be lost if there is an unusual termination
			if (pStream != NULL)
			{	
				m_pConfigStorage->Commit(STGC_DEFAULT);
			}
		}

		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{		
			STATSTG Stat;
			pStream->Stat(&Stat, STATFLAG_NONAME);

			// only attempt to parse if the stream has some data in it
			if (Stat.cbSize.LowPart > 0)
			{
				VARIANT_BOOL vbSuccess;
				hr = m_pDOM->load(CComVariant(pStream), &vbSuccess);
				THROW_HR_ON_FAIL(hr);					

				// if the stream failed to validate as XML we need to dump the current DOM and
				// create a new one
				if (hr != S_OK)
				{
					m_pDOM.Release();

					hr = m_pDOM.CoCreateInstance(L"MSXML.DOMDocument");
					THROW_HR_ON_FAIL(hr);
					hr = S_FALSE;
				}
			}

			// if the stream was zero length or failed to parse then we need to make sure we
			// have a processing instruction in the document otherwise it won't save later (yes
			// really!)
			if (Stat.cbSize.LowPart == 0 || hr != S_OK)
			{
				CComPtr<IXMLDOMNode> pProcessingInstruction;
				hr = m_pDOM->createNode(CComVariant(NODE_PROCESSING_INSTRUCTION), CComBSTR(L"xml"), CComBSTR(), &pProcessingInstruction);			
				THROW_HR_ON_FAIL(hr);

				hr = m_pDOM->appendChild(pProcessingInstruction, NULL);
				THROW_HR_ON_FAIL(hr);
			}			

			// make sure there is a 'files' node
			CComPtr<IXMLDOMNode> pFilesNode;
			m_pDOM->selectSingleNode(CComBSTR(L"/files"), &pFilesNode);
			if (pFilesNode == NULL)
			{
				hr = m_pDOM->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"files"), CComBSTR(), &pFilesNode);
				THROW_HR_ON_FAIL(hr);

				hr = m_pDOM->appendChild(pFilesNode, NULL);
				THROW_HR_ON_FAIL(hr);				
			}
		}			
	}
	catch (HRESULT)
	{
	}

	return hr;
}

HRESULT IStaticFileRulesImpl::Save(void)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(m_pDOM, hr, E_POINTER);		
		THROW_HR_ON_NULL(m_pConfigStorage, hr, E_POINTER);

		CComPtr<IStream> pStream;
		hr = m_pConfigStorage->OpenStream(CComBSTR(IISXPRESS_STATICFILERULES), NULL, 
			STGM_WRITE | STGM_SHARE_EXCLUSIVE, NULL, &pStream);
		THROW_HR_ON_FAIL(hr);

		ULARGE_INTEGER nNewSize = { 0, 0 };
		pStream->SetSize(nNewSize);

		hr = m_pDOM->save(CComVariant(pStream));		
		THROW_HR_ON_FAIL(hr);

		// do a lazy commit
		hr = pStream->Commit(STGC_DANGEROUSLYCOMMITMERELYTODISKCACHE);
	}
	catch (HRESULT)
	{
	}

	return hr;
}

bool IStaticFileRulesImpl::StateToCompressionString(int nState, CComBSTR& bsState)
{
	if (nState < 0)
	{
		bsState = L"-1";
	}
	else if (nState == 0)
	{
		bsState = L"0";
	}
	else
	{
		bsState = L"1";
	}

	return true;
}

bool IStaticFileRulesImpl::CompressionStateToString(const CComBSTR& bsState, int* pnState)
{
	if (pnState == NULL || bsState.Length() < 1)
		return false;

	if (bsState[0] == '0')
	{
		*pnState = 0;
	}
	else if (bsState == '-')
	{
		*pnState = -1;
	}
	else
	{
		*pnState = 1;
	}

	return true;
}

HRESULT IStaticFileRulesImpl::GetFileCompressionParams(LPCSTR pszFullFilePath, CustomCompressionSettings* pCustomCompressionSettings)
{
	if (pCustomCompressionSettings == NULL)
	{
		return E_POINTER;
	}

	return GetFileRule(pszFullFilePath, NULL, NULL, pCustomCompressionSettings);
}

HRESULT IStaticFileRulesImpl::SetFileCompressionParams(LPCSTR pszFullFilePath, CustomCompressionSettings* pCustomCompressionSettings)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);		
		THROW_HR_ON_NULL(pCustomCompressionSettings, hr, E_POINTER);		

		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNode> pFileNode;
		hr = EnsureFileNode(pszFullFilePath, &pFileNode);
		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{
			CComPtr<IXMLDOMNamedNodeMap> pAttribs;
			hr = pFileNode->get_attributes(&pAttribs);
			THROW_HR_ON_FAIL(hr);

			{
				CString sCompressionEnabled;
				sCompressionEnabled.Format(_T("%d"), pCustomCompressionSettings->bEnabled);

				CComPtr<IXMLDOMNode> pCompressionEnabledNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONENABLED), &pCompressionEnabledNode);
				if (hr != S_OK)
				{
					pCompressionEnabledNode.Release();

					CComPtr<IXMLDOMNode> pTemp;
					hr = m_pDOM->createNode(CComVariant(NODE_ATTRIBUTE), CComBSTR(CUSTOMCOMPRESSIONENABLED), CComBSTR(), &pTemp);
					THROW_HR_ON_FAIL(hr);
					
					hr = pAttribs->setNamedItem(pTemp, &pCompressionEnabledNode);
					THROW_HR_ON_FAIL(hr);
				}
				
				hr = pCompressionEnabledNode->put_text(CComBSTR(sCompressionEnabled));
				THROW_HR_ON_FAIL(hr);
			}

			{
				CString sCompressionMode;
				sCompressionMode.Format(_T("%d"), (int) pCustomCompressionSettings->mode);

				CComPtr<IXMLDOMNode> pCompressionModeNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONMODE), &pCompressionModeNode);
				if (hr != S_OK)
				{
					pCompressionModeNode.Release();

					CComPtr<IXMLDOMNode> pTemp;
					hr = m_pDOM->createNode(CComVariant(NODE_ATTRIBUTE), CComBSTR(CUSTOMCOMPRESSIONMODE), CComBSTR(), &pTemp);					
					THROW_HR_ON_FAIL(hr);
					
					hr = pAttribs->setNamedItem(pTemp, &pCompressionModeNode);
					THROW_HR_ON_FAIL(hr);
				}
				
				hr = pCompressionModeNode->put_text(CComBSTR(sCompressionMode));
				THROW_HR_ON_FAIL(hr);			
			}

			{
				CString sCompressionLevel;
				sCompressionLevel.Format(_T("%d"), pCustomCompressionSettings->nLevel);

				CComPtr<IXMLDOMNode> pCompressionLevelNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONLEVEL), &pCompressionLevelNode);
				if (hr != S_OK)
				{
					pCompressionLevelNode.Release();

					CComPtr<IXMLDOMNode> pTemp;
					hr = m_pDOM->createNode(CComVariant(NODE_ATTRIBUTE), CComBSTR(CUSTOMCOMPRESSIONLEVEL), CComBSTR(), &pTemp);					
					THROW_HR_ON_FAIL(hr);
					
					hr = pAttribs->setNamedItem(pTemp, &pCompressionLevelNode);
					THROW_HR_ON_FAIL(hr);
				}	

				hr = pCompressionLevelNode->put_text(CComBSTR(sCompressionLevel));
				THROW_HR_ON_FAIL(hr);
			}

			{
				CString sCompressionWindowBits;
				sCompressionWindowBits.Format(_T("%d"), pCustomCompressionSettings->nWindowBits);

				CComPtr<IXMLDOMNode> pCompressionWindowBitsNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONWINDOWBITS), &pCompressionWindowBitsNode);
				if (hr != S_OK)
				{
					pCompressionWindowBitsNode.Release();

					CComPtr<IXMLDOMNode> pTemp;
					hr = m_pDOM->createNode(CComVariant(NODE_ATTRIBUTE), CComBSTR(CUSTOMCOMPRESSIONWINDOWBITS), CComBSTR(), &pTemp);
					THROW_HR_ON_FAIL(hr);
					
					hr = pAttribs->setNamedItem(pTemp, &pCompressionWindowBitsNode);
					THROW_HR_ON_FAIL(hr);
				}	

				hr = pCompressionWindowBitsNode->put_text(CComBSTR(sCompressionWindowBits));
				THROW_HR_ON_FAIL(hr);
			}

			{
				CString sCompressionMemoryLevel;
				sCompressionMemoryLevel.Format(_T("%d"), pCustomCompressionSettings->nMemoryLevel);

				CComPtr<IXMLDOMNode> pCompressionMemoryLevelNode;
				hr = pAttribs->getNamedItem(CComBSTR(CUSTOMCOMPRESSIONMEMORYLEVEL), &pCompressionMemoryLevelNode);
				if (hr != S_OK)
				{					
					pCompressionMemoryLevelNode.Release();

					CComPtr<IXMLDOMNode> pTemp;
					hr = m_pDOM->createNode(CComVariant(NODE_ATTRIBUTE), CComBSTR(CUSTOMCOMPRESSIONMEMORYLEVEL), CComBSTR(), &pTemp);					
					THROW_HR_ON_FAIL(hr);
					
					hr = pAttribs->setNamedItem(pTemp, &pCompressionMemoryLevelNode);
					THROW_HR_ON_FAIL(hr);
				}								

				hr = pCompressionMemoryLevelNode->put_text(CComBSTR(sCompressionMemoryLevel));
				THROW_HR_ON_FAIL(hr);
			}

			hr = Save();
			THROW_HR_ON_FAIL(hr);
		}
	}
	catch (HRESULT)
	{
	}	

	return hr;
}

HRESULT IStaticFileRulesImpl::RemoveFileCompressionParams(LPCSTR pszFullFilePath)
{
	HRESULT hr = E_FAIL;

	try
	{
		THROW_HR_ON_NULL(pszFullFilePath, hr, E_POINTER);

		CComCritSecLock<CComAutoCriticalSection> lock(m_csDOM);

		CComPtr<IXMLDOMNode> pFileNode;
		hr = GetFileNode(pszFullFilePath, &pFileNode);
		THROW_HR_ON_FAIL(hr);

		if (hr == S_OK)
		{
			CComPtr<IXMLDOMNamedNodeMap> pAttribs;
			hr = pFileNode->get_attributes(&pAttribs);
			THROW_HR_ON_FAIL(hr);

			pAttribs->removeNamedItem(CComBSTR(CUSTOMCOMPRESSIONENABLED), NULL);
			pAttribs->removeNamedItem(CComBSTR(CUSTOMCOMPRESSIONMODE), NULL);
			pAttribs->removeNamedItem(CComBSTR(CUSTOMCOMPRESSIONLEVEL), NULL);
			pAttribs->removeNamedItem(CComBSTR(CUSTOMCOMPRESSIONWINDOWBITS), NULL);
			pAttribs->removeNamedItem(CComBSTR(CUSTOMCOMPRESSIONMEMORYLEVEL), NULL);
		}
	}
	catch (HRESULT)
	{		
	}	

	return hr;
}