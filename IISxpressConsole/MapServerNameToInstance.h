#pragma once

bool MapServerNameToInstance(const CString& sName, CString& sInstance);
bool MapInstanceToServerName(const CString& sInstance, CString& sName);

bool MapServerNameToInstance(IXMLDOMDocument* pDoc, const CString& sName, CString& sInstance);