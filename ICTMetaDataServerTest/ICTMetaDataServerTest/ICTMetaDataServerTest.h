
// ICTMetaDataServerTest.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CICTMetaDataServerTestApp:
// �� Ŭ������ ������ ���ؼ��� ICTMetaDataServerTest.cpp�� �����Ͻʽÿ�.
//

class CICTMetaDataServerTestApp : public CWinApp
{
public:
	CICTMetaDataServerTestApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CICTMetaDataServerTestApp theApp;