
// ICTMetaDataTestDlg.h : ��� ����
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
// CICTMetaDataTestDlg ��ȭ ����
class CICTMetaDataTestDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CICTMetaDataTestDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	CRect	m_rectDisp1;
	CRect	m_rectDisp2;
	CRect	m_rectDisp3;
	CRect	m_rectDisp4;

	// Image Data
	IplImage*			m_imgInput;				// �Է� ���� 
	ImageData			m_imgData;				///<  ���� ����ü (����ó������ ���Ǵ� ��� ������ ����)
	// AVi play
	CvCapture*			m_aviCapture;			// ���� ������ ����

	// IP camera
	CString				m_strIPCamera;

	// ���� ��� ����
	CString				m_strFileFormat;		// ���� ���� Ȯ����
	CString				m_strFolderPath;		// ���� ���� ���� ���
	CString				m_strFilePath;			// ���� ���� ��ü ���

	double				m_nFrameRate;			///< Video ������ frame rate 

	// Flag 
	bool				m_bStartFlag;			///< Calibration �� DB Indexing ���� ��ư

	// Line Crossingm_bDrawLineFlag
	CPoint	m_ptLine[2];
	CPoint	m_ptDirec[2];
	CPoint	m_ptROI[2];

	DTPARAM				m_Detect_ObjectData;	// ���� ��ü ��
	KTRPARAM			m_Tracking_Data;		// ���� ��ü �� 
	KalmanParam			m_KalmanParam;

	bool	m_bCrossingFlag;

	// Threshold
	int m_nSizeX_Th;
	int m_nSizeY_Th;
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ICTMETADATATEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
// �����Դϴ�.
protected:
	HICON m_hIcon;

	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);

	// ������ �޽��� �� �Լ�
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
