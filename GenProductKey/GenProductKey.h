// GenProductKey.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error �b�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'
#endif

#include "resource.h"		// �D�n�Ÿ�


// CGenProductKeyApp:
// �аѾ\��@�����O�� GenProductKey.cpp
//

class CGenProductKeyApp : public CWinApp
{
public:
	CGenProductKeyApp();

// �мg
	public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CGenProductKeyApp theApp;
