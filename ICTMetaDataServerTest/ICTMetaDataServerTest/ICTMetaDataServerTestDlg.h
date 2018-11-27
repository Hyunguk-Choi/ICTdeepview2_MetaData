
// ICTMetaDataServerTestDlg.h : 헤더 파일
//

#pragma once
#include <afxsock.h>
#include "afxwin.h"
#include "SocketManager.h"
#include "lock.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream> 

//#include "STR_function.h"

#include "CvvImage.h"
#include "_memdc.h"
#include "afxcmn.h"

using namespace cv;
using namespace std;

#define RTSP_IMAGE_CAP	10001
// CICTMetaDataServerTestDlg 대화 상자
class CICTMetaDataServerTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CICTMetaDataServerTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	std::list<CSocketManager*>	m_SocketManagerGroup;
	CSocketManager* m_pCurWaitServer;
	CLock	m_SocketDeleteLock;
	bool StartServer();
	void PickNextAvailable();
	void displayIplImage(IplImage* pimageIpl, CRect vImageRect, ICT_META_DATA ICTMetaData);
	void ListUpdate(ICT_META_DATA ICTMetaData);
	afx_msg LRESULT OnUpdateConnection(WPARAM wParam, LPARAM lParam);
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ICTMETADATASERVERTEST_DIALOG };
	bool m_bSnapShot;

	//UI
	CRect m_DLGRect;
	CRect m_CameraRect;
	CRect m_CamRect[5];
	CRect m_MetaDataListRect;
	CRect m_SnapShotRect;
	CRect m_StatusRect;

	//opencv
	CvCapture* capture;
	IplImage *frame;

	// IP camera
	CString				m_strIPCamera;

	// Image Data
	//IplImage*			m_imgInput;				// 입력 영상 
	//ImageData			m_imgData;				///<  영상 구조체 (영상처리에서 사용되는 모든 영상을 관리)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// //전송된 메타데이타 표시
	CEdit m_LogEdit;
	void InitUI();
	CStatic m_BitmapCam1;
	CStatic m_BitmapCam2;
	CStatic m_BitmapCam3;
	CStatic m_BitmapCam4;
	CStatic m_BitmapCam5;
	CStatic m_BitmapCam6;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void StartRTSPVideoCapTest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_ctrlObject;
};
