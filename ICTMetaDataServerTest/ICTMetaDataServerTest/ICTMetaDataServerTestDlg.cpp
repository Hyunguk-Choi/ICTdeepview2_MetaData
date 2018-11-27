
// ICTMetaDataServerTestDlg.cpp : ���� ����
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
//UINT CameraThreadProc(LPVOID Param);
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


// CICTMetaDataServerTestDlg ��ȭ ����

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


// CICTMetaDataServerTestDlg �޽��� ó����

BOOL CICTMetaDataServerTestDlg::OnInitDialog()
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
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CICTMetaDataServerTestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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

				// Text font ����
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
		AfxMessageBox("����");
	}*/
	capture = cvCaptureFromFile(".\\��ȭ_2016_10_26_09_58_01_950.avi");
	//capture = cvCaptureFromFile("rtsp://172.16.53.55:8554/0");
	if (!capture) {
		AfxMessageBox("����");
	}
	else {
		//SetTimer(RTSP_IMAGE_CAP, 50, NULL);
	}
}


void CICTMetaDataServerTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
//	// Main dialog�� ������ ����
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
//	//	// ���� ���� ����ü�� ���� �Է�
//	//	dlg->m_imgData.Input(dlg->m_imgInput);
//	//	// ��� ���� Display
//	//	ICT_META_DATA ICTMetaData;
//	//	dlg->displayIplImage(dlg->m_imgInput, dlg->m_CamRect[0], ICTMetaData);
//	//	//dlg->UpdateData(false);
//	//	cvWaitKey(1);
//	//}
//	//cvReleaseCapture(&dlg->capture);
//	return 0;
//}