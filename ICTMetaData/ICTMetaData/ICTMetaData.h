// ICTMetaData.h : ICTMetaData DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "MetaData.h"
#include <afxsock.h>


// CICTMetaDataApp
// 이 클래스의 구현을 보려면 ICTMetaData.cpp를 참조하십시오.
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
// 재정의입니다.
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