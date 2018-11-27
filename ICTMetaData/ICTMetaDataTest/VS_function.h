#ifndef _VSS_H_
#define _VSS_H_

#include "stdafx.h"
#include "stdio.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2/video/tracking.hpp>

#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/video/video.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/videostab/videostab.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/ts/ts.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "CvvImage.h"
#include "STR_function.h"
#include <algorithm>
#include <vector>

using namespace cv;

//////////////////////////////////////////////////////////////////////////////////
// Background Generation ///////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////
// 움직임 벡터 계산///////////////////////////////////////////////////////////////
void BG_OpticalFlow(unsigned char * src1, unsigned char * src2, int width, int height, unsigned char * Motion,unsigned char *Direction, float Motion_threshold, int block_size_mode = 0);
// 배경 생성 /////////////////////////////////////////////////////////////////////
bool BG_Update(ImageData * ImgData, float threshold_diff, float Threshold_Motion,int LK_mode = 0, double speed = 1);
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// Detection & Pattern /////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////
// 객체 검출 /////////////////////////////////////////////////////////////////////
int Obj_Detection(ImageData * ImgData, DTPARAM * pObjectData, int threshold_diff, int size_threshold, int object_min_sizex, int object_min_sizey);
int Obj_DetectionGMM(ImageData * ImgData, DTPARAM * Object_Data, int size_threshold, int object_min_sizex, int object_min_sizey, bool ROIflag = 0, CPoint * ROI = NULL);

// 침식 //////////////////////////////////////////////////////////////////////////
unsigned char * Obj_Erosion(unsigned char * src, int w, int h);
// 팽창 //////////////////////////////////////////////////////////////////////////
unsigned char * Obj_Dilation(unsigned char * src, int w, int h);
// 라벨링 ////////////////////////////////////////////////////////////////////////
int Obj_Labeling(LPBYTE Dest, int width, int height, int * Area, int * MassX, int * MassY, int Size_threshold);
// 실루엣 검출 ///////////////////////////////////////////////////////////////////
int Obj_OutlineSearch(LPBYTE Dest, int width, int height, int st_x, int st_y, int x_length, int y_length, LPDTPARAM OutlineRect);
int Model_OutlineSearch(IplImage * Dest, ShapeData * Dst);
double CompareShape(ShapeData * Src, ShapeData * Dst);

//////////////////////////////////////////////////////////////////////////////////
// 객체 추적 /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
bool Tr_Tracking(ImageData * ImgData, LPDTPARAM Object_Data, LPTRPARAM Identy_Data);

void Kalman_Prediction(LPKTRPARAM Tracking_Data, KalmanParam *KalParam);
void Kalman_Update(LPKTRPARAM Tracking_Data, KalmanParam *KalParam);
void DataAssociation(LPDTPARAM Observe_Data, LPKTRPARAM Object_Data);
int lap(int dim, int **assigncost, int *rowsol, int *colsol, int *u, int *v);
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// Event     /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void Event_Crossing(LPKTRPARAM Object_Data, CPoint * Line, bool Crossing_On);
float LineAngle(CPoint p1, CPoint p2);
int Direction(CPoint * Line, CPoint p1);


//////////////////////////////////////////////////////////////////////////////////
// Recognition////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void RecognizeVehicle(ImageData * ImgData, LPKTRPARAM Object_Data);
void RecognizeHumanHoG(ImageData * ImgData, LPKTRPARAM Object_Data);
void RecognizeHumanHaar(ImageData * ImgData, LPKTRPARAM Object_Data);
void TwoRectOcclusion(CRect A, CRect B, double * area, double * ratioA, double * ratioB);

#endif
