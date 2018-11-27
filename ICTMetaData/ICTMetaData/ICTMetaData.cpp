// ICTMetaData.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "ICTMetaData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 이 DLL이 MFC DLL에 대해 동적으로 링크되어 있는 경우
//		MFC로 호출되는 이 DLL에서 내보내지는 모든 함수의
//		시작 부분에 AFX_MANAGE_STATE 매크로가
//		들어 있어야 합니다.
//
//		예:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 일반적인 함수 본문은 여기에 옵니다.
//		}
//
//		이 매크로는 MFC로 호출하기 전에
//		각 함수에 반드시 들어 있어야 합니다.
//		즉, 매크로는 함수의 첫 번째 문이어야 하며 
//		개체 변수의 생성자가 MFC DLL로
//		호출할 수 있으므로 개체 변수가 선언되기 전에
//		나와야 합니다.
//
//		자세한 내용은
//		MFC Technical Note 33 및 58을 참조하십시오.
//

// CICTMetaDataApp

BEGIN_MESSAGE_MAP(CICTMetaDataApp, CWinApp)
END_MESSAGE_MAP()


// CICTMetaDataApp 생성

CICTMetaDataApp::CICTMetaDataApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CICTMetaDataApp 개체입니다.

CICTMetaDataApp theApp;


// CICTMetaDataApp 초기화

BOOL CICTMetaDataApp::InitInstance()
{
	CWinApp::InitInstance();
	m_firstConnect = true;
	m_bConnected = false;

	return TRUE;
}

BOOL CICTMetaDataApp::SendMetaData(ICT_META_DATA* pICTMetaData)
{
	/*CString mMsg;
	mMsg.Format("CameraNumber : %d\n\rFrameNumber : %d\n\rTechnologyType : %d\n\rNumberOfObject : %d\n\r", pICTMetaData->CameraNumber, pICTMetaData->FrameNumber, pICTMetaData->TechnologyType, pICTMetaData->NumberOfObject);
	AfxMessageBox(mMsg);

	for(int i=0;i<pICTMetaData->NumberOfObject; i++){
		mMsg.Format("ObjectID : %d\n\rx : %d\n\ry : %d\n\rWidth : %d\n\rHeight : %d\n\r", pICTMetaData->pObjectData[i].ObjectID, pICTMetaData->pObjectData[i].x, pICTMetaData->pObjectData[i].y, pICTMetaData->pObjectData[i].Width, pICTMetaData->pObjectData[i].Height);
		AfxMessageBox(mMsg);
	}*/
	if(m_bConnected){
		m_MetaClient.Send(pICTMetaData, sizeof(ICT_META_DATA));
	}

	return true;
}

BOOL CICTMetaDataApp::ClientInit(char* vIP, int vPort)
{
	if(m_firstConnect){
		AfxSocketInit(NULL);
		m_MetaClient.Create();
		m_firstConnect = false;
	}
	if(!m_bConnected){
		if(m_MetaClient.Connect(vIP, vPort)){
			m_bConnected = true;
			return TRUE;
		}
		else{
			m_bConnected = false;
			return FALSE;
		}
	}
}

void CICTMetaDataApp::ClientClose()
{
	try{
		m_MetaClient.Close();
	}catch(CException* e){
	}
}

bool __stdcall SendMetaData(ICT_META_DATA* pICTMetaData){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return theApp.SendMetaData(pICTMetaData);
}

bool PASCAL EXPORT ClientInit(char* vIP, int vPort){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return theApp.ClientInit(vIP, vPort);
}

void PASCAL EXPORT ClientClose(){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.ClientClose();
}