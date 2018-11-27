// ICTMetaData.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//

#include "stdafx.h"
#include "ICTMetaData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: �� DLL�� MFC DLL�� ���� �������� ��ũ�Ǿ� �ִ� ���
//		MFC�� ȣ��Ǵ� �� DLL���� ���������� ��� �Լ���
//		���� �κп� AFX_MANAGE_STATE ��ũ�ΰ�
//		��� �־�� �մϴ�.
//
//		��:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �Ϲ����� �Լ� ������ ���⿡ �ɴϴ�.
//		}
//
//		�� ��ũ�δ� MFC�� ȣ���ϱ� ����
//		�� �Լ��� �ݵ�� ��� �־�� �մϴ�.
//		��, ��ũ�δ� �Լ��� ù ��° ���̾�� �ϸ� 
//		��ü ������ �����ڰ� MFC DLL��
//		ȣ���� �� �����Ƿ� ��ü ������ ����Ǳ� ����
//		���;� �մϴ�.
//
//		�ڼ��� ������
//		MFC Technical Note 33 �� 58�� �����Ͻʽÿ�.
//

// CICTMetaDataApp

BEGIN_MESSAGE_MAP(CICTMetaDataApp, CWinApp)
END_MESSAGE_MAP()


// CICTMetaDataApp ����

CICTMetaDataApp::CICTMetaDataApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CICTMetaDataApp ��ü�Դϴ�.

CICTMetaDataApp theApp;


// CICTMetaDataApp �ʱ�ȭ

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