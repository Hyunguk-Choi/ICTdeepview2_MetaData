#ifndef _STR_H_
#define _STR_H_

#include "stdafx.h"
#include "stdio.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2/video/tracking.hpp>
#include "CvvImage.h"
#include "BGGMM.h"

#define WIDTH 320 ///< 사용되는 영상의 width 크기
#define HEIGHT 240 ///< 사용되는 영상의 height 크기
#define PI 3.141592

typedef struct KalmanParam
{
	double Assoc_cost;
	double New_hyp_cost;
	CvMat * A;
	CvMat * G;
	CvMat * B;
	CvMat * Q;
	CvMat * R;
	int MaxPropagate;

	KalmanParam(){
		Assoc_cost = 10000;
		New_hyp_cost = 2;	
		MaxPropagate = 100;
		
		float A_kernel[5][5] = {{1, 0, 1, 0, 0},
								{0, 1, 0, 1, 0},
								{0, 0, 1, 0, 0},
								{0, 0, 0, 1, 0},
								{0, 0, 0, 0, 1}};

		
		
		float Eye_kernel[5][5] = {{1, 0, 0, 0, 0},
								{0, 1, 0, 0, 0},
								{0, 0, 1, 0, 0},
								{0, 0, 0, 1, 0},
								{0, 0, 0, 0, 1}};

		float R_kernel[5][5] = {{1, 0, 0, 0, 0},
								{0, 1, 0, 0, 0},
								{0, 0, 1, 0, 0},
								{0, 0, 0, 1, 0},
								{0, 0, 0, 0, 1}};
		float q = 0.1;
		float Q_kernel[5][5] = {{q*q, 0, 0, 0, 0},
								{0, q*q, 0, 0, 0},
								{0, 0, q*q, 0, 0},
								{0, 0, 0, q*q, 0},
								{0, 0, 0, 0, q*q}};

		A = cvCreateMat(5, 5, CV_32FC1);
		G = cvCreateMat(5, 5, CV_32FC1);
		B = cvCreateMat(5, 5, CV_32FC1);
		Q = cvCreateMat(5, 5, CV_32FC1);
		R = cvCreateMat(5, 5, CV_32FC1);
		
		for(int row = 0; row<A->rows; row++)
		{
			float* ptr1 = (float*)(A->data.ptr + row * A->step);
			float* ptr2 = (float*)(G->data.ptr + row * A->step);
			float* ptr3 = (float*)(B->data.ptr + row * A->step);
			float* ptr4 = (float*)(Q->data.ptr + row * A->step);
			float* ptr5 = (float*)(R->data.ptr + row * A->step);
		
			for(int col = 0; col<A->cols; col++, ptr1++, ptr2++, ptr3++, ptr4++, ptr5++)
			{
				*ptr1 = A_kernel[row][col];
				*ptr2 = Eye_kernel[row][col];
				*ptr3 = Eye_kernel[row][col];
				*ptr4 = Q_kernel[row][col];
				*ptr5 = R_kernel[row][col];
			}
			
		}
		
	}

	~KalmanParam(){		
		cvReleaseMat(&A);
		cvReleaseMat(&G);
		cvReleaseMat(&B);
		cvReleaseMat(&Q);
		cvReleaseMat(&R);
	}

	void UpdateMean(double * x_t, double * x_t_plus){		
		
		for(int row = 0; row<A->rows; row++)
		{
			float* ptr = (float*)(A->data.ptr + row * A->step);
			double sum = 0;
			for(int col = 0; col<A->cols; col++, ptr++)
			{
				sum += (double)*ptr * x_t[col];
			}
			x_t_plus[row] = sum;
		}
	}

	void UpdateCov(double * P_t, double * P_t_plus){		
		CvMat *X1, *A_t, *G_t, *A1, *A2, *B1, *B2;
		X1 = cvCreateMat(5, 5, CV_32FC1);

		A_t = cvCreateMat(5, 5, CV_32FC1);
		G_t = cvCreateMat(5, 5, CV_32FC1);

		A1 = cvCreateMat(5, 5, CV_32FC1);
		A2 = cvCreateMat(5, 5, CV_32FC1);

		B1 = cvCreateMat(5, 5, CV_32FC1);
		B2 = cvCreateMat(5, 5, CV_32FC1);

		for(int row = 0; row<X1->rows; row++)
		{
			float* ptr = (float*)(X1->data.ptr + row * X1->step);
			for(int col = 0; col<X1->cols; col++, ptr++)
			{
				*ptr = (float)P_t[row*5 + col];
			}
		}

		cvmTranspose(A, A_t);
		cvmTranspose(G, G_t);
		cvMatMul(A, X1, A1);
		cvMatMul(A1, A_t, A2);

		cvMatMul(G, Q, B1);
		cvMatMul(B1, G_t, B2);

		for(int row = 0; row<B2->rows; row++)
		{			
			float* ptr1 = (float*)(B2->data.ptr + row * B2->step);
			float* ptr2 = (float*)(A2->data.ptr + row * A2->step);

			for(int col = 0; col<A2->cols; col++, ptr1++, ptr2++)
			{
				P_t_plus[row*5 + col] = (double)*ptr1 + (double)*ptr2;
			}
		}
		
		cvReleaseMat(&X1);
		cvReleaseMat(&A_t);
		cvReleaseMat(&G_t);

		cvReleaseMat(&A1);
		cvReleaseMat(&A2);
		cvReleaseMat(&B1);
		cvReleaseMat(&B2);
	}

	void ExpectMean(double * x_t_plus, double * expected_y_t_plus){		
		for(int row = 0; row<B->rows; row++)
		{
			float* ptr = (float*)(B->data.ptr + row * B->step);
			double sum = 0;
			for(int col = 0; col<B->cols; col++, ptr++)
			{
				sum += (double)*ptr * x_t_plus[col];
			}
			expected_y_t_plus[row] = sum;
		}
	}

	void ExpectVar(double * P_t_plus, double * expected_sigma_y_t_plus){		

		CvMat *X1, *B_t, *A1, *A2;
		X1 = cvCreateMat(5, 5, CV_32FC1);

		B_t = cvCreateMat(5, 5, CV_32FC1);
		
		A1 = cvCreateMat(5, 5, CV_32FC1);
		A2 = cvCreateMat(5, 5, CV_32FC1);
		
		for(int row = 0; row<X1->rows; row++)
		{
			float* ptr = (float*)(X1->data.ptr + row * X1->step);
			for(int col = 0; col<X1->cols; col++, ptr++)
			{
				*ptr = (float)P_t_plus[row*5 + col];
			}
		}

		cvmTranspose(B, B_t);
		cvMatMul(B, X1, A1);
		cvMatMul(A1, B_t, A2);

		for(int row = 0; row<A2->rows; row++)
		{			
			float* ptr1 = (float*)(R->data.ptr + row * R->step);
			float* ptr2 = (float*)(A2->data.ptr + row * A2->step);

			for(int col = 0; col<A2->cols; col++, ptr1++, ptr2++)
			{
				expected_sigma_y_t_plus[row*5 + col] = (double)*ptr1 + (double)*ptr2;
			}
		}
		
		cvReleaseMat(&X1);
		cvReleaseMat(&B_t);

		cvReleaseMat(&A1);
		cvReleaseMat(&A2);
	}

	void Measure_Update(double * y_t, double * x_t_plus, double * P_t_plus, double * x_t, double * P_t)
	{
		CvMat *P_tp;
		CvMat *A1, *A2, *A3, *B_t, *B_tmp;

		P_tp = cvCreateMat(5, 5, CV_32FC1);
		A1 = cvCreateMat(5, 5, CV_32FC1);
		A2 = cvCreateMat(5, 5, CV_32FC1);
		A3 = cvCreateMat(5, 5, CV_32FC1);
		B_t = cvCreateMat(5, 5, CV_32FC1);
		B_tmp = cvCreateMat(5, 5, CV_32FC1);

		for(int row = 0; row<P_tp->rows; row++)
		{
			float* ptr = (float*)(P_tp->data.ptr + row * P_tp->step);
			float* ptr1 = (float*)(B->data.ptr + row * B->step);
			float* ptr2 = (float*)(B_tmp->data.ptr + row * B_tmp->step);
			for(int col = 0; col<P_tp->cols; col++, ptr++, ptr1++, ptr2++)
			{
				*ptr = (float)P_t_plus[row*5 + col];
				*ptr2 = (float)*ptr1;
				
			}
		}

		cvmTranspose(B, B_t);
		
		cvMatMul(B, P_tp, A1);
		cvMatMul(A1, B_t, A2);

		for(int row = 0; row<A2->rows; row++)
		{
			float* ptr = (float*)(A2->data.ptr + row * A2->step);
			float* ptr1 = (float*)(R->data.ptr + row * R->step);
			for(int col = 0; col<A2->cols; col++, ptr++, ptr1++)
			{
				*ptr = (double)*ptr * (double)*ptr1;
			}			
		}

		cvInvert(A2, A1, CV_LU); 
		cvMatMul(P_tp, B_t, A2);
		cvMatMul(A2, A1, A3);
		cvMatMul(A3, B, A1);
		cvMatMul(A1, P_tp, A2);

		double tmp[5];

		for(int row = 0; row<B->rows; row++)
		{
			float* ptr = (float*)(B->data.ptr + row * B->step);
			double sum = 0;
			for(int col = 0; col<B->cols; col++, ptr++)
			{
				sum += (double)*ptr * x_t_plus[col];
			}			
			tmp[row] = y_t[row] - sum;
		}

		for(int row = 0; row<A2->rows; row++)
		{
			float* ptr = (float*)(A2->data.ptr + row * A2->step);
			double sum = 0;
			for(int col = 0; col<A2->cols; col++, ptr++)
			{
				sum += (double)*ptr * tmp[col];
			}			
			x_t[row] = x_t_plus[row] + sum;
		}

		cvMatMul(A2, B, A1);
		cvMatMul(A1, P_tp, A2);

		for(int row = 0; row<A2->rows; row++)
		{
			float* ptr = (float*)(A2->data.ptr + row * A2->step);
			double sum = 0;
			for(int col = 0; col<A2->cols; col++, ptr++)
			{
				P_t[row*5 + col] = P_t_plus[row*5 + col] - (double)*ptr;
			}			
		}
	
		cvReleaseMat(&P_tp);
		cvReleaseMat(&A1);
		cvReleaseMat(&A2);
		cvReleaseMat(&A3);
		cvReleaseMat(&B_t);
	}
}KalmanParam;




///	@struct ImageData
/// @brief VSS에서 사용되는 영상들을 관리하기 위한 구조체
typedef struct ImageData
{
	IplImage *	ImgInput;				///< 비디오 원본 영상
	IplImage *	ImgOrigin;				///< WIDTH x HEIGHT로 샘플림된 영상
	IplImage *	ImgGray;				///< 그레이로 변환된 영상
	
	IplImage *	ImgColorBG;				// 컬러기반으로 생성된 배경
	IplImage *	ImgLongBG;				///< 배경 영상
	IplImage *	ImgBackground;			///< 배경 영상
	
	IplImage *	ImgColorFG;				// 컬러기반으로 추출된 전경
	IplImage *	ImgLongFG;				// 컬러기반으로 추출된 전경
	IplImage *	ImgForeground;			// 그레이기반으로 추출된 전경
	IplImage *	ImgShadow;				// 그림자 제거용 영상
	IplImage *	ImgShadowC;				// 그림자 제거용 영상

	IplImage *	ImgPrevious;			// 프레임 차분을 위한 이전 영상	

	IplImage *	ImgLabeling;			///< 라벨링된 전경 객체

	IplImage *	ImgOverlay;				///< 객체 정보 추출 결과 영상
	IplImage *	ImgOverlay2;				///< 객체 정보 추출 결과 영상
	IplImage *	ImgMotion;				// 모션 추출 영상
	IplImage *	ImgDifference;			// 프레임 차분 결과 영상

	CvPixelBackgroundGMM* m_pGMM;

	double *	ImgBGRate;				// 
	double *	ImgBGRateC;				// 
	double *	ImgLBGRateC;				// 

	int			CurrentFrame;			///< 현재 프레임 번호
	int			UpdateRate;

	ImageData(){
		ImgInput = NULL;
		CurrentFrame = 0;
	}

	///	@brief 영상들을 input 영상 크기에 맞춰서 동적 생성으로 초기화
	/// @param input 입력 영상
	void Create(IplImage * input)
	{
		if(ImgInput != NULL)
			Reset();

		int depth;  
		int width;
		int height; 

		depth  = input->nChannels;
		width  = input->width;
		height = input->height;

		ImgInput = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
		ImgOrigin = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);
		ImgGray = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);


		ImgColorBG = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);
		ImgLongBG = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);
		ImgBackground = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);

		ImgColorFG = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);
		ImgForeground = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);
		ImgLongFG = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);
		ImgShadow = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);
		ImgShadowC = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);

		ImgPrevious = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);
		ImgLabeling = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);
		ImgMotion = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);
		ImgDifference = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);		
		
		ImgLabeling = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 1);		
		ImgOverlay = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);	
		ImgOverlay2 = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);	

		m_pGMM = cvCreatePixelBackgroundGMM(WIDTH,HEIGHT) ;

		ImgBGRate = new double[WIDTH*HEIGHT];	
		ImgBGRateC = new double[WIDTH*HEIGHT*3];	
		ImgLBGRateC = new double[WIDTH*HEIGHT*3];	
		
		cvCopyImage(input, ImgInput);
		cvResize(input, ImgOrigin);		

		cvCopyImage(ImgOrigin, ImgOverlay);	
		cvCopyImage(ImgOrigin, ImgOverlay2);	

		//cvSmooth(ImgOrigin, ImgOrigin, CV_GAUSSIAN, 3,3);
	
		cvCvtColor(ImgOrigin, ImgGray, CV_BGR2GRAY);
		cvCopyImage(ImgOrigin, ImgColorBG);	
		cvCopyImage(ImgOrigin, ImgLongBG);	
		cvCopyImage(ImgGray, ImgBackground);	
		cvCopyImage(ImgGray, ImgPrevious);	
		
		for(int i=0; i<HEIGHT; i++)
		{	
			for(int j=0; j<WIDTH; j++) 
			{
				ImgBGRate[i*WIDTH+j] = (BYTE)ImgBackground->imageData[i*ImgBackground->widthStep + j*3];
			}
		}

		for(int i=0; i<HEIGHT; i++)
		{	
			for(int j=0; j<WIDTH; j++) 
			{
				ImgBGRateC[i*WIDTH*3+j*3] = (BYTE)ImgColorBG->imageData[i*ImgColorBG->widthStep + j*3];
				ImgBGRateC[i*WIDTH*3+j*3+1] = (BYTE)ImgColorBG->imageData[i*ImgColorBG->widthStep + j*3+1];
				ImgBGRateC[i*WIDTH*3+j*3+2] = (BYTE)ImgColorBG->imageData[i*ImgColorBG->widthStep + j*3+2];

				ImgLBGRateC[i*WIDTH*3+j*3] = (BYTE)ImgColorBG->imageData[i*ImgColorBG->widthStep + j*3];
				ImgLBGRateC[i*WIDTH*3+j*3+1] = (BYTE)ImgColorBG->imageData[i*ImgColorBG->widthStep + j*3+1];
				ImgLBGRateC[i*WIDTH*3+j*3+2] = (BYTE)ImgColorBG->imageData[i*ImgColorBG->widthStep + j*3+2];
			}
		}

		CurrentFrame = 0;
	}

	///	@brief 비디오의 시간에 따른 현재 프레임 영상 갱신
	/// @param input 입력 영상
	void Input(IplImage * input)
	{
		int depth  = input->nChannels;
		int width  = input->width;
		int height = input->height;

		cvCopyImage(input, ImgInput);
		cvResize(input, ImgOrigin);		
		cvCopyImage(ImgOrigin, ImgOverlay);	
		cvCopyImage(ImgOrigin, ImgOverlay2);	

		//cvSmooth(ImgOrigin, ImgOrigin, CV_GAUSSIAN, 3,3);		

		cvCopyImage(ImgGray, ImgPrevious);
		cvCvtColor(ImgOrigin, ImgGray, CV_BGR2GRAY);

		CurrentFrame += 1;
	}

	///	@brief 영상들의 배열 해제 \n
	/// Create(IplImage * input) 실행시 이미 영상들이 동적할당이 되어 있는 경우 실행됨
	void Reset()
	{
		cvReleaseImage(&ImgInput);
		cvReleaseImage(&ImgOrigin);
		cvReleaseImage(&ImgBackground);
		cvReleaseImage(&ImgGray);
		cvReleaseImage(&ImgLabeling);		
		cvReleaseImage(&ImgOverlay);
		cvReleaseImage(&ImgOverlay2);

		cvReleaseImage(&ImgColorBG);
		cvReleaseImage(&ImgLongBG);

		cvReleaseImage(&ImgBackground);
		cvReleaseImage(&ImgColorFG);
		cvReleaseImage(&ImgLongFG);
		cvReleaseImage(&ImgForeground);
		cvReleaseImage(&ImgShadow);		
		cvReleaseImage(&ImgShadowC);

		cvReleaseImage(&ImgPrevious);
		cvReleaseImage(&ImgMotion);
		cvReleaseImage(&ImgDifference);
		cvReleasePixelBackgroundGMM(&m_pGMM);

		delete [] ImgBGRate;
		delete [] ImgBGRateC;
		delete [] ImgLBGRateC;

		CurrentFrame = 0;
	}
} ImageData;




typedef struct _TRAJECTORY 
{
	int x[100];					// 궤적 X좌표
	int y[100];					// 궤적 Y좌표		
} TRJ, * LPTRJ;

struct labelnode
{
	short x;
	short y;
};

struct Position
{
	double x;
	double y;
};

// 실루엣 구조체
typedef struct ShapeData
{

	short * xchain;
	short * ychain;
	int num;


	ShapeData()
	{
		xchain = NULL;
		ychain = NULL;
		num = 0;
	}

	~ShapeData()
	{
		Clear();
	}

	void Insert(short * cx, short * cy, int c_num)
	{
		if(xchain) 
		{
			delete[] xchain;
			xchain = NULL;
		}
		if(ychain) 
		{
			delete[] ychain;
			ychain = NULL;
		}
		
		xchain = new short[c_num];
		ychain = new short[c_num];

		num = c_num;

		for(int i=0; i<num; i++)
		{
			xchain[i] = cx[i];
			ychain[i] = cy[i];
		}						
	}

	void Cal_Center(int &x, int &y)
	{
		long center_x = 0;
		long center_y = 0;

		for(int i=0; i<num; i++)
		{
			center_x += xchain[i];
			center_y += ychain[i];
		}
		if(num == 0)
		{
			x = -1;
			y = -1;
		}
		else
		{
			x = (int)(center_x / num);
			y = (int)(center_y / num);
		}
	}

	void Clear()
	{
		if(xchain) delete[] xchain;
		if(ychain) delete[] ychain;
		xchain = NULL;
		ychain = NULL;
	}
} ShapeData;

typedef struct MotionData
{
	double move_x;
	double move_y;

	MotionData()
	{
		move_x = 0.0;
		move_y = 0.0;
	}
} MotionData;

// 객체 검출 정보 구조체
typedef struct _DETECTION_PARAM {

	int Total_size;				// 생성배열 개수
	int Object_n;				// 객체정보 개수
				
	RECT * Object_info;			// 객체 영영 정보
	int * box_size;				// 객체 넓이
	int * box_x;				// 객체 가로 크기
	int * box_y;				// 객체 세로 크기

	int * Area;					// 객체 넓이(픽셀 개수)

	double * Angle;				// 주축 각도
	double * Major_len;			// 장축 길이
	double * Minor_len;			// 단축 길수

	int * Object_MassX;			// X축 무게중심
	int * Object_MassY;			// Y축 무게중심
	int * Object_OldMassX;		// X축 이전 무게중심
	int * Object_OldMassY;		// Y축 이전 무게중심

	Position * Object_Pos;			// X축 무게중심
	
	MotionData *	MD;			// 객체 움직임 정보
	ShapeData *		SD;			// 객체 실루엣 정보
	
	_DETECTION_PARAM() {
		Total_size = 0;
		Object_n = 0;
		Object_info = NULL;

		box_size = NULL;
		box_x    = NULL;
		box_y    = NULL;		

		MD = NULL;
		SD = NULL;

		Angle = NULL;
		Major_len = NULL;
		Minor_len = NULL;

		Area = NULL;

		Object_MassX = NULL;
		Object_MassY = NULL;

		Object_OldMassX = NULL;
		Object_OldMassY = NULL;
		Object_Pos = NULL;

	}

	bool Create(int num) {
		if(num < 0) return false;

		if(Object_info) {
			delete[] Object_info;
			Object_info = NULL;
		}
		if(Area) {
			delete[] Area;
			Area = NULL;
		}
		if(Object_MassX) {
			delete[] Object_MassX;
			Object_MassX = NULL;
		}
		if(Object_MassY) {
			delete[] Object_MassY;
			Object_MassY = NULL;
		}
		if(Object_OldMassX) {
			delete[] Object_OldMassX;
			Object_OldMassX = NULL;
		}
		if(Object_OldMassY) {
			delete[] Object_OldMassY;
			Object_OldMassY = NULL;
		}

		if(Object_Pos) {
			delete[] Object_Pos;
			Object_Pos = NULL;
		}

		if(SD) {
			delete[] SD;
			SD = NULL;
		}

		if(Angle) {
			delete[] Angle;
			Angle = NULL;
		}

		if(Major_len) {
			delete[] Major_len;
			Major_len = NULL;
		}

		if(Minor_len) {
			delete[] Minor_len;
			Minor_len = NULL;
		}

		if(MD) {
			delete[] MD;
			MD = NULL;
		}

		if(box_size) {
			delete[] box_size;
			box_size = NULL;
		}
		if(box_x) {
			delete[] box_x;
			box_x = NULL;
		}

		if(box_y) {
			delete[] box_y;
			box_y = NULL;
		}

		Total_size = num;
		Object_n = 0;
		

		Object_info = new RECT[num];
		box_size = new int[num];
		box_x    = new int[num];
		box_y    = new int[num];

		Area = new int[num];
		
		Object_MassX= new int[num];
		Object_MassY= new int[num];
		Object_OldMassX= new int[num];
		Object_OldMassY= new int[num];	
		Object_Pos= new Position[num];	
		SD = new ShapeData[num];
		Angle = new double[num];
		Major_len = new double[num];
		Minor_len = new double[num];
		MD = new MotionData[num];
		
		return true;
	}

	~_DETECTION_PARAM() {
		if(Object_info)	delete[] Object_info;
		if(Area) delete[] Area;
	
		if(box_size) delete[] box_size;
		if(box_x)    delete[] box_x;
		if(box_y)    delete[] box_y;		
		
		if(Object_MassX) delete[] Object_MassX;
		if(Object_MassY) delete[] Object_MassY;
		if(Object_OldMassX) delete[] Object_OldMassX;
		if(Object_OldMassY) delete[] Object_OldMassY;
		if(Object_Pos) delete[] Object_Pos;
		
		if(SD) delete[] SD;
		if(MD) delete[] MD;

		if(Angle) delete[] Angle;
		if(Major_len) delete[] Major_len;
		if(Minor_len) delete[] Minor_len;
				
	}

	void EllipseFit() 
	{
		if(Object_n>0)
		{
			for(int i=0; i<Object_n; i++)
			{
				double m00 = SD[i].num;
				double Xm, Ym;
				double a, b, c;
				double Imin, Imax;
				double alpha, beta;
				Xm = 0, Ym = 0, a = 0, b = 0, c = 0;
				Imin = 0, Imax = 0;
				for(int j=0; j<m00; j++)
				{
					Xm += SD[i].xchain[j];
					Ym += SD[i].ychain[j];
				}
				Xm = Xm/m00;
				Ym = Ym/m00;

				for(int j=0; j<m00; j++)
				{
					a += pow((SD[i].xchain[j]-Xm),2);
					b += (SD[i].xchain[j]-Xm)*(SD[i].ychain[j]-Ym);
					c += pow((SD[i].ychain[j]-Ym),2);
				}
				
				Angle[i] = ((double)(atan(2*b/(a-c))))*0.5;

				for(int j=0; j<m00; j++)
				{
					Imin += pow((SD[i].xchain[j]-Xm)*cos(Angle[i])-(SD[i].ychain[j]-Ym)*sin(Angle[i]),2);
					Imax += pow((SD[i].xchain[j]-Xm)*sin(Angle[i])-(SD[i].ychain[j]-Ym)*cos(Angle[i]),2);
				}

				alpha = 1.6*pow((double)(4/PI),((double)1/(double)4))*pow(pow(Imin,3)/Imax,(double)1/(double)8);
				beta = 1.6*pow((double)(4/PI),((double)1/(double)4))*pow(pow(Imax,3)/Imin,(double)1/(double)8);
				/*if(alpha > beta)
				{
					Major_len[i] = alpha;
					Minor_len[i] = beta;
					Object_MassX[i] = (int)Xm;
					Object_MassY[i] = (int)Ym;

				}
				else*/
				{
					Major_len[i] = beta;
					Minor_len[i] = alpha;
					Object_MassX[i] = (int)Xm;
					Object_MassY[i] = (int)Ym;
					Angle[i] = Angle[i] + PI/2;
				}
			}
		}				
	}
} DTPARAM, * LPDTPARAM;


// 객체 추적 정보 구조체
typedef struct _TRACKING_PARAM {
	int total_size;			// 배열 개수
	int Object_n;			// 객체 개수
	int NewID;			// 객체 개수
	int * Object_ID;			// ID

	int * Frame_Start;			// Start Frame
	int * Frame_End;			// Start Frame
	
	TRJ * Trajectory;		// 궤적
	
	RECT * Object_info;		// 객체 영역
			
	int * Object_MassX;		// X축 무게중심
	int * Object_MassY;		// Y축 무게중심

	double * Angle;			// 객체 Shape의 Angle
	double * Major_len;		// 주축의 길이
	double * Minor_len;		// 짧은축의 길이
	
	int * Object_Size;			// 크기
	
	ShapeData *		SD;			// Shape Data
	ShapeData *		SD1;		// Shape Data

	int * Object_Assign;		// 객체 할당

	// Event Flag
	int * DirectionFlag;
	
	_TRACKING_PARAM() {
		total_size = 0;
		Object_n = 0;
		NewID = 0;

		Object_info=NULL;

		Object_ID=NULL;
		
		Frame_Start = NULL;
		Frame_End = NULL;

		Object_MassX = NULL;
		Object_MassY = NULL;
		Object_Assign = NULL;
		
		Angle = NULL;
		Major_len = NULL;
		Minor_len = NULL;
		
		Object_Size = NULL;
		Trajectory = NULL;

		SD = NULL;
		SD1 = NULL;

		DirectionFlag = NULL;

	}

	bool Create(int num) {
		if(num < 0) return false;

		if(SD) {
			delete[] SD;
			SD = NULL;
		}

		if(SD1) {
			delete[] SD1;
			SD1 = NULL;
		}

		if(Frame_Start) {
			delete[] Frame_Start;
			Frame_Start = NULL;
		}
		if(Frame_End) {
			delete[] Frame_End;
			Frame_End = NULL;
		}

				
		if(Object_MassX) {
			delete[] Object_MassX;
			Object_MassX = NULL;
		}
		if(Object_MassY) {
			delete[] Object_MassY;
			Object_MassY = NULL;
		}

		if(Object_Assign) {
			delete[] Object_Assign;
			Object_Assign = NULL;
		}
		
		if(Angle) {
			delete[] Angle;
			Angle = NULL;
		}

		if(Major_len) {
			delete[] Major_len;
			Major_len = NULL;
		}

		if(Minor_len) {
			delete[] Minor_len;
			Minor_len = NULL;
		}

		if(Object_Size) {
			delete[] Object_Size;
			Object_Size = NULL;
		}

		if(Trajectory) {
			delete[] Trajectory;
			Trajectory = NULL;
		}

		if(Object_info) {
			delete[] Object_info;
			Object_info = NULL;
		}

		if(Object_ID) {
			delete[] Object_ID;
			Object_ID = NULL;
		}

		if(DirectionFlag) {
			delete[] DirectionFlag;
			DirectionFlag = NULL;
		}

		Trajectory= new TRJ[num];
		
		for(int i=0;i<num;i++)
		{
			for(int j=0;j<100;j++)
			{
				Trajectory[i].x[j]=0;
				Trajectory[i].y[j]=0;
			}

		}	

		Frame_Start= new int[num];
		Frame_End= new int[num];

		NewID = 0;
		Object_n = 0;
		Object_ID= new int[num];
		Object_MassX= new int[num];
		Object_MassY= new int[num];
		Object_Assign= new int[num];

		Angle = new double[num];
		Major_len = new double[num];
		Minor_len = new double[num];

		Object_info= new RECT[num];
		Object_Size= new int[num];
		
		SD = new ShapeData[num];
		SD1 = new ShapeData[num];

		DirectionFlag = new int[num];

		return true;
	}

	~_TRACKING_PARAM() 
	{	
		if(Object_ID)
			delete[] Object_ID;
		if(Object_MassX)
			delete[] Object_MassX;
		if(Object_MassY)
			delete[] Object_MassY;	
		if(Object_Assign)
			delete[] Object_Assign;	

		if(Frame_Start)
			delete[] Frame_Start;
		if(Frame_End)
			delete[] Frame_End;	

		if(Angle) delete[] Angle;
		if(Major_len) delete[] Major_len;
		if(Minor_len) delete[] Minor_len;

		if(Object_Size)
			delete[] Object_Size;	
	
		if(Trajectory)
			delete[] Trajectory;

		if(Object_info)
			delete[] Object_info;

		if(SD) 
			delete[] SD;
		if(SD1) 
			delete[] SD1;

		if(DirectionFlag) 
			delete[] DirectionFlag;
		
	}
} TRPARAM, * LPTRPARAM;

typedef struct _KALMAN_DATA {
	double x_t[5];
	double x_t_plus[5];

	double P_t[5*5];
	double P_t_plus[5*5];

	double y_t[5];

	double expected_y_t_plus[5];
	double expected_sigma_y_t_plus[5*5];

	double new_kalman;
	double ntimes_propagated;
	double ntimes_updated;

} KALMANDATA;

// Kalman filter 기반 객체 추적 정보 구조체
typedef struct _KTRACKING_PARAM {
	int total_size;			// 배열 개수
	int Object_n;			// 객체 개수
	int NewID;			// 객체 개수
	int * Object_ID;			// ID

	int * Frame_Start;			// Start Frame
	int * Frame_End;			// Start Frame
	
	TRJ * Trajectory;		// 궤적
	
	RECT * Object_info;		// 객체 영역
			
	int * Object_MassX;		// X축 무게중심
	int * Object_MassY;		// Y축 무게중심
	int * Object_Assign;		//

	double * Angle;			// 객체 Shape의 Angle
	double * Major_len;		// 주축의 길이
	double * Minor_len;		// 짧은축의 길이
	
	int * Object_Size;			// 크기
	
	ShapeData *		SD;			// Shape Data
	ShapeData *		SD1;			// Shape Data

	// Kalman filter
	KALMANDATA * Object_Kalman; 

	//Object Type
	int * Object_Type;
	
	// Event Flag
	int * DirectionFlag;

	_KTRACKING_PARAM() {
		total_size = 0;
		Object_n = 0;
		NewID = 0;

		Object_info=NULL;

		Object_ID=NULL;
		
		Frame_Start = NULL;
		Frame_End = NULL;

		Object_MassX = NULL;
		Object_MassY = NULL;
		Object_Assign = NULL;

		Angle = NULL;
		Major_len = NULL;
		Minor_len = NULL;
		Object_Kalman = NULL;
		Object_Size = NULL;
		Trajectory = NULL;

		SD = NULL;
		SD1 = NULL;

		Object_Type = NULL;
		DirectionFlag = NULL;
	}

	bool Create(int num) {
		if(num < 0) return false;

		if(SD) {
			delete[] SD;
			SD = NULL;
		}

		if(SD1) {
			delete[] SD1;
			SD1 = NULL;
		}

		if(Frame_Start) {
			delete[] Frame_Start;
			Frame_Start = NULL;
		}
		if(Frame_End) {
			delete[] Frame_End;
			Frame_End = NULL;
		}

				
		if(Object_MassX) {
			delete[] Object_MassX;
			Object_MassX = NULL;
		}
		if(Object_MassY) {
			delete[] Object_MassY;
			Object_MassY = NULL;
		}
		if(Object_Assign) {
			delete[] Object_Assign;
			Object_Assign = NULL;
		}
		
		if(Angle) {
			delete[] Angle;
			Angle = NULL;
		}

		if(Major_len) {
			delete[] Major_len;
			Major_len = NULL;
		}

		if(Minor_len) {
			delete[] Minor_len;
			Minor_len = NULL;
		}

		if(Object_Size) {
			delete[] Object_Size;
			Object_Size = NULL;
		}
		

		if(Trajectory) {
			delete[] Trajectory;
			Trajectory = NULL;
		}

		if(Object_info) {
			delete[] Object_info;
			Object_info = NULL;
		}

		if(Object_ID) {
			delete[] Object_ID;
			Object_ID = NULL;
		}

		if(Object_Kalman) {
			delete[] Object_Kalman;
			Object_Kalman = NULL;
		}

		if(Object_Type) {
			delete[] Object_Type;
			Object_Type = NULL;
		}


		if(DirectionFlag) {
			delete[] DirectionFlag;
			DirectionFlag = NULL;
		}

		Trajectory= new TRJ[num];
		
		for(int i=0;i<num;i++)
		{
			for(int j=0;j<100;j++)
			{
				Trajectory[i].x[j]=0;
				Trajectory[i].y[j]=0;
			}

		}	

		Frame_Start= new int[num];
		Frame_End= new int[num];

		NewID = 0;
		Object_n = 0;
		Object_ID= new int[num];
		Object_MassX= new int[num];
		Object_MassY= new int[num];
		Object_Assign= new int[num];

		Angle = new double[num];
		Major_len = new double[num];
		Minor_len = new double[num];

		Object_info= new RECT[num];
		Object_Size= new int[num];
		Object_Kalman = new KALMANDATA[num];
		SD = new ShapeData[num];
		SD1 = new ShapeData[num];

		Object_Type = new int[num];

		DirectionFlag = new int[num];

		return true;
	}

	~_KTRACKING_PARAM() 
	{	
		if(Object_ID)
			delete[] Object_ID;
		if(Object_MassX)
			delete[] Object_MassX;
		if(Object_MassY)
			delete[] Object_MassY;	
		if(Object_Assign)
			delete[] Object_Assign;	

		if(Frame_Start)
			delete[] Frame_Start;
		if(Frame_End)
			delete[] Frame_End;	

		if(Angle) delete[] Angle;
		if(Major_len) delete[] Major_len;
		if(Minor_len) delete[] Minor_len;

		if(Object_Size)
			delete[] Object_Size;	
	
		if(Trajectory)
			delete[] Trajectory;

		if(Object_info)
			delete[] Object_info;

		if(Object_Kalman)
			delete[] Object_Kalman;
		
		if(SD) 
			delete[] SD;
		if(SD1) 
			delete[] SD1;

		if(Object_Type) 
			delete[] Object_Type;

		if(DirectionFlag) 
			delete[] DirectionFlag;
		
	}
} KTRPARAM, * LPKTRPARAM;



#endif
