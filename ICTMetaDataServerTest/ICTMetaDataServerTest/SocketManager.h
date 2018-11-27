// SocketManager.h: interface for the CSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETMANAGER_H__7403BD71_338A_4531_BD91_3D7E5B505793__INCLUDED_)
#define AFX_SOCKETMANAGER_H__7403BD71_338A_4531_BD91_3D7E5B505793__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "oil.h"

#include "SocketComm.h"
#include "lock.h"

#define WM_UPDATE_CONNECTION	WM_APP+0x1234

class CSocketManager : public CSocketComm  
{
public:
	CSocketManager(CWnd* pParent /*=NULL*/);
	virtual ~CSocketManager();

	void SetMessageWindow(CEdit* pMsgCtrl){m_pMsgCtrl = pMsgCtrl;}
	void AppendMessage(LPCTSTR strText );

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	ULONG	GetPeerIPAddr();
	short	GetPeerPort();	
	void	StopComm();
	int		GetID(){return m_ID;}
	int		GetCmd(){return m_Cmd;}


	SYSTEMTIME	GetConnectionTime(){return m_ConnectTime;}
	bool	IsConnectedClient(){return m_bConnect;}

	BOOL	IsTargetConnected(){return m_bConnectToTarget;}

	TCHAR*	GetStoreFileName(){return m_strFileName;}
	CWnd* m_pParent;

protected:
	void DisplayData(const LPBYTE lpData, DWORD dwCount);
	void OnSendFinish();

	CEdit*				m_pMsgCtrl;
	CString			m_IP;
	bool				m_bCamCH2Use;
	bool		m_bIsFirst;
	int			m_ID;
	short		m_Cmd;
	int			m_nCount;
	bool		m_bConnect;
	bool		m_bConnectToTarget;
	
	CSocketComm		m_PassSender;

	SYSTEMTIME	m_ConnectTime;
	static	SYSTEMTIME	m_LastST;
	static  CLock		m_CS;

	CLock		m_SendCS;
	int			m_FirstDataSize;
	BYTE		m_FirstData[64];


	TCHAR			m_strFileName[256];

	bool		m_SnapFlag;

//	BYTE*			m_pTmpBuf;	
};


#endif // !defined(AFX_SOCKETMANAGER_H__7403BD71_338A_4531_BD91_3D7E5B505793__INCLUDED_)
