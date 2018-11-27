// SocketManager.cpp: implementation of the CSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlconv.h>
#include "ICTMetaDataServerTest.h"
#include "SocketManager.h"
#include "ICTMetaDataServerTestDlg.h"

//#pragma comment(lib, "../FFMPEGEncoder/FFMPEGEncoder")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SYSTEMTIME		CSocketManager::m_LastST;

CSocketManager::CSocketManager(CWnd* pParent /*=NULL*/)
: m_pMsgCtrl(NULL)
{
	GetLocalTime(&m_LastST);
	m_bIsFirst			= true;
	m_bConnect			= false;
	m_bConnectToTarget	= false;
	m_bCamCH2Use = false;
	m_FirstDataSize		= 0;
	m_pParent = pParent;
	m_SnapFlag = false;
}


CSocketManager::~CSocketManager()
{
	StopComm();
}


void CSocketManager::StopComm()
{
	OnSendFinish();
	CSocketComm::StopComm();
}


void CSocketManager::DisplayData(const LPBYTE lpData, DWORD dwCount)
{
	CString strData;
	strData.ReleaseBuffer(dwCount);
	AppendMessage( strData );
}


//1. lock
static CLock g_MessageLock;
void CSocketManager::AppendMessage(LPCTSTR strText )
{
	if (NULL == m_pMsgCtrl)
		return;

	CAutoLock lock(g_MessageLock);
	TCHAR strTemp[254];

	SYSTEMTIME st;
	GetLocalTime(&st);

	/*_stprintf(strTemp, _T("%04d:%02d:%02d-%02d:%02d:%02d: %s"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, strText);*/
	_stprintf(strTemp, _T("%s"),
		strText);

	m_LastST = st;

	HWND hWnd = m_pMsgCtrl->GetSafeHwnd();
	DWORD dwResult = 0;
	if (SendMessageTimeout(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 1000L, &dwResult) != 0)
	{
		int nLen = (int) dwResult;
		if (SendMessageTimeout(hWnd, EM_SETSEL, nLen, nLen, SMTO_NORMAL, 1000L, &dwResult) != 0)
		{
			if (SendMessageTimeout(hWnd, EM_REPLACESEL, FALSE, (LPARAM)strTemp, SMTO_NORMAL, 1000L, &dwResult) != 0)
			{
			}
		}

	}
}


void CSocketManager::OnSendFinish()
{
	if(!m_bIsFirst){
		TCHAR strTemp[254];
		_stprintf(strTemp, _T("OnSendFinish [%d]\n"), m_hComm);
		AppendMessage( strTemp );

		m_PassSender.CloseComm();
		m_bIsFirst	= true;
		m_nCount	= 0;
	}
}


void CSocketManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	LPBYTE lpData = lpBuffer;
	
	//const TCPPacket::header_tag *tag = (TCPPacket::header_tag *)lpBuffer;
	const BYTE *pBuf = (const BYTE*)lpBuffer;
	const int tot_size = dwCount;

	if (tot_size == 0){
		TRACE("===== Fail tot_size Zero ===== \n");
		return;
	}
		
	if(tot_size < 0/*|| ((int)pBuf2[1]<<8) + ((int)pBuf2[0]) != 0x0fa4*/){
		TRACE("wrong packet \n");
		return;
	}

	ICT_META_DATA pICTMetaData;
	memcpy(&pICTMetaData, lpBuffer, sizeof(ICT_META_DATA));
	if (pICTMetaData.FrameNumber > 0) {
		//CString mMsg, tempMsg;
		//tempMsg.Format("CameraNumber : %d FrameNumber : %d TechnologyType : %d NumberOfObject : %d\r\n", pICTMetaData.CameraNumber, pICTMetaData.FrameNumber, pICTMetaData.TechnologyType, pICTMetaData.NumberOfObject);
		//mMsg = tempMsg;
		//AppendMessage(mMsg);

		//for (int i = 0; i<pICTMetaData.NumberOfObject; i++) {
		//	tempMsg.Format("ObjectID : %d x : %d y : %d Width : %d Height : %d\r\n", pICTMetaData.pObjectData[i].ObjectID, pICTMetaData.pObjectData[i].x, pICTMetaData.pObjectData[i].y, pICTMetaData.pObjectData[i].Width, pICTMetaData.pObjectData[i].Height);
		//	mMsg += tempMsg;
		//}
		//AppendMessage(mMsg);
		//((CICTMetaDataServerTestDlg*)m_pParent)->displayIplImage(pICTMetaData.ImageData, ((CICTMetaDataServerTestDlg*)m_pParent)->m_CamRect[0]);
		if (((CICTMetaDataServerTestDlg*)m_pParent)->capture == NULL)
			((CICTMetaDataServerTestDlg*)m_pParent)->StartRTSPVideoCapTest();

		((CICTMetaDataServerTestDlg*)m_pParent)->ListUpdate(pICTMetaData);
		((CICTMetaDataServerTestDlg*)m_pParent)->displayIplImage(NULL, ((CICTMetaDataServerTestDlg*)m_pParent)->m_CamRect[pICTMetaData.TechnologyType], pICTMetaData);

		/*for (int i = 0; i < pICTMetaData.NumberOfObject; i++)
		{
			if (pICTMetaData.pObjectData[i].ntimes_updated > 24 && m_SnapFlag == false)
			{
				((CICTMetaDataServerTestDlg*)m_pParent)->m_bSnapShot = true;
				m_SnapFlag = true;
			}
		}*/

		/*if (pICTMetaData.NumberOfObject == 0)
		{*/
		//m_SnapFlag = false;
		//}
	}
}


ULONG	CSocketManager::GetPeerIPAddr()
{
	SockAddrIn addr;
	GetPeerName(addr);
	return addr.GetIPAddr();
}


short	CSocketManager::GetPeerPort()
{
	SockAddrIn addr;
	GetPeerName(addr);
	return addr.GetPort();
}


///////////////////////////////////////////////////////////////////////////////
// OnEvent
// Send message to parent window to indicate connection status
void CSocketManager::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (NULL == m_pMsgCtrl)
		return;

	CWnd* pParent = m_pMsgCtrl->GetParent();
	if (!::IsWindow( pParent->GetSafeHwnd()))
		return;

	SockAddrIn addr;
	GetPeerName(addr);

	TCHAR	strMessage[256];
	BYTE	IPAddr2[4];
	ULONG	IPAddr = addr.GetIPAddr();
	memcpy(IPAddr2, &IPAddr, 4);

	switch( uEvent )
	{
		case EVT_CONSUCCESS:
			OnSendFinish();
					
			m_bConnect			= true;
			m_bConnectToTarget	= false;
			m_nCount			= 0;
			_stprintf(strMessage, _T("Connection Established From %d:%d:%d:%d\r\n"), 
				IPAddr2[0], IPAddr2[1],IPAddr2[2],IPAddr2[3]);
			GetLocalTime(&m_ConnectTime);
			m_IP.Format("%d.%d.%d.%d", IPAddr2[0], IPAddr2[1],IPAddr2[2],IPAddr2[3]);

			AppendMessage(strMessage );

			break;
		case EVT_CONFAILURE:
			AppendMessage( _T("Connection Failed\r\n") );
			break;
		case EVT_CONDROP:
			_stprintf(strMessage, _T("Connection Abandonned From %d:%d:%d:%d\r\n"), 
				IPAddr2[0], IPAddr2[1],IPAddr2[2],IPAddr2[3]);
			AppendMessage( strMessage );
			OnSendFinish();
			m_bConnect			= false;
			m_bConnectToTarget	= false;

			break;
		case EVT_ZEROLENGTH:
			AppendMessage( _T("Zero Length Message\r\n") );
			break;
		default:
			TRACE("Unknown Socket event\n");
			break;
	}

	pParent->SendNotifyMessage( WM_UPDATE_CONNECTION, uEvent, (LPARAM) this);

}