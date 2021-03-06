#pragma once

#include "iadmw.h"
#include "iiscnfg.h"

#include <string>
#include <vector>
#include <set>

#include <atlbase.h>

#define METABASE_TIMEOUT	1000

class CIISMetaBaseData
{
	
public:

	CIISMetaBaseData(void);
	CIISMetaBaseData(const CIISMetaBaseData& Data);
	~CIISMetaBaseData(void);

	bool ReadData(IMSAdminBase* pAdminBase, const WCHAR* pszKeyPath = NULL, DWORD dwIdentifier = -1);
	bool WriteData(IMSAdminBase* pAdminBase, BYTE* pbyData = NULL, DWORD dwSize = 0);

	void GetKeyPath(std::wstring& sKeyPath);
	void SetKeyPath(LPCWSTR pszKeyPath);

	DWORD GetIdentifier(void);
	bool SetIdentifier(DWORD dwIdentifier);

	DWORD GetDataSize(void);
	const BYTE* GetData(void);
	bool SetData(const BYTE* pbyData, DWORD dwSize);

	bool GetAsDWORD(DWORD* pdwData);
	bool SetAsDWORD(DWORD dwData);

	bool GetAsString(std::wstring& sData);
	bool SetAsString(const std::wstring& sData);

	bool GetAsStringVector(std::vector<std::wstring>& saData);
	bool SetAsStringVector(const std::vector<std::wstring>& saData);

	DWORD GetAttributes(void);
	bool SetAttributes(DWORD);

	DWORD GetUserType(void);
	bool SetUserType(DWORD);

	DWORD GetDataType(void);
	bool SetDataType(DWORD);

	void SetFromRecord(const METADATA_RECORD& Record, const std::wstring& sKeyPath);

private:

	METADATA_RECORD		m_Data;
	std::wstring		m_sKeyPath;

};

class CIISMetaBase
{

public:

	CIISMetaBase(void);
	CIISMetaBase(IMSAdminBase* pAdminBase);

	~CIISMetaBase(void);

	bool Init(void);
	void Release(void);

	bool EnumKeys(LPCTSTR pszPath, std::vector<std::wstring>& saKeys);
	bool EnumKeys(LPCTSTR pszPath, std::set<std::wstring>& Keys);

	bool EnumData(LPCTSTR pszPath, std::vector<CIISMetaBaseData>& EnumData);

	bool EnumIdentifiers(LPCTSTR pszPath, std::set<DWORD>& Identifiers);

	bool GetAdminBase(IMSAdminBase** pAdminBase);

private:

	CComPtr<IMSAdminBase>	m_pAdminBase;
};