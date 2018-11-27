
// ICTMetaDataServerTestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ICTMetaDataServerTest.h"
#include "ICTMetaDataServerTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinThread* pThread = NULL;
static volatile bool isThreadRunng;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
//UINT CameraThreadProc(LPVOID Param);
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CICTMetaDataServerTestDlg 대화 상자

CICTMetaDataServerTestDlg::CICTMetaDataServerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CICTMetaDataServerTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	capture = NULL;
	m_bSnapShot = false;
	//m_imgInput = NULL;
}

void CICTMetaDataServerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_LogEdit);
	DDX_Control(pDX, IDC_CAM1, m_BitmapCam1);
	DDX_Control(pDX, IDC_CAM2, m_BitmapCam2);
	DDX_Control(pDX, IDC_CAM3, m_BitmapCam3);
	DDX_Control(pDX, IDC_CAM4, m_BitmapCam4);
	DDX_Control(pDX, IDC_CAM5, m_BitmapCam5);
	DDX_Control(pDX, IDC_CAM6, m_BitmapCam6);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_ctrlObject);
}

BEGIN_MESSAGE_MAP(CICTMetaDataServerTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATE_CONNECTION, OnUpdateConnection)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CICTMetaDataServerTestDlg 메시지 처리기

BOOL CICTMetaDataServerTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CRect tmpRect;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &tmpRect, 0);
	MoveWindow(0, 0, tmpRect.Width(), tmpRect.Height());

	PickNextAvailable();
	StartServer();

	m_ctrlObject.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 40);
	m_ctrlObject.InsertColumn(1, _T("Type"), LVCFMT_CENTER, 80);
	m_ctrlObject.InsertColumn(2, _T("Reverse"), LVCFMT_CENTER, 50);

	InitUI();

	//m_strIPCamera = "rtsp://172.16.15.18/stream2";
	//AfxBeginThread(CameraThreadProc, this);
	//StartRTSPVideoCapTest();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CICTMetaDataServerTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CICTMetaDataServerTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CICTMetaDataServerTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool CICTMetaDataServerTestDlg::StartServer()
{
	bool bSuccess = false;

	if (m_pCurWaitServer != NULL)
	{
		m_pCurWaitServer->SetSmartAddressing(false);
		bSuccess = m_pCurWaitServer->CreateSocket("12001", AF_INET, SOCK_STREAM, 0); // TCP

		if (bSuccess && m_pCurWaitServer->WatchComm())
		{
			CString strServer, strAddr, strDestPort;
			m_pCurWaitServer->GetLocalName(strServer.GetBuffer(256), 256);
			strServer.ReleaseBuffer();
			m_pCurWaitServer->GetLocalAddress(strAddr.GetBuffer(256), 256);
			strAddr.ReleaseBuffer();
			strDestPort = "12001";
			CString strMsg = _T("Server: ") + strServer;
			strMsg += _T(", @Address: ") + strAddr;
			strMsg += _T(" is running on port ") + strDestPort + CString("\r\n");
			m_pCurWaitServer->AppendMessage(strMsg);
		}
	}

	return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////
// PickNextAvailable : this is useful only for TCP socket
void CICTMetaDataServerTestDlg::PickNextAvailable()
{
	m_pCurWaitServer = NULL;

	CSocketManager *pSocketManager = new CSocketManager(this);

	pSocketManager->SetMessageWindow(&m_LogEdit);
	pSocketManager->SetServerState(true);	// run as server

	m_SocketManagerGroup.push_back(pSocketManager);

	m_pCurWaitServer = pSocketManager;
	TRACE(_T(" ==== m_SocketManagerGroup Count [%d] ===== \n"), m_SocketManagerGroup.size());
}

///////////////////////////////////////////////////////////////////////////////
// OnUpdateConnection
// This message is sent by server manager to indicate connection status
LRESULT CICTMetaDataServerTestDlg::OnUpdateConnection(WPARAM wParam, LPARAM lParam)
{
	UINT uEvent = (UINT)wParam;
	CSocketManager* pManager = reinterpret_cast<CSocketManager*>(lParam);

	if (pManager != NULL) {
		// Server socket is now connected, we need to pick a new one
		if (uEvent == EVT_CONSUCCESS) {
			//wait new connection
			PickNextAvailable();
			StartServer();
		}
		else if (uEvent == EVT_CONFAILURE || uEvent == EVT_CONDROP) {
			CAutoLock lock(m_SocketDeleteLock);
			pManager->StopComm();
			//Find Manager and delete and remove
			std::list<CSocketManager*>::iterator it;
			for (it = m_SocketManagerGroup.begin(); it != m_SocketManagerGroup.end(); it++) {
				if (pManager == *it) {
					delete pManager;
					break;
				}
			}

			if (it != m_SocketManagerGroup.end()) {
				m_SocketManagerGroup.remove(*it);
			}

			//wait new connection
			if (m_pCurWaitServer == NULL)
			{
				PickNextAvailable();
				StartServer();
			}
		}
		else if (EVT_RECV_FIRSTDATA == uEvent) {
			
		}
		else if (uEvent == EVT_RECV_DIALOG) {
			
		}
	}

	return 1L;
}

void CICTMetaDataServerTestDlg::InitUI()
{
	GetClientRect(&m_DLGRect);

	int x, y, width, height;
	int CamWidth = (m_DLGRect.Width() - 20) / 5, CamHeight = (m_DLGRect.Height()-20) / 4;
	x = 10, y = 10, width = x + CamWidth * 2, height = y + CamHeight * 2;
	m_CamRect[0] = CRect(x, y, width, height);
	m_BitmapCam1.MoveWindow(m_CamRect[0], true);
	x = 10 + CamWidth * 2, y = 10, width = x + CamWidth, height = y + CamHeight;
	m_CamRect[1] = CRect(x, y, width, height);
	m_BitmapCam2.MoveWindow(m_CamRect[1], true);
	x = 10 + CamWidth * 2, y = 10 + CamHeight, width = x + CamWidth, height = y + CamHeight;
	m_CamRect[2] = CRect(x, y, width, height);
	m_BitmapCam3.MoveWindow(m_CamRect[2], true);
	x = 10, y = 10 + CamHeight * 2, width = x + CamWidth, height = y + CamHeight;
	m_CamRect[3] = CRect(x, y, width, height);
	m_BitmapCam4.MoveWindow(m_CamRect[3], true);
	x = 10 + CamWidth, y = 10 + CamHeight * 2, width = x + CamWidth, height = y + CamHeight;
	m_CamRect[4] = CRect(x, y, width, height);
	m_BitmapCam5.MoveWindow(m_CamRect[4], true);
	x = 10 + CamWidth * 2, y = 10 + CamHeight * 2, width = x + CamWidth, height = y + CamHeight;
	m_CamRect[5] = CRect(x, y, width, height);
	m_BitmapCam6.MoveWindow(m_CamRect[5], true);
	x = 10 + CamWidth * 3, y = 10, width = x + CamWidth * 2, height = y + CamHeight * 2;
	m_SnapShotRect = CRect(x, y, width, height);
	x = 10 + CamWidth * 3, y = 10 + CamHeight * 2, width = x + CamWidth * 2, height = y + CamHeight;
	m_ctrlObject.MoveWindow(CRect(x, y, width, height));

	m_MetaDataListRect = CRect(10, 15 + CamHeight * 3, 10 + CamWidth * 5, 15 + CamHeight * 4);
	m_LogEdit.MoveWindow(m_MetaDataListRect);

	Invalidate();
}


void CICTMetaDataServerTestDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (GetDlgItem(IDC_CAM1) != NULL)
		InitUI();
}

void CICTMetaDataServerTestDlg::displayIplImage(IplImage* pimageIpl, CRect vImageRect, ICT_META_DATA ICTMetaData)
{
	try
	{
		if (capture != NULL)
		{
			if (pimageIpl == NULL)
			{
				cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, ICTMetaData.FrameNumber);
				pimageIpl = cvQueryFrame(capture);
			}

			if (pimageIpl != NULL) {
				IplImage * TmpImg = cvCloneImage(pimageIpl);
				CClientDC dc(this);
				CvvImage m_SrcView;
				CRect ImgSize;
				ImgSize.top = vImageRect.top;
				ImgSize.left = vImageRect.left;
				ImgSize.bottom = vImageRect.top + TmpImg->height;
				ImgSize.right = vImageRect.left + TmpImg->width;
				_CMemDC memDC_Img(&dc, &vImageRect, &ImgSize);

				// Text font 설정
				CvFont font;
				cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1, 1, 0, 2.5);
				CString str;
				if (ICTMetaData.NumberOfObject > 0)
				{
					for (int i = 0; i < ICTMetaData.NumberOfObject; i++)
					{
						cvRectangle(TmpImg, cvPoint(ICTMetaData.pObjectData[i].x, ICTMetaData.pObjectData[i].y),
							cvPoint(ICTMetaData.pObjectData[i].x + ICTMetaData.pObjectData[i].Width, ICTMetaData.pObjectData[i].y + ICTMetaData.pObjectData[i].Height), CV_RGB(0, 255, 0), 2, 8, 0);

						str.Format("%d", ICTMetaData.pObjectData[i].ObjectID);
						cvPutText(TmpImg, str, cvPoint(ICTMetaData.pObjectData[i].x, ICTMetaData.pObjectData[i].y - 1), &font, CV_RGB(0, 0, 255));
					}
				}

				m_SrcView.CopyOf(TmpImg, 1);
				m_SrcView.Show(memDC_Img.GetSafeHdc(), vImageRect.left, vImageRect.top, vImageRect.left + TmpImg->width, vImageRect.top + TmpImg->height);
				cvReleaseImage(&TmpImg);

				if (m_bSnapShot)
				{
					CvvImage m_SrcView;
					_CMemDC memDC_Img(&dc, &vImageRect, &ImgSize);
					m_SrcView.CopyOf(pimageIpl, 1);
					m_SrcView.Show(memDC_Img.GetSafeHdc(), vImageRect.left, vImageRect.top, vImageRect.left + pimageIpl->width, vImageRect.top + pimageIpl->height);
					dc.StretchBlt(m_SnapShotRect.left, m_SnapShotRect.top, m_SnapShotRect.Width(), m_SnapShotRect.Height(), &memDC_Img,
						ICTMetaData.pObjectData[0].x, ICTMetaData.pObjectData[0].y, ICTMetaData.pObjectData[0].Width, ICTMetaData.pObjectData[0].Height, SRCCOPY);
					m_bSnapShot = false;
				}
			}
		}
	}
	catch (CException *e)
	{

	}
}


void CICTMetaDataServerTestDlg::ListUpdate(ICT_META_DATA ICTMetaData)
{
	if (ICTMetaData.NumberOfObject>0)
	{
		int n = ICTMetaData.NumberOfObject;

		for (int i = 0; i<n; i++)
		{
			int assign = -1;

			CString str;
			str.Format("%d", ICTMetaData.pObjectData[i].ObjectID);
			CString str1, str2;

			if (ICTMetaData.pObjectData[i].Type == 0)
				str1.Format("Unkown");
			else if (ICTMetaData.pObjectData[i].Type == 1)
				str1.Format("Vehicle");
			else if (ICTMetaData.pObjectData[i].Type == 2)
				str1.Format("Human");

			if (ICTMetaData.pObjectData[i].DirectionFlag == 2)
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

			if (assign == -1 && ICTMetaData.pObjectData[i].ntimes_updated > 24)
			{
				int num = m_ctrlObject.GetItemCount();
				m_ctrlObject.InsertItem(num, str);
				m_ctrlObject.SetItem(num, 1, LVIF_TEXT, str1, 0, 0, 0, NULL);
				m_ctrlObject.SetItem(num, 2, LVIF_TEXT, str2, 0, 0, 0, NULL);
				m_bSnapShot = true;
			}
		}
	}
}


void CICTMetaDataServerTestDlg::StartRTSPVideoCapTest()
{
	/*if (!m_vcap.open("rtsp://172.16.15.18/stream1")) {
		AfxMessageBox("실패");
	}*/
	capture = cvCaptureFromFile(".\\녹화_2016_10_26_09_58_01_950.avi");
	//capture = cvCaptureFromFile("rtsp://172.16.53.55:8554/0");
	if (!capture) {
		AfxMessageBox("실패");
	}
	else {
		//SetTimer(RTSP_IMAGE_CAP, 50, NULL);
	}
}


void CICTMetaDataServerTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case RTSP_IMAGE_CAP:
		//m_vcap.read(m_image);
		frame = cvQueryFrame(capture);
		//displayIplImage(frame, m_CamRect[0]);
		if (m_bSnapShot) {
			//displayIplImage(frame, m_SnapShotRect);
			m_bSnapShot = false;
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


//UINT CameraThreadProc(LPVOID Param)
//{
//	// Main dialog의 포인터 얻어옴
//	CICTMetaDataServerTestDlg *dlg = (CICTMetaDataServerTestDlg*)Param;
//	int num = 0;
//	// IP Camera setup
//	dlg->capture = cvCreateFileCapture(dlg->m_strIPCamera);
//
//	while (1) {
//		if (dlg->capture != NULL)
//			break;
//
//		dlg->capture = cvCreateFileCapture(dlg->m_strIPCamera);
//		Sleep(100);
//	}
//	IplImage * img;
//	if (dlg->m_imgInput != NULL)
//	{
//		cvReleaseImage(&dlg->m_imgInput);
//	}
//
//	//while (1)
//	//{
//	//	//if (dlg->m_bStartFlag == false) continue;
//	//	img = cvQueryFrame(dlg->capture);
//	//	if (img == NULL) break;
//	//	if (dlg->m_imgInput == NULL)
//	//	{
//	//		int width = img->width;
//	//		int height = img->height;
//
//	//		dlg->m_imgInput = cvCloneImage(img);
//	//		dlg->m_imgData.Create(dlg->m_imgInput);
//	//	}
//	//	else
//	//	{
//	//		cvCopyImage(img, dlg->m_imgInput);
//	//	}
//	//	// 영상 정보 구조체에 영상 입력
//	//	dlg->m_imgData.Input(dlg->m_imgInput);
//	//	// 결과 영상 Display
//	//	ICT_META_DATA ICTMetaData;
//	//	dlg->displayIplImage(dlg->m_imgInput, dlg->m_CamRect[0], ICTMetaData);
//	//	//dlg->UpdateData(false);
//	//	cvWaitKey(1);
//	//}
//	//cvReleaseCapture(&dlg->capture);
//	return 0;
//}