
// ICTMetaDataTestDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ICTMetaDataTest.h"
#include "ICTMetaDataTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UM_UPDATE WM_USER
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
UINT CameraThreadProc(LPVOID Param);
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CICTMetaDataTestDlg ��ȭ ����



CICTMetaDataTestDlg::CICTMetaDataTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CICTMetaDataTestDlg::IDD, pParent)
	, m_nSizeX_Th(8)
	, m_nSizeY_Th(15)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bStartFlag = false;
	m_imgInput = NULL;
	m_aviCapture = NULL;

	m_bCrossingFlag = false;
	m_ptLine[0].x = -1;
	m_ptLine[0].y = -1;
	m_ptLine[1].x = -1;
	m_ptLine[1].y = -1;

	m_ptROI[0].x = -1;
	m_ptROI[0].y = -1;
	m_ptROI[1].x = -1;
	m_ptROI[1].y = -1;
}

void CICTMetaDataTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SERVERIP, m_ServerIP);
	DDX_Control(pDX, IDC_EDIT_CAMIP, m_CamIP);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_ctrlObject);
}

BEGIN_MESSAGE_MAP(CICTMetaDataTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CICTMetaDataTestDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_START, &CICTMetaDataTestDlg::OnBnClickedBtnStart)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LOAD, &CICTMetaDataTestDlg::OnBnClickedBtnLoad)
	ON_MESSAGE(UM_UPDATE, OnUpdateData)
END_MESSAGE_MAP()


// CICTMetaDataTestDlg �޽��� ó����
LRESULT CICTMetaDataTestDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);

	return 0;
}

BOOL CICTMetaDataTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ServerIP.SetWindowTextA("172.16.87.1");
	//m_CamIP.SetWindowTextA(".\\19991202 210158.avi");
	m_CamIP.SetWindowTextA("rtsp://172.16.15.18/stream2");

	GetDlgItem(IDC_STATIC_DISP1)->GetWindowRect(m_rectDisp1);
	ScreenToClient(m_rectDisp1);
	GetDlgItem(IDC_STATIC_DISP2)->GetWindowRect(m_rectDisp2);
	ScreenToClient(m_rectDisp2);
	GetDlgItem(IDC_STATIC_DISP3)->GetWindowRect(m_rectDisp3);
	ScreenToClient(m_rectDisp3);
	GetDlgItem(IDC_STATIC_DISP4)->GetWindowRect(m_rectDisp4);
	ScreenToClient(m_rectDisp4);

	m_ctrlObject.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 40);
	m_ctrlObject.InsertColumn(1, _T("Type"), LVCFMT_CENTER, 80);
	m_ctrlObject.InsertColumn(2, _T("Reverse"), LVCFMT_CENTER, 50);
	m_CamIP.GetWindowTextA(m_strIPCamera);

	CString IP;
	m_ServerIP.GetWindowTextA(IP);
	if (ClientInit(IP.GetBuffer(), 12001)) {
		//AfxMessageBox("����");
	}
	else {
		AfxMessageBox("����");
	}

	//StartVideo();
	/*AfxBeginThread(CameraThreadProc, this);
	m_Tracking_Data.Create(100);*/
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CICTMetaDataTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CICTMetaDataTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		DispImg();
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CICTMetaDataTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CICTMetaDataTestDlg::StartVideo()
{
	UpdateData(true);
	CFileDialog dlgfile(TRUE);
	CString FilePath, FileName;
	int PathLength = 0;

	if (dlgfile.DoModal() == IDOK)	//���� ��ư�� ������ â �ݾ����� ���ǹ� �ȿ� �ڵ�� ����
	{
		// ���ϰ�� ��������
		FilePath = dlgfile.GetPathName();
		// ������ ������ Ȯ���� 
		m_strFileFormat = FilePath.Right(3);
		char* FullFileName = (char*)((LPCSTR)FilePath);
		m_Tracking_Data.Create(100);
		/////////////////////////////////////////////////////////////////////////////
		// avi/mov ���� �ε�
		/////////////////////////////////////////////////////////////////////////////	
		if (!m_strFileFormat.Compare("avi") || !m_strFileFormat.Compare("mov"))
		{
			m_aviCapture = cvCaptureFromAVI(FilePath.GetBuffer(FilePath.GetLength() + 1));

			if (m_aviCapture != NULL)
				cvReleaseCapture(&m_aviCapture);
			m_aviCapture = cvCaptureFromAVI(FilePath.GetBuffer(FilePath.GetLength() + 1));
			m_nFrameRate = cvGetCaptureProperty(m_aviCapture, CV_CAP_PROP_FPS);
			if (m_aviCapture)
			{
				cvGrabFrame(m_aviCapture);
				m_imgInput = cvRetrieveFrame(m_aviCapture);
				if (m_imgInput->origin == IPL_ORIGIN_TL)
					cvCopy(m_imgInput, m_imgInput, 0);
				else
					cvFlip(m_imgInput, m_imgInput, 0);
			}
		}
		m_ctrlObject.DeleteAllItems();
		m_imgData.Create(m_imgInput); // ���� ���� ����ü �ʱ�ȭ
		m_strFilePath = FilePath;

		UpdateData(false);
	}
	Invalidate(0);
	//PlayVideo();
}


void CICTMetaDataTestDlg::PlayVideo()
{
	if (m_bStartFlag == false)
	{
		SetTimer(1, 20, NULL);
		m_bStartFlag = true;
		//AfxBeginThread(CameraThreadProc,this);
		GetDlgItem(IDC_BTN_START)->SetWindowTextA("STOP");
	}
	else
	{
		KillTimer(1);
		m_bStartFlag = false;
		GetDlgItem(IDC_BTN_START)->SetWindowTextA("PLAY");
	}
}


void CICTMetaDataTestDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString IP;
	m_ServerIP.GetWindowTextA(IP);
	if(ClientInit(IP.GetBuffer(), 12001)){
		//AfxMessageBox("����");
	}
	else{
		AfxMessageBox("����");
	}

	ICT_META_DATA mMetaData;
	mMetaData.CameraNumber = 1;
	mMetaData.FrameNumber = 1;
	mMetaData.TechnologyType = DETECT1;
	mMetaData.NumberOfObject = 2;

	//OBJECT_DATA* mObjectData;�̼��� ������ ��ų����Ŀ ~���ο��� �����ϴ� �̼��� �Ϸ�~ 6ȭ
	//mObjectData = new OBJECT_DATA[mMetaData.NumberOfObject];
	for(int i=0;i<mMetaData.NumberOfObject;i++){
		mMetaData.pObjectData[i].ObjectID = i;
		mMetaData.pObjectData[i].x = 10 * (i+1);
		mMetaData.pObjectData[i].y = 10 * (i+1);
		mMetaData.pObjectData[i].Width = 100 * (i+1);
		mMetaData.pObjectData[i].Height = 100 * (i+1);
	}

	//mMetaData.pObjectData = mObjectData;
	SendMetaData(&mMetaData);

	//CDialogEx::OnOK();
}


void CICTMetaDataTestDlg::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	ClientClose();

	CDialogEx::OnClose();
}


void CICTMetaDataTestDlg::VideoPrecssing(void)
{

	//Obj_Detection(&m_imgData, &m_Detect_ObjectData, m_nDiffer_Th, (int)(0.7*m_nSizeX_Th*m_nSizeY_Th), m_nSizeX_Th,m_nSizeY_Th);
	bool ROIflag = 0;
	/*if (IsDlgButtonChecked(IDC_CHECK_ROI) == 1)
	{
		ROIflag = 1;
	}*/

	Obj_DetectionGMM(&m_imgData, &m_Detect_ObjectData, (int)(0.7*m_nSizeX_Th*m_nSizeY_Th), m_nSizeX_Th, m_nSizeY_Th, ROIflag, m_ptROI);
	//Tr_Tracking(&m_imgData, &m_Detect_ObjectData, &m_Tracking_Data);	
	m_Detect_ObjectData.EllipseFit();

	Kalman_Prediction(&m_Tracking_Data, &m_KalmanParam);
	DataAssociation(&m_Detect_ObjectData, &m_Tracking_Data);

	//if (IsDlgButtonChecked(IDC_CHECK_VEHICLE) == 1)
	//{
	//	RecognizeVehicle(&m_imgData, &m_Tracking_Data);
	//}
	//if (IsDlgButtonChecked(IDC_CHECK_HUMAN) == 1)
	//{
	//	RecognizeHumanHaar(&m_imgData, &m_Tracking_Data);
	//}
	RecognizeVehicle(&m_imgData, &m_Tracking_Data);
	RecognizeHumanHaar(&m_imgData, &m_Tracking_Data);

	Kalman_Update(&m_Tracking_Data, &m_KalmanParam);

	Event_Crossing(&m_Tracking_Data, m_ptLine, m_bCrossingFlag);

	//BG_Update(&m_imgData,8,(float) m_nMotion_Th, 3, 0.2);

	//cvNamedWindow("a");
	//cvShowImage("a",m_imgData.ImgOverlay2);
}


void CICTMetaDataTestDlg::DispImg(void)
{
	CClientDC dc(this);

	if (m_imgInput != NULL)
	{
		ICT_META_DATA mMetaData;
		IplImage * TmpImg = cvCloneImage(m_imgData.ImgOrigin);

		CvvImage m_SrcView;
		CRect ImgSize;
		ImgSize.top = m_rectDisp1.top;
		ImgSize.left = m_rectDisp1.left;
		ImgSize.bottom = m_rectDisp1.top + m_imgData.ImgOrigin->height;
		ImgSize.right = m_rectDisp1.left + m_imgData.ImgOrigin->width;
		// memory DC ����
		_CMemDC memDC_Img(&dc, &m_rectDisp1, &ImgSize);

		CPoint ptTmp;

		m_ptDirec[0].x = (m_ptLine[0].x + m_ptLine[1].x) / 2;
		m_ptDirec[0].y = (m_ptLine[0].y + m_ptLine[1].y) / 2;

		ptTmp.x = m_ptLine[1].x - m_ptDirec[0].x;
		ptTmp.y = m_ptLine[1].y - m_ptDirec[0].y;

		float norm = sqrt((float)(ptTmp.x*ptTmp.x + ptTmp.y*ptTmp.y));
		ptTmp.x = 30 * ptTmp.x / norm;
		ptTmp.y = 30 * ptTmp.y / norm;

		m_ptDirec[1].x = ptTmp.y + m_ptDirec[0].x;
		m_ptDirec[1].y = -ptTmp.x + m_ptDirec[0].y;

		cvLine(TmpImg, cvPoint(m_ptDirec[0].x, m_ptDirec[0].y),
			cvPoint(m_ptDirec[1].x, m_ptDirec[1].y), CV_RGB(0, 0, 255), 6, 8, 0);

		cvLine(TmpImg, cvPoint(m_ptLine[0].x, m_ptLine[0].y),
			cvPoint(m_ptLine[1].x, m_ptLine[1].y), CV_RGB(255, 0, 120), 2, 8, 0);

		cvRectangle(TmpImg, cvPoint(m_ptROI[0].x, m_ptROI[0].y),
			cvPoint(m_ptROI[1].x, m_ptROI[1].y), CV_RGB(255, 0, 255), 0);

		// Text font ����
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.4, 0.4, 0, 1.5);
		CString str;

		//��Ÿ������ �Է�
		if (m_imgInput != NULL)
		{
			mMetaData.CameraNumber = 1;
			mMetaData.FrameNumber = cvGetCaptureProperty(m_aviCapture, CV_CAP_PROP_POS_FRAMES);
			mMetaData.TechnologyType = DETECT1;
		}

		if (m_Tracking_Data.Object_n>0)
		{
			for (int i = 0; i<m_Tracking_Data.Object_n; i++)
			{
				if (m_Tracking_Data.Object_Kalman[i].ntimes_propagated == 0)
				{
					cvCircle(TmpImg, cvPoint(m_Tracking_Data.Object_Kalman[i].x_t[0], m_Tracking_Data.Object_Kalman[i].x_t[1]), 2, CV_RGB(255, 0, 0), 3);

					/*	if(m_Tracking_Data.DirectionFlag[i] == 0)
					{
					cvEllipse(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_MassX[i],m_Tracking_Data.Object_MassY[i]), cvSize(m_Tracking_Data.Minor_len[i], m_Tracking_Data.Major_len[i]), m_Tracking_Data.Angle[i], 0, 360, CV_RGB(0,255,0), 1, CV_AA, 0);
					}
					else if(m_Tracking_Data.DirectionFlag[i] == 1)
					{
					cvEllipse(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_MassX[i],m_Tracking_Data.Object_MassY[i]), cvSize(m_Tracking_Data.Minor_len[i], m_Tracking_Data.Major_len[i]), m_Tracking_Data.Angle[i], 0, 360, CV_RGB(255,255,0), 1, CV_AA, 0);
					}
					else if(m_Tracking_Data.DirectionFlag[i] == 2)
					{
					cvEllipse(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_MassX[i],m_Tracking_Data.Object_MassY[i]), cvSize(m_Tracking_Data.Minor_len[i], m_Tracking_Data.Major_len[i]), m_Tracking_Data.Angle[i], 0, 360, CV_RGB(255,0,255), 1, CV_AA, 0);
					}*/
				}
				else
				{
					//cvEllipse(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_MassX[i],m_Tracking_Data.Object_MassY[i]), cvSize(m_Tracking_Data.Minor_len[i], m_Tracking_Data.Major_len[i]), m_Tracking_Data.Angle[i], 0, 360, CV_RGB(0,255,255), 1, CV_AA, 0);  
					//cvCircle(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_Kalman[i].x_t[0], m_Tracking_Data.Object_Kalman[i].x_t[1]), 2, CV_RGB(255,0,255), 3);
					continue;
				}
				if (m_Tracking_Data.Trajectory[i].x[2]<1 || m_Tracking_Data.Trajectory[i].y[2]<1)
					continue;

				cvRectangle(TmpImg, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top - 10),
					cvPoint(m_Tracking_Data.Object_info[i].left + 10, m_Tracking_Data.Object_info[i].top), CV_RGB(255, 255, 255), -1);


				if (m_Tracking_Data.Object_Type[i] == 0)
				{
					cvRectangle(TmpImg, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top),
						cvPoint(m_Tracking_Data.Object_info[i].right, m_Tracking_Data.Object_info[i].bottom), CV_RGB(0, 255, 0), 1, 8, 0);

					str.Format("U,%d", m_Tracking_Data.Object_ID[i]);
					cvPutText(TmpImg, str, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top - 1), &font, CV_RGB(0, 0, 255));

				}
				else if (m_Tracking_Data.Object_Type[i] == 1)
				{
					cvRectangle(TmpImg, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top),
						cvPoint(m_Tracking_Data.Object_info[i].right, m_Tracking_Data.Object_info[i].bottom), CV_RGB(0, 0, 255), 1, 8, 0);

					str.Format("V,%d", m_Tracking_Data.Object_ID[i]);
					cvPutText(TmpImg, str, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top - 1), &font, CV_RGB(0, 0, 255));

				}
				else if (m_Tracking_Data.Object_Type[i] == 2)
				{
					cvRectangle(TmpImg, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top),
						cvPoint(m_Tracking_Data.Object_info[i].right, m_Tracking_Data.Object_info[i].bottom), CV_RGB(255, 255, 255), 1, 8, 0);

					str.Format("H,%d", m_Tracking_Data.Object_ID[i]);
					cvPutText(TmpImg, str, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top - 1), &font, CV_RGB(0, 0, 255));

				}

				/*if(m_Tracking_Data.DirectionFlag[i] == 0)
				{
				cvEllipse(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_MassX[i],m_Tracking_Data.Object_MassY[i]), cvSize(m_Tracking_Data.Minor_len[i], m_Tracking_Data.Major_len[i]), m_Tracking_Data.Angle[i], 0, 360, CV_RGB(0,255,0), 1, CV_AA, 0);
				}
				else if(m_Tracking_Data.DirectionFlag[i] == 1)
				{
				cvEllipse(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.Object_MassX[i],m_Tracking_Data.Object_MassY[i]), cvSize(m_Tracking_Data.Minor_len[i], m_Tracking_Data.Major_len[i]), m_Tracking_Data.Angle[i], 0, 360, CV_RGB(255,255,0), 1, CV_AA, 0);
				}*/
				if (m_Tracking_Data.DirectionFlag[i] == 2)
				{
					cvRectangle(TmpImg, cvPoint(m_Tracking_Data.Object_info[i].left, m_Tracking_Data.Object_info[i].top),
						cvPoint(m_Tracking_Data.Object_info[i].right, m_Tracking_Data.Object_info[i].bottom), CV_RGB(255, 0, 0), 1, 8, 0);

				}

				/*for(int j=1; j<m_Tracking_Data.SD[i].num; j++)
				{
				cvLine(m_imgData.ImgOverlay, cvPoint(m_Tracking_Data.SD[i].xchain[j-1],m_Tracking_Data.SD[i].ychain[j-1]),
				cvPoint(m_Tracking_Data.SD[i].xchain[j], m_Tracking_Data.SD[i].ychain[j]), CV_RGB(0,255,0), 1, 8, 0);
				}*/

				for (int j = 0; j<98; j++)
				{
					if (m_Tracking_Data.Trajectory[i].x[j + 1]<1 || m_Tracking_Data.Trajectory[i].y[j + 1]<1)
						break;
					else
					{
						cvLine(TmpImg, cvPoint(m_Tracking_Data.Trajectory[i].x[j], m_Tracking_Data.Trajectory[i].y[j]),
							cvPoint(m_Tracking_Data.Trajectory[i].x[j + 1], m_Tracking_Data.Trajectory[i].y[j + 1]), CV_RGB(255, 255, 0), 1, 8, 0);
					}
				}

				//������Ʈ �Է�
				//if ((m_Tracking_Data.Object_Type[i] == 1) || (m_Tracking_Data.Object_Type[i] == 2)) {
					mMetaData.pObjectData[mMetaData.NumberOfObject].ObjectID = m_Tracking_Data.Object_ID[i];
					mMetaData.pObjectData[mMetaData.NumberOfObject].x = (m_Tracking_Data.Object_info[i].left * m_imgInput->width) / TmpImg->width;
					mMetaData.pObjectData[mMetaData.NumberOfObject].y = (m_Tracking_Data.Object_info[i].top * m_imgInput->height) / TmpImg->height;
					mMetaData.pObjectData[mMetaData.NumberOfObject].Width = ((m_Tracking_Data.Object_info[i].right - m_Tracking_Data.Object_info[i].left)  * m_imgInput->width) / TmpImg->width;
					mMetaData.pObjectData[mMetaData.NumberOfObject].Height = ((m_Tracking_Data.Object_info[i].bottom - m_Tracking_Data.Object_info[i].top) * m_imgInput->height) / TmpImg->height;
					mMetaData.pObjectData[mMetaData.NumberOfObject].Type = m_Tracking_Data.Object_Type[i];
					mMetaData.pObjectData[mMetaData.NumberOfObject].DirectionFlag = m_Tracking_Data.DirectionFlag[i];
					mMetaData.pObjectData[mMetaData.NumberOfObject].ntimes_updated = m_Tracking_Data.Object_Kalman[i].ntimes_updated;
					mMetaData.NumberOfObject++;
				//}
			}
		}

		SendMetaData(&mMetaData);
		m_SrcView.CopyOf(TmpImg, 1);
		m_SrcView.Show(memDC_Img.GetSafeHdc(), m_rectDisp1.left, m_rectDisp1.top, m_rectDisp1.left + m_imgData.ImgOrigin->width, m_rectDisp1.top + m_imgData.ImgOrigin->height);
		cvReleaseImage(&TmpImg);
	}

	if (m_imgInput != NULL)
	{
		CvvImage m_SrcView;
		CRect ImgSize;
		ImgSize.top = m_rectDisp2.top;
		ImgSize.left = m_rectDisp2.left;
		ImgSize.bottom = m_rectDisp2.top + m_imgData.ImgColorBG->height;
		ImgSize.right = m_rectDisp2.left + m_imgData.ImgColorBG->width;
		// memory DC ����
		_CMemDC memDC_Img(&dc, &m_rectDisp2, &ImgSize);

		m_SrcView.CopyOf(m_imgData.ImgOverlay2, 1);
		m_SrcView.Show(memDC_Img.GetSafeHdc(), m_rectDisp2.left, m_rectDisp2.top, m_rectDisp2.left + m_imgData.ImgColorBG->width, m_rectDisp2.top + m_imgData.ImgColorBG->height);

	}

	if (m_imgInput != NULL)
	{
		CvvImage m_SrcView;
		CRect ImgSize;
		ImgSize.top = m_rectDisp3.top;
		ImgSize.left = m_rectDisp3.left;
		ImgSize.bottom = m_rectDisp3.top + m_imgData.ImgColorFG->height;
		ImgSize.right = m_rectDisp3.left + m_imgData.ImgColorFG->width;
		// memory DC ����
		_CMemDC memDC_Img(&dc, &m_rectDisp3, &ImgSize);

		m_SrcView.CopyOf(m_imgData.ImgColorBG, 1);
		m_SrcView.Show(memDC_Img.GetSafeHdc(), m_rectDisp3.left, m_rectDisp3.top, m_rectDisp3.left + m_imgData.ImgColorFG->width, m_rectDisp3.top + m_imgData.ImgColorFG->height);

	}

	if (m_imgInput != NULL)
	{
		CvvImage m_SrcView;
		CRect ImgSize;
		ImgSize.top = m_rectDisp4.top;
		ImgSize.left = m_rectDisp4.left;
		ImgSize.bottom = m_rectDisp4.top + m_imgData.ImgOrigin->height;
		ImgSize.right = m_rectDisp4.left + m_imgData.ImgOrigin->width;
		// memory DC ����
		_CMemDC memDC_Img(&dc, &m_rectDisp4, &ImgSize);

		m_SrcView.CopyOf(m_imgData.ImgForeground, 1);
		m_SrcView.Show(memDC_Img.GetSafeHdc(), m_rectDisp4.left, m_rectDisp4.top, m_rectDisp4.left + m_imgData.ImgOrigin->width, m_rectDisp4.top + m_imgData.ImgOrigin->height);

	}
}

UINT CameraThreadProc(LPVOID Param)
{
	// Main dialog�� ������ ����
	CICTMetaDataTestDlg *dlg = (CICTMetaDataTestDlg*)Param;
	int num = 0;
	// IP Camera setup
	CvCapture *camera = cvCreateFileCapture(dlg->m_strIPCamera);
	//dlg->m_aviCapture = cvCreateFileCapture(dlg->m_strIPCamera);

	while (1) {
		if (camera != NULL)
			break;

		camera = cvCreateFileCapture(dlg->m_strIPCamera);
		Sleep(100);
	}
	IplImage * img;
	if (dlg->m_imgInput != NULL)
	{
		cvReleaseImage(&dlg->m_imgInput);
	}

	while (1)
	{
		//if (dlg->m_bStartFlag == false) continue;
		img = cvQueryFrame(camera);
		if (img == NULL) break;
		if (dlg->m_imgInput == NULL)
		{
			int width = img->width;
			int height = img->height;

			dlg->m_imgInput = cvCloneImage(img);
			dlg->m_imgData.Create(dlg->m_imgInput);
		}
		else
		{
			cvCopyImage(img, dlg->m_imgInput);
		}
		// ���� ���� ����ü�� ���� �Է�
		dlg->m_imgData.Input(dlg->m_imgInput);
		// Video Anlaysis Function
		dlg->VideoPrecssing();
		dlg->ListUpdate();
		// ��� ���� Display
		dlg->DispImg();
		//dlg->UpdateData(false);
		dlg->PostMessageA(UM_UPDATE);
		cvWaitKey(1);
	}
	cvReleaseCapture(&camera);
	return 0;
}

void CICTMetaDataTestDlg::ListUpdate(void)
{
	if (m_Tracking_Data.Object_n>0)
	{
		int n = m_Tracking_Data.Object_n;

		for (int i = 0; i<n; i++)
		{
			int assign = -1;

			CString str;
			str.Format("%d", m_Tracking_Data.Object_ID[i]);
			CString str1, str2;

			if (m_Tracking_Data.Object_Type[i] == 0)
				str1.Format("Unkown");
			else if (m_Tracking_Data.Object_Type[i] == 1)
				str1.Format("Vehicle");
			else if (m_Tracking_Data.Object_Type[i] == 2)
				str1.Format("Human");

			if (m_Tracking_Data.DirectionFlag[i] == 2)
				str2.Format("Y");
			else
				str2.Format("N");

			for (int nIndex = 0; nIndex < m_ctrlObject.GetItemCount(); nIndex++)
			{
				if (str.Compare(m_ctrlObject.GetItemText(nIndex, 0)) == 0)
				{
					assign = nIndex;
					m_ctrlObject.SetItem(nIndex, 1, LVIF_TEXT, str1, 0, 0, 0, NULL);
					m_ctrlObject.SetItem(nIndex, 2, LVIF_TEXT, str2, 0, 0, 0, NULL);
				}
			}

			if (assign == -1 && m_Tracking_Data.Object_Kalman[i].ntimes_updated > 24)
			{
				int num = m_ctrlObject.GetItemCount();
				m_ctrlObject.InsertItem(num, str);
				m_ctrlObject.SetItem(num, 1, LVIF_TEXT, str1, 0, 0, 0, NULL);
				m_ctrlObject.SetItem(num, 2, LVIF_TEXT, str2, 0, 0, 0, NULL);
			}
		}
	}
}

void CICTMetaDataTestDlg::OnBnClickedBtnStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bStartFlag == false)
	{
		SetTimer(1, 20, NULL);
		m_bStartFlag = true;
		//AfxBeginThread(CameraThreadProc,this);
		GetDlgItem(IDC_BTN_START)->SetWindowTextA("STOP");
	}
	else
	{
		KillTimer(1);
		m_bStartFlag = false;
		GetDlgItem(IDC_BTN_START)->SetWindowTextA("PLAY");
	}
}


void CICTMetaDataTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (!m_strFileFormat.Compare("avi") || !m_strFileFormat.Compare("mov") || !m_strFileFormat.Compare("mp4"))
	{
		// ���� read
		cvGrabFrame(m_aviCapture);
		m_imgInput = cvRetrieveFrame(m_aviCapture);

		// ���� ���� ���� �˻�
		if (m_imgInput == NULL)
		{
			KillTimer(1);
			m_bStartFlag = false;
			GetDlgItem(IDC_BTN_START)->SetWindowTextA("PLAY");
			//VideoLoad();
			/*cvSetCaptureProperty(m_aviCapture, CV_CAP_PROP_POS_FRAMES, 0);
			m_imgInput = cvRetrieveFrame(m_aviCapture);*/
			return;
		}

		if (m_imgInput->origin == IPL_ORIGIN_TL)
			cvCopy(m_imgInput, m_imgInput, 0);
		else
			cvFlip(m_imgInput, m_imgInput, 0);
	}
	// ���� ���� ����ü�� ���� �Է�
	m_imgData.Input(m_imgInput);
	// Video Anlaysis Function
	int FrameCount = cvGetCaptureProperty(m_aviCapture, CV_CAP_PROP_POS_FRAMES);
	if (FrameCount % 5 == 0)
	{
		VideoPrecssing();
		ListUpdate();
	}
	// ��� ���� Display
	DispImg();
	UpdateData(FALSE);
#ifdef _SAVE
	int save = 0;

	for (int i = 0; i<m_Tracking_Data.Object_n; i++)
	{
		if (m_Tracking_Data.Object_Type[i] == 1 || m_Tracking_Data.Object_Type[i] == 2)
		{
			save = 1;
		}
	}
	if (save == 1)
	{
		CString str;
		if (m_imgData.CurrentFrame >= 10000)
		{
			str.Format(".\\Results\\Frame%d.jpg", m_imgData.CurrentFrame);
		}
		else if (m_imgData.CurrentFrame >= 1000)
		{
			str.Format(".\\Results\\Frame0%d.jpg", m_imgData.CurrentFrame);
		}
		else if (m_imgData.CurrentFrame >= 100)
		{
			str.Format(".\\Results\\Frame00%d.jpg", m_imgData.CurrentFrame);
		}
		else if (m_imgData.CurrentFrame >= 10)
		{
			str.Format(".\\Results\\Frame000%d.jpg", m_imgData.CurrentFrame);
		}
		else
		{
			str.Format(".\\Results\\Frame0000%d.jpg", m_imgData.CurrentFrame);
		}
		cvSaveImage(str, m_imgData.ImgOverlay);
	}
#endif
	CDialogEx::OnTimer(nIDEvent);
}


void CICTMetaDataTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	cvReleaseImage(&m_imgInput);
}


void CICTMetaDataTestDlg::OnBnClickedBtnLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(true);
	CFileDialog dlgfile(TRUE);
	CString FilePath, FileName;
	int PathLength = 0;

	if (dlgfile.DoModal() == IDOK)	//���� ��ư�� ������ â �ݾ����� ���ǹ� �ȿ� �ڵ�� ����
	{
		// ���ϰ�� ��������
		FilePath = dlgfile.GetPathName();
		// ������ ������ Ȯ���� 
		m_strFileFormat = FilePath.Right(3);
		char* FullFileName = (char*)((LPCSTR)FilePath);
		m_Tracking_Data.Create(100);
		/////////////////////////////////////////////////////////////////////////////
		// avi/mov ���� �ε�
		/////////////////////////////////////////////////////////////////////////////	
		if (!m_strFileFormat.Compare("avi") || !m_strFileFormat.Compare("mov") || !m_strFileFormat.Compare("mp4"))
		{
			m_aviCapture = cvCaptureFromFile(FilePath.GetBuffer(FilePath.GetLength() + 1));

			if (m_aviCapture != NULL)
			{
				cvReleaseCapture(&m_aviCapture);
			}
			m_aviCapture = cvCaptureFromFile(FilePath.GetBuffer(FilePath.GetLength() + 1));
			TRACE(FilePath);
			m_nFrameRate = cvGetCaptureProperty(m_aviCapture, CV_CAP_PROP_FPS);
			if (m_aviCapture)
			{
				cvGrabFrame(m_aviCapture);
				m_imgInput = cvRetrieveFrame(m_aviCapture);
				if (m_imgInput->origin == IPL_ORIGIN_TL)
					cvCopy(m_imgInput, m_imgInput, 0);
				else
					cvFlip(m_imgInput, m_imgInput, 0);
			}
		}
		m_ctrlObject.DeleteAllItems();
		m_imgData.Create(m_imgInput); // ���� ���� ����ü �ʱ�ȭ
		m_strFilePath = FilePath;
		UpdateData(false);
	}
	Invalidate(0);
}


//void CICTMetaDataTestDlg::VideoLoad()
//{
//	UpdateData(true);
//	
//	if (!m_strFileFormat.Compare("avi") || !m_strFileFormat.Compare("mov"))
//	{
//		m_aviCapture = cvCaptureFromAVI(m_strFilePath.GetBuffer(m_strFilePath.GetLength() + 1));
//
//		if (m_aviCapture != NULL)
//			cvReleaseCapture(&m_aviCapture);
//		m_aviCapture = cvCaptureFromAVI(m_strFilePath.GetBuffer(m_strFilePath.GetLength() + 1));
//		m_nFrameRate = cvGetCaptureProperty(m_aviCapture, CV_CAP_PROP_FPS);
//		if (m_aviCapture)
//		{
//			cvGrabFrame(m_aviCapture);
//			m_imgInput = cvRetrieveFrame(m_aviCapture);
//			if (m_imgInput->origin == IPL_ORIGIN_TL)
//				cvCopy(m_imgInput, m_imgInput, 0);
//			else
//				cvFlip(m_imgInput, m_imgInput, 0);
//		}
//	}
//	m_ctrlObject.DeleteAllItems();
//	m_imgData.Create(m_imgInput); // ���� ���� ����ü �ʱ�ȭ
//
//	UpdateData(false);
//	Invalidate(0);
//	//PlayVideo();
//}
