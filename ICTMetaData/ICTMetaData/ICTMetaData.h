// ICTMetaData.h : ICTMetaData DLL�� �⺻ ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "MetaData.h"
#include <afxsock.h>


// CICTMetaDataApp
// �� Ŭ������ ������ ������ ICTMetaData.cpp�� �����Ͻʽÿ�.
//

class CICTMetaDataApp : public CWinApp
{
public:
	CICTMetaDataApp();

	CSocket m_MetaClient;
	BOOL m_firstConnect;
	BOOL m_bConnected;

	BOOL SendMetaData(ICT_META_DATA* pICTMetaData);
	BOOL ClientInit(char* vIP, int vPort);
	void ClientClose();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
//
//#ifdef DLLDEMO_EXPORT
//extern "C" __declspec(dllexport)//1
//#else
//extern "C" __declspec(dllimport)//2
//#endif
//
//bool __stdcall SendMetaData(ICT_META_DATA* pICTMetaData);
//bool __stdcall ClientInit(char* vIP, int vPort);
//
//#ifndef DLLDEMO_EXPORT
//#pragma comment(lib, "ICTMetaData")
//#endif