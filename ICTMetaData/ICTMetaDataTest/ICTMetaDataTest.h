
// ICTMetaDataTest.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CICTMetaDataTestApp:
// �� Ŭ������ ������ ���ؼ��� ICTMetaDataTest.cpp�� �����Ͻʽÿ�.
//

class CICTMetaDataTestApp : public CWinApp
{
public:
	CICTMetaDataTestApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CICTMetaDataTestApp theApp;