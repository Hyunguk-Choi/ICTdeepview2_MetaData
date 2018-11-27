
// ICTMetaDataTestDlg.h : 헤더 파일
//

#pragma once
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include <opencv2/video/tracking.hpp>
#include "CvvImage.h"
#include "_memdc.h"

#include "MetaData.h"

#include "STR_function.h"
#include "VS_function.h"
#include "afxcmn.h"

#include "XLEzAutomation.h" 
// CICTMetaDataTestDlg 대화 상자
class CICTMetaDataTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CICTMetaDataTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	CRect	m_rectDisp1;
	CRect	m_rectDisp2;
	CRect	m_rectDisp3;
	CRect	m_rectDisp4;

	// Image Data
	IplImage*			m_imgInput;				// 입력 영상 
	ImageData			m_imgData;				///<  영상 구조체 (영상처리에서 사용되는 모든 영상을 관리)
	// AVi play
	CvCapture*			m_aviCapture;			// 비디오 파일을 관리

	// IP camera
	CString				m_strIPCamera;

	// 파일 경로 관리
	CString				m_strFileFormat;		// 비디오 파일 확장자
	CString				m_strFolderPath;		// 비디오 파일 폴더 경로
	CString				m_strFilePath;			// 비디오 파일 전체 경로

	double				m_nFrameRate;			///< Video 파일의 frame rate 

	// Flag 
	bool				m_bStartFlag;			///< Calibration 및 DB Indexing 시작 버튼

	// Line Crossingm_bDrawLineFlag
	CPoint	m_ptLine[2];
	CPoint	m_ptDirec[2];
	CPoint	m_ptROI[2];

	DTPARAM				m_Detect_ObjectData;	// 검출 객체 모델
	KTRPARAM			m_Tracking_Data;		// 추적 객체 모델 
	KalmanParam			m_KalmanParam;

	bool	m_bCrossingFlag;

	// Threshold
	int m_nSizeX_Th;
	int m_nSizeY_Th;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ICTMETADATATEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
// 구현입니다.
protected:
	HICON m_hIcon;

	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	void DispImg(void);
	void VideoPrecssing(void);
	void ListUpdate(void);
	void StartVideo(void);
	void PlayVideo(void);
	CListCtrl m_ctrlObject;
	CEdit m_ServerIP;
	CEdit m_CamIP;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnLoad();
	//void VideoLoad();
};
