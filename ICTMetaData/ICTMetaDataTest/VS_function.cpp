#include "stdafx.h"
#include "VS_function.h"
#include "math.h"


//////////////////////////////////////////////////////////////////////////////////
///////// Background Generation //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// 움직임 벡터 계산///////////////////////////////////////////////////////////////
void BG_OpticalFlow(unsigned char * source1, unsigned char * source2, int width, int height, unsigned char * Motion,unsigned char * Direction, float Motion_threshold, int block_size_mode)
{
	int block_size_x, block_size_y;
	if(block_size_mode == 1)		{block_size_x = 5; block_size_y = 5;}
	else if(block_size_mode == 2)	{block_size_x = 7; block_size_y = 7;}
	else if(block_size_mode == 3)	{block_size_x = 9; block_size_y = 9;}
	else if(block_size_mode == 4)	{block_size_x = 12; block_size_y = 12;}
	else							{block_size_x = 3; block_size_y = 3;}
	int half_block_size_x = block_size_x / 2;
	int half_block_size_y = block_size_y / 2;
	
   	float dx1, dx2, dt;
	float Inverse_Parameter[2][2];
	float Inverse_Mat_Data[2][2];
	float Minus_Parameter[2];
	float Motion_x, Motion_y;
	int i, j, x, y, p;
	int Scale=5;
	BYTE fillvalue;
	
	memset(Motion,0,width*height*sizeof(unsigned char));
	
	for(j=0; j<height-block_size_y; j+=block_size_y) {
		for(i=0; i<width-block_size_x; i+=block_size_x) {
			memset(Inverse_Parameter, 0, sizeof(float)*4);
			memset(Minus_Parameter, 0, sizeof(float)*2);
			for(y=j; y<=j+block_size_y; y++)
			{
				for(x=i; x<=i+block_size_x; x++) 
				{
					p = x + y * width;
					dx1 = 0.25f * (source1[p+1] - source1[p] + source1[p+1+width] - source1[p+width] + source2[p+1] - source2[p] + source2[p+1+width] - source2[p+width]);
					dx2 = 0.25f * (source1[p+width] - source1[p] + source1[p+1+width] - source1[p+1] + source2[p+width] - source2[p] + source2[p+1+width] - source2[p+1]);
					dt = 0.25f * (source2[p] - source1[p] + source2[p+1] - source1[p+1] + source2[p+width] - source1[p+width] + source2[p+1+width] - source1[p+1+width]);

					//1. det 계산
					Inverse_Parameter[0][0]+=(float)pow(dx1,2);
					Inverse_Parameter[0][1]+=dx1*dx2;
					Inverse_Parameter[1][0]+=dx1*dx2;
					Inverse_Parameter[1][1]+=(float)pow(dx2,2);
					Minus_Parameter[0] += dx1*dt;
					Minus_Parameter[1] += dx2*dt;
				}
			}
			Minus_Parameter[0] *= -1.0f;
			Minus_Parameter[1] *= -1.0f;

	int num=2;			 
	float cash[10][10], stand[10][10];
	float temp, d=0.0, t=0.0;

	for(int k=0;k<num;k++)
    for(int l=0;l<num;l++) 
	{
		cash[k][l]=Inverse_Parameter[k][l];
		stand[k][l]=(k==l)?1.0f:0.0f;
    }
  
  	for(int k=0;k<num;k++)   //0이 아닌 수로 만들어 준다
		if(cash[k][k]==0.0) {
			for(int l=0;l<num;l++) {
				if(cash[l][k]==0.0) continue;
				     for(int m=0;m<num;m++) {
	        			  cash[k][m]+=cash[k][m];
	       				  stand[l][m]+=stand[k][m];
					  }
       			  break;
			}
		}
  
	  for(int k=0;k<num;k++) {
	  temp=cash[k][k];
	  for(int l=0;l<num;l++) { //1로 만들어 준다
	       cash[k][l]/=temp;
	       stand[k][l]/=temp;
      }
          
  	  for(int m=0;m<num;m++) { //0으로 만들어 준다
      	  if(k==m) continue; 
		  if(cash[m][k]==0) continue;
		  temp=cash[m][k];
	  	   for(int l=0;l<num;l++) {
			   cash[m][l]=cash[m][l]-cash[k][l]*temp;
	  		   stand[m][l]=stand[m][l]-stand[k][l]*temp;
		   }
      }
  }
  for(int k=0;k<num;k++)
    for(int l=0;l<num;l++) 
		Inverse_Mat_Data[k][l]=stand[k][l];

		Motion_x =(float)((Inverse_Mat_Data[0][0]*Minus_Parameter[0]+Inverse_Mat_Data[0][1]*Minus_Parameter[1])*Scale);
		Motion_y =(float)((Inverse_Mat_Data[1][0]*Minus_Parameter[0]+Inverse_Mat_Data[1][1]*Minus_Parameter[1])*Scale)*(-1);

		float mag=0, angle = 0;	
		mag=sqrt(Motion_x*Motion_x+Motion_y*Motion_y);		
		if(mag > Motion_threshold)	fillvalue = 255;
		else	
			fillvalue = 0;

		for(int y=j; y<=j+block_size_y; y++) 
			for(int x=i; x<=i+block_size_x; x++)
			{								
				
				Motion[x + y * width] = fillvalue;
				/*if(Motion_x>Motion_threshold)
					Direction[x + y * width] = 1;
				else if(Motion_x<-Motion_threshold)
					Direction[x + y * width] = 2;
				else
					Direction[x + y * width] = 0;

				if(Motion_y>Motion_threshold)
					Direction[x + y * width] += 4;
				else if(Motion_y<-Motion_threshold)
					Direction[x + y * width] += 8;	*/

				if(mag < Motion_threshold)
				{
					Direction[x + y * width] = 0;
				}
				else if(Motion_x == 0)
				{
					if(Motion_y == 0)
					{
						Direction[x + y * width] = 0;
					}
					else if(Motion_y < 0)
					{
						Direction[x + y * width] = 1;
					}
					else
					{
						Direction[x + y * width] = 5;
					}
				}
				else if(Motion_x > 0)
				{
					if(Motion_y == 0)
					{
						Direction[x + y * width] = 3;
					}
					else if(Motion_y < 0)
					{
						angle = (atan(abs(Motion_y)/abs(Motion_x))*180.0)/PI;
						if(angle <= 22.5)
							Direction[x + y * width] = 3;
						else if(angle<=67.5)
							Direction[x + y * width] = 2;
						else
							Direction[x + y * width] = 1;
					}
					else
					{
						angle = (atan(abs(Motion_y)/abs(Motion_x))*180.0)/PI;
						if(angle <= 22.5)
							Direction[x + y * width] = 3;
						else if(angle<=67.5)
							Direction[x + y * width] = 4;
						else
							Direction[x + y * width] = 5;
					}
				}
				else
				{
					if(Motion_y == 0)
					{
						Direction[x + y * width] = 7;
					}
					else if(Motion_y < 0)
					{
						angle = (atan(abs(Motion_y)/abs(Motion_x))*180.0)/PI;
						if(angle <= 22.5)
							Direction[x + y * width] = 7;
						else if(angle<=67.5)
							Direction[x + y * width] = 8;
						else
							Direction[x + y * width] = 1;
					}
					else
					{
						angle = (atan(abs(Motion_y)/abs(Motion_x))*180.0)/PI;
						if(angle <= 22.5)
							Direction[x + y * width] = 7;
						else if(angle<=67.5)
							Direction[x + y * width] = 6;
						else
							Direction[x + y * width] = 5;
					}
				}
			}
		}
	}	
}

// 배경 생성 /////////////////////////////////////////////////////////////////////
bool BG_Update(ImageData * ImgData, float threshold_diff, float Threshold_Motion, int LK_mode, double speed)
{	
	// input: ImgData-> 영상 구조체, threshold_diff-> frame difference의 문턱치, Threshold_Motion-> 모션값의 문턱치,
	//        LK_mode-> OpticalFlow 블록 모드 설정, speed-> 배경 업데이트 속도
	// output: bool -> 생성 완료시 true 반환
	
	int Width		= ImgData->ImgOrigin->width;
	int Widthstep	= ImgData->ImgGray->widthStep;
	int WidthstepC	= ImgData->ImgOrigin->widthStep;
	int Height		= ImgData->ImgOrigin->height;
	int Total_size	= Width * Height;

	unsigned char * pBackgroundImage = new unsigned char[Width * Height];
	memset(pBackgroundImage, 0x00, Width * Height);
	unsigned char * pCurrentImage = new unsigned char[Width * Height];
	memset(pCurrentImage, 0x00, Width * Height);
	unsigned char * pDelayImage = new unsigned char[Width * Height];
	memset(pDelayImage, 0x00, Width * Height);
	
	//unsigned char * tsource0 = pBackgroundImage;
	//unsigned char * tsource1 = pCurrentImage;
	//unsigned char * tsource2 = pDelayImage;
		
	for(int i=0; i<Height; i++)
	{
		for(int j=0; j<Width; j++)
		{
			pBackgroundImage[i*Width + j]   = (BYTE)ImgData->ImgBackground->imageData[i*Widthstep + j];
			pCurrentImage[i*Width + j]   = (BYTE)ImgData->ImgGray->imageData[i*Widthstep + j];
			pDelayImage[i*Width + j]   = (BYTE)ImgData->ImgPrevious->imageData[i*Widthstep + j];
		}
	}
	
	////////////////////////////////////////////////////////////////////
	// Frame difference
	////////////////////////////////////////////////////////////////////
	unsigned char * pDifferImage = new unsigned char[Total_size * sizeof(unsigned char)];
	for(int i=0; i<Height; i++)
	{	
		for(int j=0; j<Width; j++) 
		{
			pDifferImage[i*Width + j] = (abs((int)pCurrentImage[i*Width + j] - (int)pDelayImage[i*Width + j]) > threshold_diff) ? 255 : 0;			
			ImgData->ImgDifference->imageData[i*Widthstep + j] = (BYTE)pDifferImage[i*Width + j];
		}
	}
	
	////////////////////////////////////////////////////////////////////
	// Motion - Optical-flow
	////////////////////////////////////////////////////////////////////
	unsigned char * pMotionImage = new unsigned char[Total_size * sizeof(unsigned char)];
	unsigned char * pDirectionImage = new unsigned char[Total_size * sizeof(unsigned char)];
	
	BG_OpticalFlow(pCurrentImage, pDelayImage, Width, Height, pMotionImage, pDirectionImage, Threshold_Motion, LK_mode); 

	for(int i=0; i<Height; i++) 
	{
		for(int j=0; j<Width; j++) 
		{
			//ImgData->ImgMotion->imageData[i*Widthstep + j] = 						
			BYTE ori = (BYTE)pDirectionImage[i*Width + j];
			if(ori==1) // bottom
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)124;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)74;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)96;	
								
			}
			else if(ori==2) // left top
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)200;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)20;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)248;		

			}
			else if(ori==3) //Left
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)255;	

			}
			else if(ori==4) // left bottom
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)255;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)255;	

			}
			else if(ori==5) // top
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)84;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)138;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)148;	
								

			}
			else if(ori==6)  // right down
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)221;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)205;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)147;	

			}
			else if(ori==7) //Right
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)255;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)0;	

			}
			else if(ori==8) // right up
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)255;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)0;	

			}
			else
			{
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+0] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+1] = (BYTE)0;	
				ImgData->ImgMotion->imageData[i*ImgData->ImgMotion->widthStep+j*3+2] = (BYTE)0;	
			}
		}
	}	

	////////////////////////////////////////////////////////////////////
	// Background Update
	////////////////////////////////////////////////////////////////////
	for(int i=0; i<Height; i++)
	{
		for(int j=0; j<Width; j++) 
		{
			int pos = j + i * Width;
		
			if(pMotionImage[pos] == 0 && pDifferImage[pos]==0)
			{
				double r;
				r = pBackgroundImage[pos] > pCurrentImage[pos] ? (ImgData->ImgBGRate[i*Width + j] - speed) : (ImgData->ImgBGRate[i*Width + j] + speed);
					
				if(r > 255)
				{
					ImgData->ImgBGRate[i*Width + j] = 255;					
				}
				else if(r < 0)
				{
					ImgData->ImgBGRate[i*Width + j] = 0;					
				}
				else
				{
					ImgData->ImgBGRate[i*Width + j] = r;
				}
				ImgData->ImgBackground->imageData[i*Widthstep + j] = (BYTE)ImgData->ImgBGRate[i*Width + j];
				

				for(int k=0; k<3; k++)
				{
					double s;
					s = (BYTE)ImgData->ImgColorBG->imageData[i*WidthstepC + j*3+k] > (BYTE)ImgData->ImgOrigin->imageData[i*WidthstepC + j*3+k]
						? ImgData->ImgBGRateC[i*Width*3 + j*3+k] - speed : ImgData->ImgBGRateC[i*Width*3 + j*3+k] + speed;
												
					if(s > 255)
					{
						ImgData->ImgBGRateC[i*Width*3 + j*3+k] = 255;								
					}
					else if(s < 0)
					{
						ImgData->ImgBGRateC[i*Width*3 + j*3+k] = 0;	
					}
					else
					{
						ImgData->ImgBGRateC[i*Width*3 + j*3+k] = s;	
					}					
					ImgData->ImgColorBG->imageData[i*WidthstepC + j*3+k] = (BYTE)ImgData->ImgBGRateC[i*Width*3 + j*3+k];

					s = (BYTE)ImgData->ImgLongBG->imageData[i*WidthstepC + j*3+k] > (BYTE)ImgData->ImgColorBG->imageData[i*WidthstepC + j*3+k]
						? ImgData->ImgLBGRateC[i*Width*3 + j*3+k] - speed*0.1 : ImgData->ImgLBGRateC[i*Width*3 + j*3+k] + speed*0.1;
												
					if(s > 255)
					{
						ImgData->ImgLBGRateC[i*Width*3 + j*3+k] = 255;								
					}
					else if(s < 0)
					{
						ImgData->ImgLBGRateC[i*Width*3 + j*3+k] = 0;	
					}
					else
					{
						ImgData->ImgLBGRateC[i*Width*3 + j*3+k] = s;	
					}					
					ImgData->ImgLongBG->imageData[i*WidthstepC + j*3+k] = (BYTE)ImgData->ImgLBGRateC[i*Width*3 + j*3+k];
				}								
			}		
		}
	}

	delete[] pBackgroundImage;
	delete[] pCurrentImage;
	delete[] pDelayImage;
	delete[] pMotionImage;
	delete[] pDifferImage;
	delete[] pDirectionImage;
 	
	return true;
}


//////////////////////////////////////////////////////////////////////////////////
///////// Detection & Pattern //////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////
// 객체 검출 /////////////////////////////////////////////////////////////////////

int Obj_DetectionGMM(ImageData * ImgData, DTPARAM * Object_Data, int size_threshold, int object_min_sizex, int object_min_sizey, bool ROIflag, CPoint * ROI)
{
	// input: ImgData-> 영상 구조체, Object_Data-> 객체 검출 정보 구조체, threshold_diff-> 입력영상과 배경의 차분 문턱치,
	//        size_threshold-> 객체 넓이의 문턱치, object_min_sizex-> 객체 가로 크기의 문턱치, object_min_sizey-> 객체 세로 크기의 문턱치
	// output: int -> 검출된 객체 갯수
	
	int width		= ImgData->ImgOrigin->width;
	int widthstep	= ImgData->ImgGray->widthStep;
	int widthstepC	= ImgData->ImgOrigin->widthStep;
	int height		= ImgData->ImgOrigin->height;
	int Total_size	= width * height;

	unsigned char * pForegroundImage = new unsigned char[width * height];
	memset(pForegroundImage, 0x00, width * height);
	unsigned char * pCurrentImage = new unsigned char[width * height*3];
	memset(pCurrentImage, 0x00, width * height*3);
	
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			//pBackgroundImage[i*width + j] = (BYTE)ImgData->ImgBackground->imageData[i*widthstep + j];
			pCurrentImage[(i*width)*3 + j*3] = (BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + 3*j+2];
			pCurrentImage[(i*width)*3 + j*3+1] = (BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + 3*j+1];
			pCurrentImage[(i*width)*3 + j*3+2] = (BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + 3*j];
		}
	}
	cvUpdatePixelBackgroundGMM(ImgData->m_pGMM,pCurrentImage,pForegroundImage);
	////////////////////////////////////////////////////////////////////
	// 입력영상과 배경영상의 차분(그림자 제거)
	////////////////////////////////////////////////////////////////////
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			//pBackgroundImage[i*width + j] = (BYTE)ImgData->ImgBackground->imageData[i*widthstep + j];
			ImgData->ImgColorBG->imageData[i*widthstepC + 3*j+2] = (BYTE)ImgData->m_pGMM->rGMM[(i*width + j)*ImgData->m_pGMM->nM].muR;
			ImgData->ImgColorBG->imageData[i*widthstepC + 3*j+1] = (BYTE)ImgData->m_pGMM->rGMM[(i*width + j)*ImgData->m_pGMM->nM].muG;
			ImgData->ImgColorBG->imageData[i*widthstepC + 3*j+0] = (BYTE)ImgData->m_pGMM->rGMM[(i*width + j)*ImgData->m_pGMM->nM].muB;

			if(ROIflag == 1)
			{
				if(!(ROI[0].y < i && ROI[1].y > i && ROI[0].x < j && ROI[1].x > j))
				{
					pForegroundImage[i*width+j] = 0;
				}
			}
		}
	}
	//	
	Obj_Erosion(pForegroundImage, width, height);
//	Obj_Erosion(pForegroundImage, width, height);
	Obj_Dilation(pForegroundImage, width, height);
	Obj_Dilation(pForegroundImage, width, height);
	Obj_Erosion(pForegroundImage, width, height);
	//
	for(int i=0; i<height; i++)
	{
		for (int j=0; j<width ;j++)
		{
	 		ImgData->ImgForeground->imageData[i*widthstep + j] = (BYTE)pForegroundImage[i*width + j] ;
			
		}
	}
	
	////////////////////////////////////////////////////////////////////
	// Detection
	////////////////////////////////////////////////////////////////////
	int MassX[256],MassY[256],Area[256];
	int Object_num = Obj_Labeling(pForegroundImage, width, height, Area, MassX, MassY, size_threshold);
	if(Object_num <= 0){
		Object_Data->Object_n=0;
		delete[] pForegroundImage;
		delete[] pCurrentImage;
		return 0;
	}

	Object_Data->Create(Object_num);

	int outnum = Obj_OutlineSearch(pForegroundImage, width, height, 0, 0, width, height, Object_Data);
	
	// 크기 작은 라벨들을 제거
	if(outnum > 0 && outnum <= Object_num )
	{
		int find_num = 0;
		for(int i=0; i<Object_Data->Object_n; i++)
		{			
			int size_x = Object_Data->Object_info[i].right - Object_Data->Object_info[i].left;
			int size_y = Object_Data->Object_info[i].bottom - Object_Data->Object_info[i].top;
			int boundary = 0;
			if(Object_Data->Object_info[i].left < 10 || Object_Data->Object_info[i].top < 10 || Object_Data->Object_info[i].bottom > height - 10 || Object_Data->Object_info[i].right > width - 10)
				boundary = 1;

			if(size_x >= object_min_sizex && size_y >= object_min_sizey && size_x < width && size_y < height && boundary == 0)
			{
				Object_Data->Object_info[find_num]  = Object_Data->Object_info[i];
				// 크기 저장
				Object_Data->box_size[find_num] = size_x * size_y;
				Object_Data->box_x[find_num]    = size_x;
				Object_Data->box_y[find_num]    = size_y;

				Object_Data->Object_MassX[find_num] = MassX[i+1];
				Object_Data->Object_MassY[find_num] = MassY[i+1];
				Object_Data->Area[find_num]    = Area[i+1];
				Object_Data->Object_Pos[find_num].x = (Object_Data->Object_info[i].right-Object_Data->Object_info[i].left)/2+Object_Data->Object_info[i].left;
				Object_Data->Object_Pos[find_num].y = Object_Data->Object_info[i].bottom;

				if(find_num<i)
				Object_Data->SD[find_num].Insert(Object_Data->SD[i].xchain,Object_Data->SD[i].ychain,Object_Data->SD[i].num);
							
				for(int j=0; j<width*height; j++)
				{
					if(pForegroundImage[j] == (i+1)) pForegroundImage[j] = find_num+1;
				}
				find_num++;					
			}
			else
			{
				for(int j=0; j<width*height; j++)
				{
					if(pForegroundImage[j] == (i+1)) pForegroundImage[j] = (BYTE) 0;
				}

			}
		}
		Object_Data->Object_n = find_num;
	}
	
	for(int i=0; i<height; i++)
	{
		for (int j=0; j<width ;j++)
		{
			ImgData->ImgLabeling->imageData[i*widthstep + j]= (BYTE)pForegroundImage[j + i * width];	
		}
	}	
	 	

	delete[] pForegroundImage;
	delete[] pCurrentImage;
	
	return Object_Data->Object_n;
}


int Obj_Detection(ImageData * ImgData, DTPARAM * Object_Data, int threshold_diff, int size_threshold, int object_min_sizex, int object_min_sizey)
{
	// input: ImgData-> 영상 구조체, Object_Data-> 객체 검출 정보 구조체, threshold_diff-> 입력영상과 배경의 차분 문턱치,
	//        size_threshold-> 객체 넓이의 문턱치, object_min_sizex-> 객체 가로 크기의 문턱치, object_min_sizey-> 객체 세로 크기의 문턱치
	// output: int -> 검출된 객체 갯수
	
	int width		= ImgData->ImgOrigin->width;
	int widthstep	= ImgData->ImgGray->widthStep;
	int widthstepC	= ImgData->ImgOrigin->widthStep;
	int height		= ImgData->ImgOrigin->height;
	int Total_size	= width * height;

	unsigned char * pBackgroundImage = new unsigned char[width * height];
	memset(pBackgroundImage, 0x00, width * height);
	unsigned char * pCurrentImage = new unsigned char[width * height];
	memset(pCurrentImage, 0x00, width * height);
	
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			pBackgroundImage[i*width + j] = (BYTE)ImgData->ImgBackground->imageData[i*widthstep + j];
			pCurrentImage[i*width + j] = (BYTE)ImgData->ImgGray->imageData[i*widthstep + j];
		}
	}

	////////////////////////////////////////////////////////////////////
	// 입력영상과 배경영상의 차분(그림자 제거)
	////////////////////////////////////////////////////////////////////
	unsigned char * pDifferImage = new unsigned char[Total_size * sizeof(unsigned char)];
	for(int i=0; i<height; i++)
	{	
		for(int j=0; j<width; j++) 
		{
			pDifferImage[i*width + j] = (abs((int)(BYTE)pCurrentImage[i*width + j] - (int)(BYTE)pBackgroundImage[i*width + j]) > threshold_diff) ? 255 : 0;			
		}
	}
	
	////////////////////////////////////////////////////////////////////
	// Shadow Removal
	////////////////////////////////////////////////////////////////////	
	unsigned char * pShadow = new unsigned char[Total_size * sizeof(unsigned char) * 3];
	int s1,s2,sum;
	float shadow1[2], shadow2[2],result[3];	

	for(int i=0; i<height; i++)
	{						
		for (int j=0; j<width ;j++)
		{
			double numerator;
			double denominator;

			sum=abs((int)(BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + j*3]-(int)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3])
				+abs((int)(BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + j*3+1]-(int)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1])
				+abs((int)(BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + j*3+2]-(int)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2]);
			
			if(abs(sum) > threshold_diff)
				pDifferImage[j + i * width]=255;
			else
				pDifferImage[j + i * width]=0;

			numerator = ((double)(BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + j*3]*(double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3])
				+((double)(BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + j*3+1]*(double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1])
				+((double)(BYTE)ImgData->ImgOrigin->imageData[i*widthstepC + j*3+2]*(double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2]);

			denominator = ((double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3]*(double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3])
				+((double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1]*(double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1])
				+((double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2]*(double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2]);

			if (denominator == 0)
			{
				pShadow[j + i * width]=255;	
				ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)255 ;
				continue;
			}
			float a = numerator / denominator;
			float fTau = 0.8f;//
			float fTb = 3*4;
			// if tau < a < 1 then also check the color distortion
			if ((a <= 1) && (a >= fTau))//m_nBeta=1
			{
				float dR=a * (double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2] - (double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2];
				float dG=a * (double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1] - (double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1];
				float dB=a * (double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3] - (double)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3];

				//square distance -slower and less accurate
				//float maxDistance = cvSqrt(m_fTb*var);
				//if ((fabs(dR) <= maxDistance) && (fabs(dG) <= maxDistance) && (fabs(dB) <= maxDistance))
				//circle
				float dist=(dR*dR+dG*dG+dB*dB);
				if (dist<fTb*5*a*a)
				{
					pShadow[j + i * width]=255;	
					ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)0 ;
				}
				else
				{
					pShadow[j + i * width]=0;	
					ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)125 ;
				}
			}		
			else
			{
				pShadow[j + i * width]=255;	
				ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)255 ;
			}

			sum=abs((int)(BYTE)ImgData->ImgLongBG->imageData[i*widthstepC + j*3]-(int)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3])
				+abs((int)(BYTE)ImgData->ImgLongBG->imageData[i*widthstepC + j*3+1]-(int)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+1])
				+abs((int)(BYTE)ImgData->ImgLongBG->imageData[i*widthstepC + j*3+2]-(int)(BYTE)ImgData->ImgColorBG->imageData[i*widthstepC + j*3+2]);
			
			if(abs(sum) > threshold_diff)
				ImgData->ImgLongFG->imageData[i*widthstep + j]=255;
			else
				ImgData->ImgLongFG->imageData[i*widthstep + j]=0;
		}
	}

	//Obj_Dilation(pShadow, width, height);
	//Obj_Dilation(pShadow, width, height);
	//Obj_Dilation(pShadow, width, height);
		
	Obj_Erosion(pDifferImage, width, height);
	Obj_Dilation(pDifferImage, width, height);
	Obj_Dilation(pDifferImage, width, height);
	Obj_Erosion(pDifferImage, width, height);
	
	for(int i=0; i<height; i++)
	{
		for (int j=0; j<width ;j++)
		{
	 		if(pShadow[j + i * width] > 0 && pDifferImage[j + i * width] > 0)// && pNow4[j]>0)
			{
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3] = (BYTE)255 ;
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3+1] = (BYTE)255 ;
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3+2] = (BYTE)255 ;

				//ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)255 ;
				ImgData->ImgForeground->imageData[i*widthstep + j] = (BYTE)255 ;

				pDifferImage[j + i * width]=255;
			}
			else if(pDifferImage[j + i * width]>0 )
			{
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3] = (BYTE)255 ;
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3+1] = (BYTE)0 ;
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3+2] = (BYTE)255 ;

				//ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)0 ;
				ImgData->ImgForeground->imageData[i*widthstep + j] = (BYTE)0 ;

				pDifferImage[j + i * width]=0;

			}
			else
			{
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3] = (BYTE)0 ;
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3+1] = (BYTE)0 ;
				ImgData->ImgShadowC->imageData[i*widthstepC + j*3+2] = (BYTE)0 ;

				//ImgData->ImgShadow->imageData[i*widthstep + j] = (BYTE)0 ;
				ImgData->ImgForeground->imageData[i*widthstep + j] = (BYTE)0 ;
				pDifferImage[j + i * width]=0;
			}
		}
	}
	delete [] pShadow;

	////////////////////////////////////////////////////////////////////
	// Detection
	////////////////////////////////////////////////////////////////////
	int MassX[256],MassY[256],Area[256];
	int Object_num = Obj_Labeling(pDifferImage, width, height, Area, MassX, MassY, size_threshold);
	if(Object_num <= 0){
		Object_Data->Object_n=0;
		delete[] pBackgroundImage;
		delete[] pCurrentImage;
		delete[] pDifferImage;
		
		return 0;
	}

	Object_Data->Create(Object_num);

	int outnum = Obj_OutlineSearch(pDifferImage, width, height, 0, 0, width, height, Object_Data);
	
	// 크기 작은 라벨들을 제거
	if(outnum > 0 && outnum <= Object_num )
	{
		int find_num = 0;
		for(int i=0; i<Object_Data->Object_n; i++)
		{			
			int size_x = Object_Data->Object_info[i].right - Object_Data->Object_info[i].left;
			int size_y = Object_Data->Object_info[i].bottom - Object_Data->Object_info[i].top;
			int boundary = 0;
			if(Object_Data->Object_info[i].left < 10 || Object_Data->Object_info[i].top < 10 || Object_Data->Object_info[i].bottom > height - 10 || Object_Data->Object_info[i].right > width - 10)
				boundary = 1;

			if(size_x >= object_min_sizex && size_y >= object_min_sizey && size_x < width && size_y < height && boundary == 0)
			{
				Object_Data->Object_info[find_num]  = Object_Data->Object_info[i];
				// 크기 저장
				Object_Data->box_size[find_num] = size_x * size_y;
				Object_Data->box_x[find_num]    = size_x;
				Object_Data->box_y[find_num]    = size_y;

				Object_Data->Area[find_num]    = Area[i+1];

				Object_Data->Object_MassX[find_num] = MassX[i+1];
				Object_Data->Object_MassY[find_num] = MassY[i+1];
				Object_Data->Object_Pos[find_num].x = (Object_Data->Object_info[i].right-Object_Data->Object_info[i].left)/2+Object_Data->Object_info[i].left;
				Object_Data->Object_Pos[find_num].y = Object_Data->Object_info[i].bottom;
				if(find_num<i)
				Object_Data->SD[find_num].Insert(Object_Data->SD[i].xchain,Object_Data->SD[i].ychain,Object_Data->SD[i].num);
							
				for(int j=0; j<width*height; j++)
				{
					if(pDifferImage[j] == (i+1)) pDifferImage[j] = find_num+1;
				}
				find_num++;					
			}
			else
			{
				for(int j=0; j<width*height; j++)
				{
					if(pDifferImage[j] == (i+1)) pDifferImage[j] = (BYTE) 0;
				}

			}
		}
		Object_Data->Object_n = find_num;
	}
	
	for(int i=0; i<height; i++)
	{
		for (int j=0; j<width ;j++)
		{
			ImgData->ImgLabeling->imageData[i*widthstep + j]= (BYTE)pDifferImage[j + i * width];	
		}
	}	
	 	

	delete[] pBackgroundImage;
	delete[] pCurrentImage;
	delete[] pDifferImage;
	
	return Object_Data->Object_n;
}

// 침식 //////////////////////////////////////////////////////////////////////////
unsigned char * Obj_Erosion(unsigned char * src, int w, int h)
{
	int wR = w-1;
	int hR = h-1;
	int x, y;
	unsigned char * psrcU, * psrcC, * psrcD, * psrc;
	unsigned char * dst  = new BYTE[w*h];
	memcpy(dst, src, sizeof(unsigned char)*w*h);
	memset(src, 0x00, sizeof(unsigned char)*w*h);
	for(y=1; y<hR; y++)	{
		psrcU = dst + w*(y-1);
		psrcC = dst + w*y;
		psrcD = dst + w*(y+1);
		psrc  = src + w*y;
		for(x=1; x<wR; x++) {
			psrcU++;
			psrcC++;
			psrcD++;
			psrc++;
			if(*psrcU==0xFF && *(psrcC-1)==0xFF && *psrcC==0xFF && *(psrcC+1)==0xFF && *psrcD==0xFF )
				*psrc = 0xFF;
		}
	}
	delete dst;
	return src;
}

// 팽창 //////////////////////////////////////////////////////////////////////////
unsigned char * Obj_Dilation(unsigned char * src, int w, int h)
{
	int wR = w-1;
	int hR = h-1;
	int x, y;
	unsigned char * psrcU, * psrcC, * psrcD, * psrc;
	unsigned char * dst = new unsigned char[w*h];
	memcpy(dst, src, sizeof(unsigned char)*w*h);
	memset(src, 0x00, sizeof(unsigned char)*w*h);
	for(y=1; y<hR; y++)	{
		psrcU = dst + w*(y-1);
		psrcC = dst + w*y;
		psrcD = dst + w*(y+1);
		psrc  = src + w*y;
		for(x=1; x<wR; x++)	{
			psrcU++;
			psrcC++;
			psrcD++;
			psrc++;
			if(*psrcU==0xFF || *(psrcC-1)==0xFF || *psrcC==0xFF || *(psrcC+1)==0xFF || *psrcD==0xFF)
				*psrc = 0xFF;
		}
	}
	delete dst;
	return src;
}

// 라벨링 ////////////////////////////////////////////////////////////////////////
int Obj_Labeling(LPBYTE Dest, int width, int height,int * Area, int * MassX, int * MassY, int Size_threshold)
{
	int p, mp, sp;
	int Totalsize = width*height;
	
	int Search_current = 0;
	int Search_End = 0;
	int mark_num = 0;
	int count;

	labelnode * Search_node = new labelnode[Totalsize];
	LPBYTE visit = new BYTE[Totalsize];
	memset(visit, 0x00, Totalsize);

	//시작위치 
	mp = 0;
	while(mp<Totalsize)
	{
		//시작위치 찾기
		if(visit[mp] == 0)
		{
			visit[mp] = 1;
			if(Dest[mp] > 0)
			{
				mark_num++;
				MassX[mark_num]=mp%width;
				MassY[mark_num]=mp/width;
	
				Dest[mp] = mark_num;
				Search_current = 0;
				Search_End = 0;
				Search_node[Search_End].x = mp % width;
				Search_node[Search_End].y = mp / width;
				Search_End++;
				count = 0;

				//시작
				while(Search_current<Search_End)
				{
					//주변 탐색
					p = Search_node[Search_current].x + Search_node[Search_current].y * width;

					//0 degree
					sp = p + 1;
					if(Search_node[Search_current].x + 1 < width && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x + 1;
						Search_node[Search_End].y = Search_node[Search_current].y;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//90 degree
					sp = p + width;
					if(Search_node[Search_current].y + 1 < height && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x;
						Search_node[Search_End].y = Search_node[Search_current].y + 1;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//180 degree
					sp = p - 1;
					if(Search_node[Search_current].x - 1 >= 0 && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x - 1;
						Search_node[Search_End].y = Search_node[Search_current].y;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//270 degree
					sp = p - width;
					if(Search_node[Search_current].y - 1 >= 0 && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x;
						Search_node[Search_End].y = Search_node[Search_current].y - 1;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//45 degree
					sp = p + width + 1;
					if(Search_node[Search_current].x + 1 < width && Search_node[Search_current].y + 1 < height && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x + 1;
						Search_node[Search_End].y = Search_node[Search_current].y + 1;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//135 degree
					sp = p + width - 1;
					if(Search_node[Search_current].x - 1 >= 0 && Search_node[Search_current].y + 1 < height && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x - 1;
						Search_node[Search_End].y = Search_node[Search_current].y + 1;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//225 degree
					sp = p - width - 1;
					if(Search_node[Search_current].x - 1 >= 0 && Search_node[Search_current].y - 1 >= 0 && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x - 1;
						Search_node[Search_End].y = Search_node[Search_current].y - 1;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}

					//315 degree
					sp = p - width + 1;
					if(Search_node[Search_current].x + 1 < width && Search_node[Search_current].y - 1 >= 0 && visit[sp] == 0 && Dest[sp] > 0)
					{
						Dest[sp] = mark_num;
						visit[sp] = 1;
						Search_node[Search_End].x = Search_node[Search_current].x + 1;
						Search_node[Search_End].y = Search_node[Search_current].y - 1;
						Search_End++;
						MassX[mark_num]+=sp%width;
						MassY[mark_num]+=sp/width;
					}
					
					count++;
					Search_current++;
				}
				MassX[mark_num]=MassX[mark_num]/count;
				MassY[mark_num]=MassY[mark_num]/count;
				Area[mark_num]=count;

				if(count < Size_threshold)
				{
					for(int i=0; i<Search_End; i++)
					{
						p = Search_node[i].x + Search_node[i].y * width;
						Dest[p] = 0;
					}
					mark_num--;
				}
			}
			else
			{
				Dest[mp] = 0;
			}
		}
		mp++;
	}

	delete[] visit;
	delete[] Search_node;

	return mark_num;
}

// 실루엣 검출 ///////////////////////////////////////////////////////////////////
int Obj_OutlineSearch(LPBYTE Dest, int width, int height, int st_x, int st_y, int x_length, int y_length, LPDTPARAM OutlineRect)
{
	int num = 0;
	int skip = 1;
	int m_iObjectNum;
	int top = st_y;
	int bottom = st_y + y_length;
	int left = st_x;
	int right = left + x_length;
	int Totalsize = width*height;

	short * xchain = new short[Totalsize];
	short * ychain = new short[Totalsize];

	LPBYTE visit = new BYTE[Totalsize];
	memset(visit, 0x00, Totalsize);

	short box_left;
	short box_right;
	short box_top;
	short box_bottom;

	POINT nei[8] = {{0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}};

	int x0, y0, x, y, k, n, p;
	int border_count;
	unsigned char c0;

	m_iObjectNum = 1;
	for(y=top; y<bottom; y++)
	{
		for(x=left; x<right; x++)
		{
			p = x + y * width;
			c0 = Dest[p];

			if(c0 == m_iObjectNum && visit[p] == 0)
			{
				box_left = width - 1;
				box_right = 0;
				box_top = height - 1;
				box_bottom = 0;

				border_count = 0;
				x0 = x;
				y0 = y;
				n = 4;
				do
				{
					for(k=0; k<8; k++, n=((n+1)&7))
					{
						short u = (short)(x+nei[n].x);
						short v = (short)(y+nei[n].y);
						if(u<0 || u>=width || v<0 || v>=height) continue;
						if(Dest[u+v*width]==c0) break;
					}
					if(k==8) break;

					box_left = x < box_left ? x : box_left;
					box_right = x > box_right ? x : box_right;
					box_top = y < box_top ? y : box_top;
					box_bottom = y > box_bottom ? y : box_bottom;

					visit[x+y*width]=255;

					xchain[border_count] = x;
					ychain[border_count] = y;
					border_count++;

					if(border_count>=Totalsize) break;

					x = x + nei[n].x;
					y = y + nei[n].y;

					n = (n+5)&7;

				}while(!(x==x0 && y==y0));

				if(k==8) continue;
 
				OutlineRect->SD[OutlineRect->Object_n].Insert(xchain,ychain,border_count);
				m_iObjectNum++;
			
				OutlineRect->Object_info[OutlineRect->Object_n].left = box_left;
				OutlineRect->Object_info[OutlineRect->Object_n].right = box_right;
				OutlineRect->Object_info[OutlineRect->Object_n].top = box_top;
				OutlineRect->Object_info[OutlineRect->Object_n].bottom = box_bottom;
				OutlineRect->Object_n++;

				if(OutlineRect->Object_n >= OutlineRect->Total_size) break;
			}
		}
		if(OutlineRect->Object_n >= OutlineRect->Total_size) break;
	}
	delete [] visit;
	delete [] xchain;
	delete [] ychain;

	return m_iObjectNum-1;
}

int Model_OutlineSearch(IplImage * Dest, ShapeData * Dst)
{
	int num = 0;
	int skip = 1;
	int width = Dest->width;
	int height = Dest->height;
	int m_iObjectNum;
	int top = 0;
	int bottom =  height;
	int left = 0;
	int right = width;
	int Totalsize = width*height;

	short * xchain = new short[Totalsize];
	short * ychain = new short[Totalsize];

	LPBYTE visit = new BYTE[Totalsize];
	memset(visit, 0x00, Totalsize);

	short box_left;
	short box_right;
	short box_top;
	short box_bottom;

	POINT nei[8] = {{0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}};

	int x0, y0, x, y, k, n, p;
	int border_count;
	unsigned char c0;
	int flag;
	flag = 0;
	m_iObjectNum = 1;
	for(y=top; y<bottom; y++)
	{
		for(x=left; x<right; x++)
		{
			p = x + y * width;
			c0 = (BYTE)Dest->imageData[y*Dest->widthStep+x];

			if(c0 == 255 && visit[p] == 0)
			{
				box_left = width - 1;
				box_right = 0;
				box_top = height - 1;
				box_bottom = 0;

				border_count = 0;
				x0 = x;
				y0 = y;
				n = 4;
				do
				{
					for(k=0; k<8; k++, n=((n+1)&7))
					{
						short u = (short)(x+nei[n].x);
						short v = (short)(y+nei[n].y);
						if(u<0 || u>=width || v<0 || v>=height) continue;
						if( (BYTE)Dest->imageData[v*Dest->widthStep+u]==c0) break;
					}
					if(k==8) break;

					box_left = x < box_left ? x : box_left;
					box_right = x > box_right ? x : box_right;
					box_top = y < box_top ? y : box_top;
					box_bottom = y > box_bottom ? y : box_bottom;

					visit[x+y*width]=255;

					xchain[border_count] = x;
					ychain[border_count] = y;
					border_count++;

					if(border_count>=Totalsize) break;

					x = x + nei[n].x;
					y = y + nei[n].y;

					n = (n+5)&7;

				}while(!(x==x0 && y==y0));

				if(k==8) continue;
 
				Dst->Insert(xchain,ychain,border_count);
				flag = 1;
				break;

			}
			if(flag == 1)
				break;
		}
	}
	delete [] visit;
	delete [] xchain;
	delete [] ychain;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
///////// Tracking /////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////
// 객체 추적 /////////////////////////////////////////////////////////////////////
bool Tr_Tracking(ImageData * ImgData, LPDTPARAM Object_Data, LPTRPARAM Identy_Data)
{
	// input: ImgData-> 영상 구조체, Object_Data-> 객체 검출 정보 구조체, Identy_Data-> 추적된 객체 정보 구조체,
	// output: bool -> 검출 객체가 0개이면 false, 추적 검출 완료시 true

	// 검출 객체가 0개이면 함수 종료
	if(Object_Data->Object_n == 0)
	{
			Identy_Data->Object_n = 0;
			return false;
	}
			

	int minDist_th = 50; //최소 거리 threshold
	int width=ImgData->ImgOrigin->width, height=ImgData->ImgOrigin->height, NewObject_num=0;
	int Object_num = Object_Data->Object_n; // 새로운 객체 개수
	int Identy_num = Identy_Data->Object_n; // 기존 객체 개수

	float TempDist;

	float * Dist=new float[Identy_num*Object_num];	// 객체간 거리
	float * AllocDist=new float[Object_num];		// 할당된 번호의 거리
	int * AllocNum=new int[Object_num];				// 할당 번호
	int * AllocTimes=new int[Identy_num];			// 할당 횟수

	TRPARAM NewIdenty_Data;
	NewIdenty_Data.Create(Object_num);
				
	for(int i=0;i<Identy_num;i++)
	{
		AllocTimes[i]=0;		
	}
	for(int i=0;i<Object_num;i++)
	{
		AllocDist[i]=1000000;
		AllocNum[i]=1000000;
	}
		
	// 객체간 거리 계산
	for(int i=0;i<Identy_num;i++)
	{
		for(int j=0;j<Object_num;j++)
		{
			TempDist = (float)sqrt((float)(abs(Object_Data->Object_MassX[j]-Identy_Data->Object_MassX[i])*abs(Object_Data->Object_MassX[j]-Identy_Data->Object_MassX[i]))
				+(float)(abs(Object_Data->Object_MassY[j]-Identy_Data->Object_MassY[i])*abs(Object_Data->Object_MassY[j]-Identy_Data->Object_MassY[i])));
			Dist[j*Identy_num+i] = TempDist;
		} 
	}

	// 가까운 거리의 객체 할당
	for(int i=0;i<Identy_num;i++)
	{
		for(int j=0;j<Object_num;j++)
		{
			if(AllocDist[j] >= Dist[j*Identy_num+i] && minDist_th>Dist[j*Identy_num+i])
			{
				AllocDist[j] = Dist[j*Identy_num+i];
								
				if(AllocNum[j] == 1000000)
				{
					AllocTimes[i]++;
					AllocNum[j] = i;
	
				}
				else
				{
					AllocTimes[AllocNum[j]]--;
					AllocTimes[i]++;
					AllocNum[j] = i;

				}
			}
		}
	}

	// 여러번 할당된 객체를 한번만 할당 되게 함	
	for(int i=0;i<Identy_num;i++)
	{
		if(AllocTimes[i]>1)
		{
			float * Temp = new float[AllocTimes[i]];
			int TempNum = 0;
			float Min = 1000000;
			
			for(int j=0;j<Object_num;j++)
			{
				if(AllocNum[j ]== i)
				{
					Temp[TempNum] = AllocDist[j];
					TempNum++;
				}
			}
			for(int j=0;j<TempNum;j++)
			{
				if(Temp[j]<Min)
				{
					Min = Temp[j];
				}
			}
			for(int j=0;j<Object_num;j++)
			{
				if(AllocNum[j ]== i)
				{
					if(!(Min == AllocDist[j]))
					{
						AllocTimes[i]--;
						AllocDist[j] = 1000000;
						AllocNum[j] = 1000000;

					}
				}
			}
			delete[] Temp;
		}
	}	

	for(int i=0;i<Identy_num;i++)
	{
		if(AllocTimes[i]==0)
		{
			for(int j=0;j<Object_num;j++)
			{
				if(AllocNum[j]==1000000)
				{
					AllocNum[j]=i;
					AllocTimes[i]++;
				
					break;						
				}
			}
		}
	}

	int alloc_num = 0;
	for(int i=0;i<Object_num;i++)
	{
		if(AllocNum[i] > Identy_num)
		{
			AllocDist[i] = 1000000;
		}

		if(Object_Data->Object_MassX[i]<=0 || AllocDist[i]>minDist_th) 
		{
			AllocNum[i] = -1;

			NewObject_num++;

			for(int j=0;j<100;j++)
			{
				NewIdenty_Data.Trajectory[i].x[j]=0;
				NewIdenty_Data.Trajectory[i].y[j]=0;
			}
			NewIdenty_Data.Object_ID[i] = Identy_Data->NewID;

			Identy_Data->NewID ++;										
		}
		else
		{
			alloc_num ++;
			NewIdenty_Data.SD1[i].Insert(Identy_Data->SD[AllocNum[i]].xchain,Identy_Data->SD[AllocNum[i]].ychain,Identy_Data->SD[AllocNum[i]].num);
			
			for(int j=0;j<99;j++)
			{
				NewIdenty_Data.Trajectory[i].x[j+1]=Identy_Data->Trajectory[AllocNum[i]].x[j];
				NewIdenty_Data.Trajectory[i].y[j+1]=Identy_Data->Trajectory[AllocNum[i]].y[j];
			}

			NewIdenty_Data.Object_ID[i] = Identy_Data->Object_ID[AllocNum[i]];
			NewIdenty_Data.Frame_Start[i] = Identy_Data->Frame_Start[AllocNum[i]];
			
			NewIdenty_Data.Object_Size[i] = Identy_Data->Object_Size[AllocNum[i]];
					
						
		}

		NewIdenty_Data.SD[i].Insert(Object_Data->SD[i].xchain,Object_Data->SD[i].ychain,Object_Data->SD[i].num);
			
		NewIdenty_Data.Object_info[i]=Object_Data->Object_info[i];				

		NewIdenty_Data.Trajectory[i].x[0]=(NewIdenty_Data.Object_info[i].right-NewIdenty_Data.Object_info[i].left)/2+NewIdenty_Data.Object_info[i].left;
		NewIdenty_Data.Trajectory[i].y[0]=NewIdenty_Data.Object_info[i].bottom;
		
		NewIdenty_Data.Object_MassX[i]=Object_Data->Object_MassX[i];
		NewIdenty_Data.Object_MassY[i]=Object_Data->Object_MassY[i];

		NewIdenty_Data.Angle[i]=Object_Data->Angle[i];
		NewIdenty_Data.Major_len[i]=Object_Data->Major_len[i];
		NewIdenty_Data.Minor_len[i]=Object_Data->Minor_len[i];

	}
		
	for(int i=0;i<Object_num;i++)
	{	
		Identy_Data->SD[i].Insert(NewIdenty_Data.SD[i].xchain,NewIdenty_Data.SD[i].ychain,NewIdenty_Data.SD[i].num);		
		Identy_Data->SD1[i].Insert(NewIdenty_Data.SD1[i].xchain,NewIdenty_Data.SD1[i].ychain,NewIdenty_Data.SD1[i].num);
		
		for(int j=0;j<100;j++)
		{
			Identy_Data->Trajectory[i].x[j]=NewIdenty_Data.Trajectory[i].x[j];
			Identy_Data->Trajectory[i].y[j]=NewIdenty_Data.Trajectory[i].y[j];
		}
		
		Identy_Data->Frame_Start[i]= NewIdenty_Data.Frame_Start[i];
		Identy_Data->Object_MassX[i]=NewIdenty_Data.Object_MassX[i];
		Identy_Data->Object_MassY[i]=NewIdenty_Data.Object_MassY[i];

		Identy_Data->Angle[i]=NewIdenty_Data.Angle[i];
		Identy_Data->Major_len[i]=NewIdenty_Data.Major_len[i];
		Identy_Data->Minor_len[i]=NewIdenty_Data.Minor_len[i];

		Identy_Data->Object_ID[i] = NewIdenty_Data.Object_ID[i];
		Identy_Data->Object_info[i]=NewIdenty_Data.Object_info[i];

		Identy_Data->Object_Size[i]=NewIdenty_Data.Object_Size[i];		
	}
		
	Identy_Data->Object_n=Object_Data->Object_n;

	delete[] AllocNum;
	delete[] AllocTimes;
	delete[] AllocDist;
	delete[] Dist;
 
	return true;
}

void Kalman_Prediction(LPKTRPARAM Object_Data, KalmanParam *KalParam)
{
	int num;
	num = Object_Data->Object_n;
	if(num <1)
	{
		return;
	}

	for(int i=0; i<num; i++)
	{
		KalParam->UpdateMean(Object_Data->Object_Kalman[i].x_t, Object_Data->Object_Kalman[i].x_t_plus);
		KalParam->UpdateCov(Object_Data->Object_Kalman[i].P_t, Object_Data->Object_Kalman[i].P_t_plus);
		KalParam->ExpectMean(Object_Data->Object_Kalman[i].x_t_plus, Object_Data->Object_Kalman[i].expected_y_t_plus);
		KalParam->ExpectVar(Object_Data->Object_Kalman[i].P_t_plus, Object_Data->Object_Kalman[i].expected_sigma_y_t_plus);

		Object_Data->Object_Kalman[i].new_kalman = 0;
	}

	int alloc = 0;
	int n = 0;
	for(int i=0; i<num; i++)
	{
		if(Object_Data->Object_Kalman[i].expected_y_t_plus[0] < 0 || Object_Data->Object_Kalman[i].expected_y_t_plus[0] > WIDTH
			|| Object_Data->Object_Kalman[i].expected_y_t_plus[1] < 0 || Object_Data->Object_Kalman[i].expected_y_t_plus[1] > HEIGHT
			|| Object_Data->Object_Kalman[i].ntimes_propagated > KalParam->MaxPropagate)
		{
			
		}		
		else
		{
			if(i == n)
			{

			}
			else
			{
				Object_Data->SD[n].Insert(Object_Data->SD[i].xchain,Object_Data->SD[i].ychain,Object_Data->SD[i].num);		
		
				for(int j=0;j<100;j++)
				{
					Object_Data->Trajectory[n].x[j]=Object_Data->Trajectory[i].x[j];
					Object_Data->Trajectory[n].y[j]=Object_Data->Trajectory[i].y[j];
				}
		
				Object_Data->Frame_Start[n]= Object_Data->Frame_Start[i];
				Object_Data->Object_MassX[n]=Object_Data->Object_MassX[i];
				Object_Data->Object_MassY[n]=Object_Data->Object_MassY[i];

				Object_Data->Angle[n]=Object_Data->Angle[i];
				Object_Data->Major_len[n]=Object_Data->Major_len[i];
				Object_Data->Minor_len[n]=Object_Data->Minor_len[i];

				Object_Data->Object_ID[n] = Object_Data->Object_ID[i];
				Object_Data->Object_info[n]=Object_Data->Object_info[i];

				Object_Data->Object_Size[n]=Object_Data->Object_Size[i];		
				Object_Data->DirectionFlag[n] = Object_Data->DirectionFlag[i];
				Object_Data->Object_Type[n] = Object_Data->Object_Type[i]; 
				Object_Data->Object_Kalman[n] = Object_Data->Object_Kalman[i];
				
			}
			n ++;
		}		
	}
	Object_Data->Object_n = n;
}

void DataAssociation(LPDTPARAM Observe_Data, LPKTRPARAM Object_Data)
{
	int nObject = Object_Data->Object_n;
	int nObserve = Observe_Data->Object_n;
	if(nObject == 0 && nObserve == 0)
	{
		return;
	}
	if(nObject > 0 && nObserve == 0)
	{
		int a= 0;
		for (int i = 0; i < nObject; i++)
		{
			Object_Data->Object_Assign[i] = 0;
		}
		//Object_Data->Object_n = 0;
		return;
	}

	double * cost;
	cost = new double[nObject*nObserve];
	int * assignment;
	assignment = new int[nObject*nObserve];
	// cost 계산
	for(int i=0; i<nObject; i++)
	{
		CvMat *X;
		CvMat *Sigma, *Sigma_inv, *X_t, *Y1;
		X = cvCreateMat(5, nObserve, CV_32FC1);
		X_t = cvCreateMat(nObserve, 5, CV_32FC1);
		Y1 = cvCreateMat(nObserve, 5, CV_32FC1);

		Sigma = cvCreateMat(5, 5, CV_32FC1);
		Sigma_inv = cvCreateMat(5, 5, CV_32FC1);
				
		for(int row = 0; row<Sigma->rows; row++)
		{
			float* ptr = (float*)(Sigma->data.ptr + row * Sigma->step);
			for(int col = 0; col<Sigma->cols; col++, ptr++)
			{
				*ptr = 2 * PI * Object_Data->Object_Kalman[i].expected_sigma_y_t_plus[row * 5 + col];
			}
		}

		double det = cvmDet(Sigma);
		cvInvert(Sigma, Sigma_inv, CV_LU);  
		
		for(int row = 0; row<X->rows; row++)
		{
			float* ptr = (float*)(X->data.ptr + row * X->step);
			for(int col = 0; col<X->cols; col++, ptr++)
			{
				if(row == 0)
				{
					*ptr = (float)(Observe_Data->Object_Pos[col].x - Object_Data->Object_Kalman[i].expected_y_t_plus[row]);
				}
				else if(row == 1)
				{
					*ptr = (float)(Observe_Data->Object_Pos[col].y - Object_Data->Object_Kalman[i].expected_y_t_plus[row]);
				}
				else if(row == 2)
				{
					*ptr = 0.2*(float)(Observe_Data->Object_Pos[col].x - Object_Data->Object_Kalman[i].y_t[0] - Object_Data->Object_Kalman[i].expected_y_t_plus[row]);
				}
				else if(row == 3)
				{
					*ptr = 0.2*(float)(Observe_Data->Object_Pos[col].y - Object_Data->Object_Kalman[i].y_t[1] - Object_Data->Object_Kalman[i].expected_y_t_plus[row]);
				}
				else if(row == 4)
				{
					*ptr = 0.05*(float)(Observe_Data->Area[col] - Object_Data->Object_Kalman[i].expected_y_t_plus[row]);
				}
			}
		}

		cvmTranspose(X, X_t);
		cvMatMul(X_t, Sigma_inv, Y1);

		for(int row = 0; row<X_t->rows; row++)
		{
			float* ptr1 = (float*)(X_t->data.ptr + row * X_t->step);
			float* ptr2 = (float*)(Y1->data.ptr + row * Y1->step);
			double sum = 0;
			for(int col = 0; col<X_t->cols; col++, ptr1++, ptr2++)
			{
				sum += (double)*ptr1 * (double)*ptr2;
			}

			sum = -0.5*(sum + log(det));
			cost[i*nObserve+row] = -sum;
		}

		cvReleaseMat(&X);
		cvReleaseMat(&Sigma);
		cvReleaseMat(&Sigma_inv);
		cvReleaseMat(&X_t);
		cvReleaseMat(&Y1);
	}	

	int enddim;
	if(nObject >= nObserve)
		enddim = nObject * 2;
	else
		enddim = nObserve * 2;
	int **assigncost; 
	assigncost = new int*[enddim];
	for (int i = 0; i < enddim; i++)
		assigncost[i] = new int[enddim];

	int * rowsol = new int[enddim];
	int * colsol = new int[enddim];
	int * u = new int[enddim];
	int * v = new int[enddim];
	int MAX_COST = 200;
	
	for (int i = 0; i < enddim; i++)
	{
		for (int j = 0; j < enddim; j++)
		{
			if(i < nObject && j < nObserve)
			{
				assigncost[i][j] = cost[i*nObserve+j];
			}
			else
				assigncost[i][j] = MAX_COST;
		}
	}

	double lapcost = lap(enddim, assigncost, rowsol, colsol, u, v);
	int * assign = new int[enddim*enddim];
	memset(assign,0,sizeof(int)*enddim*enddim);
	for (int i = 0; i < enddim; i++)
	{
		assign[i*enddim + rowsol[i]] = 1;
	}

	for (int i = 0; i < nObject; i++)
	{
		for (int j = 0; j < nObserve; j++)
		{
			assignment[i*nObserve + j] = assign[i*enddim + j];
		}
	}

	delete [] assign;
	for (int i = 0; i < nObject; i++)
	{
		Object_Data->Object_Assign[i] = 0;
	}
	for (int i = 0; i < nObserve; i++)
	{
		int newly = 0;
		for (int j = 0; j < nObject; j++)
		{
			if(assignment[j* nObserve + i] == 1)
			{
				newly = 1;

				int num = Object_Data->Object_n;
				//Object_Data->SD1[num].Insert(Observe_Data->SD[AllocNum[i]].xchain,Identy_Data->SD[AllocNum[i]].ychain,Identy_Data->SD[AllocNum[i]].num);
			
				for(int k=98;k>0;k--)
				{
					Object_Data->Trajectory[j].x[k]=Object_Data->Trajectory[j].x[k-1];
					Object_Data->Trajectory[j].y[k]=Object_Data->Trajectory[j].y[k-1];
				}

				Object_Data->Object_Size[j] = Observe_Data->Area[i];
				
				Object_Data->SD[j].Insert(Observe_Data->SD[i].xchain,Observe_Data->SD[i].ychain,Observe_Data->SD[i].num);
			
				Object_Data->Object_info[j]=Observe_Data->Object_info[i];				

				Object_Data->Trajectory[j].x[0]=(Observe_Data->Object_info[i].right-Observe_Data->Object_info[i].left)/2+Observe_Data->Object_info[i].left;
				Object_Data->Trajectory[j].y[0]=Observe_Data->Object_info[i].bottom;
		
				Object_Data->Object_MassX[j]=Observe_Data->Object_MassX[i];
				Object_Data->Object_MassY[j]=Observe_Data->Object_MassY[i];

				Object_Data->Angle[j]=Observe_Data->Angle[i];
				Object_Data->Major_len[j]=Observe_Data->Major_len[i];
				Object_Data->Minor_len[j]=Observe_Data->Minor_len[i];

				Object_Data->Object_Assign[j] = 2;
			}
		}
		if(newly == 0)
		{
			int num = Object_Data->Object_n;
			//Object_Data->SD1[num].Insert(Observe_Data->SD[AllocNum[i]].xchain,Identy_Data->SD[AllocNum[i]].ychain,Identy_Data->SD[AllocNum[i]].num);
			
			for(int k=0;k<99;k++)
			{
				Object_Data->Trajectory[num].x[k+1]=0;
				Object_Data->Trajectory[num].y[k+1]=0;
			}

 			Object_Data->Object_ID[num] = Object_Data->NewID;
			Object_Data->NewID ++;
			
			Object_Data->Object_Size[num] = Observe_Data->Area[i];				

			Object_Data->SD[num].Insert(Observe_Data->SD[i].xchain,Observe_Data->SD[i].ychain,Observe_Data->SD[i].num);
			
			Object_Data->Object_info[num]=Observe_Data->Object_info[i];				

			Object_Data->Trajectory[num].x[0]=(Observe_Data->Object_info[i].right-Observe_Data->Object_info[i].left)/2+Observe_Data->Object_info[i].left;
			Object_Data->Trajectory[num].y[0]=Observe_Data->Object_info[i].bottom;
		
			Object_Data->Object_MassX[num]=Observe_Data->Object_MassX[i];
			Object_Data->Object_MassY[num]=Observe_Data->Object_MassY[i];

			Object_Data->Angle[num]=Observe_Data->Angle[i];
			Object_Data->Major_len[num]=Observe_Data->Major_len[i];
			Object_Data->Minor_len[num]=Observe_Data->Minor_len[i];
			Object_Data->DirectionFlag[num] = 0;
			Object_Data->Object_Type[num] = 0;
			Object_Data->Object_Assign[num] = 1;
			
			
			
			Object_Data->Object_n ++;
		}
	}
}

void Kalman_Update(LPKTRPARAM Tracking_Data, KalmanParam *KalParam)
{
	int num = Tracking_Data->Object_n;
	if(num == 0)
		return;
	for(int i=0; i<num; i++)
	{
		// 관측 되지 않은 객체
		if(Tracking_Data->Object_Assign[i] == 0)
		{
			Tracking_Data->Object_Kalman[i].y_t[0] = Tracking_Data->Object_Kalman[i].x_t_plus[0];
			Tracking_Data->Object_Kalman[i].y_t[1] = Tracking_Data->Object_Kalman[i].x_t_plus[1];
			Tracking_Data->Object_Kalman[i].y_t[2] = Tracking_Data->Object_Kalman[i].x_t_plus[2];
			Tracking_Data->Object_Kalman[i].y_t[3] = Tracking_Data->Object_Kalman[i].x_t_plus[3];
			Tracking_Data->Object_Kalman[i].y_t[4] = Tracking_Data->Object_Kalman[i].x_t_plus[4];

			for(int k=98;k>0;k--)
			{
				Tracking_Data->Trajectory[i].x[k]=Tracking_Data->Trajectory[i].x[k-1];
				Tracking_Data->Trajectory[i].y[k]=Tracking_Data->Trajectory[i].y[k-1];
			}
						
			Tracking_Data->Object_Kalman[i].ntimes_propagated ++;
			
		}
		// 새로운 객체
		else if(Tracking_Data->Object_Assign[i] == 1)
		{
			Tracking_Data->Object_Kalman[i].x_t_plus[0] = Tracking_Data->Trajectory[i].x[0];
			Tracking_Data->Object_Kalman[i].x_t_plus[1] = Tracking_Data->Trajectory[i].y[0];
			Tracking_Data->Object_Kalman[i].x_t_plus[2] = 0;
			Tracking_Data->Object_Kalman[i].x_t_plus[3] = 0;
			Tracking_Data->Object_Kalman[i].x_t_plus[4] = Tracking_Data->Object_Size[i];

			Tracking_Data->Object_Kalman[i].y_t[0] = Tracking_Data->Trajectory[i].x[0];
			Tracking_Data->Object_Kalman[i].y_t[1] = Tracking_Data->Trajectory[i].y[0];
			Tracking_Data->Object_Kalman[i].y_t[2] = 0;
			Tracking_Data->Object_Kalman[i].y_t[3] = 0;
			Tracking_Data->Object_Kalman[i].y_t[4] = Tracking_Data->Object_Size[i];

			Tracking_Data->Object_Kalman[i].expected_y_t_plus[0] = Tracking_Data->Trajectory[i].x[0];
			Tracking_Data->Object_Kalman[i].expected_y_t_plus[1] = Tracking_Data->Trajectory[i].y[0];
			Tracking_Data->Object_Kalman[i].expected_y_t_plus[2] = 0;
			Tracking_Data->Object_Kalman[i].expected_y_t_plus[3] = 0;
			Tracking_Data->Object_Kalman[i].expected_y_t_plus[4] = Tracking_Data->Object_Size[i];

			for(int j= 0; j<5; j++)
			{
				for(int k= 0; k<5; k++)
				{
					if(j == k)
						Tracking_Data->Object_Kalman[i].P_t_plus[j*5+k] = 1;
					else
						Tracking_Data->Object_Kalman[i].P_t_plus[j*5+k] = 0;
				}
			}
						
			Tracking_Data->DirectionFlag[i] = 0;
			Tracking_Data->Object_Type[i] = 0;

			Tracking_Data->Object_Kalman[i].ntimes_propagated = 0;
			Tracking_Data->Object_Kalman[i].ntimes_updated = 1;
		}
		// 객체 정보 유지
		else if(Tracking_Data->Object_Assign[i] == 2)
		{
			Tracking_Data->Object_Kalman[i].y_t[0] = Tracking_Data->Trajectory[i].x[0];
			Tracking_Data->Object_Kalman[i].y_t[1] = Tracking_Data->Trajectory[i].y[0];
			Tracking_Data->Object_Kalman[i].y_t[2] = Tracking_Data->Trajectory[i].x[0] -Tracking_Data->Trajectory[i].x[1];
			Tracking_Data->Object_Kalman[i].y_t[3] = Tracking_Data->Trajectory[i].y[0] -Tracking_Data->Trajectory[i].y[1];
			Tracking_Data->Object_Kalman[i].y_t[4] = Tracking_Data->Object_Size[i];

			Tracking_Data->Object_Kalman[i].ntimes_propagated = 0;
			Tracking_Data->Object_Kalman[i].ntimes_updated = Tracking_Data->Object_Kalman[i].ntimes_updated ++;
		}

		KalParam->Measure_Update(Tracking_Data->Object_Kalman[i].y_t, Tracking_Data->Object_Kalman[i].x_t_plus, Tracking_Data->Object_Kalman[i].P_t_plus, Tracking_Data->Object_Kalman[i].x_t, Tracking_Data->Object_Kalman[i].P_t);
		Tracking_Data->Trajectory[i].x[0] = Tracking_Data->Object_Kalman[i].x_t[0];
		Tracking_Data->Trajectory[i].y[0] = Tracking_Data->Object_Kalman[i].x_t[1];

		if(Tracking_Data->Object_Assign[i] == 0)
		{
			Tracking_Data->Object_MassX[i] = Tracking_Data->Trajectory[i].x[0] 
											+ (Tracking_Data->Object_MassX[i] - Tracking_Data->Trajectory[i].x[1]);
			Tracking_Data->Object_MassY[i] = Tracking_Data->Trajectory[i].y[0] 
											+ (Tracking_Data->Object_MassY[i] - Tracking_Data->Trajectory[i].y[1]);
		}
	}

}


int lap(int dim, int **assigncost, int *rowsol, int *colsol, int *u, int *v)
// input:
// dim        - problem size
// assigncost - cost matrix

// output:
// rowsol     - column assigned to row in solution
// colsol     - row assigned to column in solution
// u          - dual variables, row reduction numbers
// v          - dual variables, column reduction numbers
{
  boolean unassignedfound;
  int  i, imin, numfree = 0, prvnumfree, f, i0, k, freerow, *pred, *free;
  int  j, j1, j2, endofpath, last, low, up, *collist, *matches;
  int min, h, umin, usubmin, v2, *d;
  int BIG = 100000;
  free = new int[dim];       // list of unassigned rows.
  collist = new int[dim];    // list of columns to be scanned in various ways.
  matches = new int[dim];    // counts how many times a row could be assigned.
  d = new int[dim];         // 'cost-distance' in augmenting path calculation.
  pred = new int[dim];       // row-predecessor of column in augmenting/alternating path.

  // init how many times a row will be assigned in the column reduction.
  for (i = 0; i < dim; i++)  
    matches[i] = 0;

  // COLUMN REDUCTION 
  for (j = dim-1; j >= 0; j--)    // reverse order gives better results.
  {
    // find minimum cost over rows.
    min = assigncost[0][j]; 
    imin = 0;
    for (i = 1; i < dim; i++)  
      if (assigncost[i][j] < min) 
      { 
        min = assigncost[i][j]; 
        imin = i;
      }
    v[j] = min; 

    if (++matches[imin] == 1) 
    { 
      // init assignment if minimum row assigned for first time.
      rowsol[imin] = j; 
      colsol[j] = imin; 
    }
    else
      colsol[j] = -1;        // row already assigned, column not assigned.
  }

  // REDUCTION TRANSFER
  for (i = 0; i < dim; i++) 
    if (matches[i] == 0)     // fill list of unassigned 'free' rows.
      free[numfree++] = i;
    else
      if (matches[i] == 1)   // transfer reduction from rows that are assigned once.
      {
        j1 = rowsol[i]; 
        min = BIG;
        for (j = 0; j < dim; j++)  
          if (j != j1)
            if (assigncost[i][j] - v[j] < min) 
              min = assigncost[i][j] - v[j];
        v[j1] = v[j1] - min;
      }

  // AUGMENTING ROW REDUCTION 
  int loopcnt = 0;           // do-loop to be done twice.
  do
  {
    loopcnt++;

    // scan all free rows.
    // in some cases, a free row may be replaced with another one to be scanned next.
    k = 0; 
    prvnumfree = numfree; 
    numfree = 0;             // start list of rows still free after augmenting row reduction.
    while (k < prvnumfree)
    {
      i = free[k]; 
      k++;

      // find minimum and second minimum reduced cost over columns.
      umin = assigncost[i][0] - v[0]; 
      j1 = 0; 
      usubmin = BIG;
      for (j = 1; j < dim; j++) 
      {
        h = assigncost[i][j] - v[j];
        if (h < usubmin)
          if (h >= umin) 
          { 
            usubmin = h; 
            j2 = j;
          }
          else 
          { 
            usubmin = umin; 
            umin = h; 
            j2 = j1; 
            j1 = j;
          }
      }

      i0 = colsol[j1];
      if (umin < usubmin) 
        // change the reduction of the minimum column to increase the minimum
        // reduced cost in the row to the subminimum.
        v[j1] = v[j1] - (usubmin - umin);
      else                   // minimum and subminimum equal.
        if (i0 >= 0)         // minimum column j1 is assigned.
        { 
          // swap columns j1 and j2, as j2 may be unassigned.
          j1 = j2; 
          i0 = colsol[j2];
        }

      // (re-)assign i to j1, possibly de-assigning an i0.
      rowsol[i] = j1; 
      colsol[j1] = i;

      if (i0 >= 0)           // minimum column j1 assigned earlier.
        if (umin < usubmin) 
          // put in current k, and go back to that k.
          // continue augmenting path i - j1 with i0.
          free[--k] = i0; 
        else 
          // no further augmenting reduction possible.
          // store i0 in list of free rows for next phase.
          free[numfree++] = i0; 
    }
  }
  while (loopcnt < 2);       // repeat once.

  // AUGMENT SOLUTION for each free row.
  for (f = 0; f < numfree; f++) 
  {
    freerow = free[f];       // start row of augmenting path.

    // Dijkstra shortest path algorithm.
    // runs until unassigned column added to shortest path tree.
    for (j = 0; j < dim; j++)  
    { 
      d[j] = assigncost[freerow][j] - v[j]; 
      pred[j] = freerow;
      collist[j] = j;        // init column list.
    }

    low = 0; // columns in 0..low-1 are ready, now none.
    up = 0;  // columns in low..up-1 are to be scanned for current minimum, now none.
             // columns in up..dim-1 are to be considered later to find new minimum, 
             // at this stage the list simply contains all columns 
    unassignedfound = FALSE;
    do
    {
      if (up == low)         // no more columns to be scanned for current minimum.
      {
        last = low - 1; 

        // scan columns for up..dim-1 to find all indices for which new minimum occurs.
        // store these indices between low..up-1 (increasing up). 
        min = d[collist[up++]]; 
        for (k = up; k < dim; k++) 
        {
          j = collist[k]; 
          h = d[j];
          if (h <= min)
          {
            if (h < min)     // new minimum.
            { 
              up = low;      // restart list at index low.
              min = h;
            }
            // new index with same minimum, put on undex up, and extend list.
            collist[k] = collist[up]; 
            collist[up++] = j; 
          }
        }

        // check if any of the minimum columns happens to be unassigned.
        // if so, we have an augmenting path right away.
        for (k = low; k < up; k++) 
          if (colsol[collist[k]] < 0) 
          {
            endofpath = collist[k];
            unassignedfound = TRUE;
            break;
          }
      }

      if (!unassignedfound) 
      {
        // update 'distances' between freerow and all unscanned columns, via next scanned column.
        j1 = collist[low]; 
        low++; 
        i = colsol[j1]; 
        h = assigncost[i][j1] - v[j1] - min;

        for (k = up; k < dim; k++) 
        {
          j = collist[k]; 
          v2 = assigncost[i][j] - v[j] - h;
          if (v2 < d[j])
          {
            pred[j] = i;
            if (v2 == min)   // new column found at same minimum value
              if (colsol[j] < 0) 
              {
                // if unassigned, shortest augmenting path is complete.
                endofpath = j;
                unassignedfound = TRUE;
                break;
              }
              // else add to list to be scanned right away.
              else 
              { 
                collist[k] = collist[up]; 
                collist[up++] = j; 
              }
            d[j] = v2;
          }
        }
      } 
    }
    while (!unassignedfound);

    // update column prices.
    for (k = 0; k <= last; k++)  
    { 
      j1 = collist[k]; 
      v[j1] = v[j1] + d[j1] - min;
    }

    // reset row and column assignments along the alternating path.
    do
    {
      i = pred[endofpath]; 
      colsol[endofpath] = i; 
      j1 = endofpath; 
      endofpath = rowsol[i]; 
      rowsol[i] = j1;
    }
    while (i != freerow);
  }

  // calculate optimal cost.
  int lapcost = 0;
  for (i = 0; i < dim; i++)  
  {
    j = rowsol[i];
    u[i] = assigncost[i][j] - v[j];
    lapcost = lapcost + assigncost[i][j]; 
  }

  // free reserved memory.
  delete[] pred;
  delete[] free;
  delete[] collist;
  delete[] matches;
  delete[] d;

  return lapcost;
}


void Event_Crossing(LPKTRPARAM Object_Data, CPoint * Line, bool Crossing_On)
{
	int Object_n = Object_Data->Object_n;

	double x1;
	double x2;
	double x3 = Line[0].x;
	double x4 = Line[1].x;
	double y1;
	double y2;
	double y3 = Line[0].y;
	double y4 = Line[1].y;

	for(int i=0; i<Object_n; i++)
	{
		int length = 0;
		for(int j=0; j<100; j++)
		{
			if(Object_Data->Trajectory[i].x[j] < 1)
			{
				break;
			}
			length ++;
		}
		if(length<10 )//|| Object_Data->DirectionFlag[i] != 0)
		{
			continue;
		}
		
		x1 = Object_Data->Trajectory[i].x[length-1];
		y1 = Object_Data->Trajectory[i].y[length-1];
		x2 = Object_Data->Trajectory[i].x[0];
		y2 = Object_Data->Trajectory[i].y[0];

		bool extendA = FALSE;
		bool extendB = FALSE;

		double denominator = ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
		if (denominator == 0) // lines are parallel
		{
			continue;
		}
		double numerator_ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3));
		double numerator_ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3));
		double ua = numerator_ua / denominator;
		double ub = numerator_ub / denominator;
		// if a line is not extended then ua (or ub) must be between 0 and 1
		if (extendA == false)
		{
			if (ua < 0 || ua > 1)
				continue;
		}
		if (extendB == false)
		{
			if (ub < 0 || ub > 1)
				continue;
		}
		if (extendA || extendB) // no need to chck range of ua and ub if check is one on lines 
		{	
			Object_Data->DirectionFlag[i] = Direction(Line,CPoint(x2,y2));			
			continue;
		}
		if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1)
		{
			Object_Data->DirectionFlag[i] = Direction(Line,CPoint(x2,y2));
			continue;
		}
		continue;
	}
}

float LineAngle(CPoint p1, CPoint p2)
{
	// R : Radius의 준말
	if (p1.x == p2.x)
	{
		if (p1.y > p2.y)	// 270도
			return PI * 6/4;
		if (p1.y < p2.y)	// 90도
			return PI/2;
		return 0;
	}

	// first find angle A
	double adjacent = p2.x - p1.x;	// 밑변
	double opposite = p2.y - p1.y;	// 높이
	// find angle A
	double A = atan(opposite / adjacent);
	if (adjacent < 0)			// 2nd and 3rd quadrant
		A += PI;			// + 180
	if (adjacent > 0 && opposite < 0) // 4th quadrant
		A += PI * 2;		// +360
	
	return A;
}

int Direction(CPoint * Line, CPoint p1)
{
	int nDirection=0;
	double AngleLine = LineAngle(Line[0], Line[1]);
	double AngleOrbit = LineAngle(Line[0], p1);

	double sAngle, eAngle;
	sAngle = AngleLine;
	eAngle = AngleLine+PI;

	if( AngleOrbit > sAngle && AngleOrbit < eAngle ){
		nDirection=2;	// 역방향
	}
	else if(AngleOrbit+2*PI > sAngle && AngleOrbit+2*PI < eAngle){
		nDirection=2;	// 역방향
	}
	else if(AngleLine == AngleOrbit || AngleLine+PI == AngleOrbit){
		nDirection=0;
	}
	else{
		nDirection=1;
	}
	return nDirection;
}

void RecognizeVehicle(ImageData * ImgData, LPKTRPARAM Object_Data)
{
	// Vehicle detection
	CvHaarClassifierCascade *cascade;
	CvMemStorage            *storage;
	
	CString str;
	str.Format("cars3.xml");
	cascade = (CvHaarClassifierCascade*) cvLoad(str, 0, 0, 0);
	storage = cvCreateMemStorage(0);

	CvSize img_size = cvGetSize(ImgData->ImgGray);
	CvSeq *object = cvHaarDetectObjects(
		ImgData->ImgGray,
		cascade,
		storage,
		1.1, //1.1,//1.5, //-------------------SCALE FACTOR
		2, //2        //------------------MIN NEIGHBOURS
		0, //CV_HAAR_DO_CANNY_PRUNING
		cvSize(10,10),//cvSize( 30,30), // ------MINSIZE
		cvSize(100,100)//cvSize(640,480)  //---------MAXSIZE
	);
	for(int i = 0 ; i < ( object ? object->total : 0 ) ; i++)
	{
		CvRect *r = (CvRect*)cvGetSeqElem(object, i);
		cvRectangle(ImgData->ImgOverlay2,
		cvPoint(r->x, r->y),
		cvPoint(r->x + r->width, r->y + r->height),
		CV_RGB(255, 0, 0), 2, 8, 0);
	}

	int num = Object_Data->Object_n;
	if(num == 0)
	{
		cvClearSeq(object);
		cvReleaseMemStorage(&storage);
		cvReleaseHaarClassifierCascade( &cascade );
		return;
	}

	for(int i=0; i<num; i++)
	{	
		if(Object_Data->Object_Kalman[i].ntimes_propagated > 0)
			continue;
		for(int j = 0 ; j < ( object ? object->total : 0 ) ; j++)
		{
			double area, ratioA, ratioB;
			CvRect *r = (CvRect*)cvGetSeqElem(object, j);
			CRect A(Object_Data->Object_info[i].left,Object_Data->Object_info[i].top,Object_Data->Object_info[i].right, Object_Data->Object_info[i].bottom);
			CRect B(r->x,r->y,r->x + r->width, r->y + r->height);
		
			TwoRectOcclusion(A, B, &area, &ratioA, &ratioB);
			
			
			if(ratioA > 0.8 && ratioB > 0.3)
			{
				Object_Data->Object_Type[i] = 1;
				
			}
			
		}
	}
	cvClearSeq(object);
	cvReleaseMemStorage(&storage);
	cvReleaseHaarClassifierCascade( &cascade );
}

void RecognizeHumanHoG(ImageData * ImgData, LPKTRPARAM Object_Data)
{
	// Vehicle detection
	string cascadeName = "hogcascade_pedestrians.xml";
	CascadeClassifier detector;
	if( !detector.load( cascadeName ) )
	{
		return;
	}

	// parameters
	int gr_thr = 6;
	double scale_step = 1.1;
	Size min_obj_sz(10,20);
	Size max_obj_sz(100,200);

	// detect
	vector<Rect> found;
	detector.detectMultiScale(ImgData->ImgOrigin, found, scale_step, gr_thr, 0, min_obj_sz, max_obj_sz);


	// draw results (bounding boxes)
	for(int i=0; i<(int)found.size(); i++)
		cvRectangle(ImgData->ImgOverlay, cvPoint(found[i].x, found[i].y), 
		cvPoint(found[i].x+found[i].width, found[i].y+found[i].height), CV_RGB(0,0,255), 1, 8, 0);
		
	cvNamedWindow("a");
	cvShowImage("a",ImgData->ImgOverlay);


	//CvSize img_size = cvGetSize(ImgData->ImgGray);
	//CvSeq *object = cvHaarDetectObjects(
	//	ImgData->ImgGray,
	//	cascade,
	//	storage,
	//	1.2, //1.1,//1.5, //-------------------SCALE FACTOR
	//	2, //2        //------------------MIN NEIGHBOURS
	//	0, //CV_HAAR_DO_CANNY_PRUNING
	//	cvSize(10,10),//cvSize( 30,30), // ------MINSIZE
	//	img_size //cvSize(70,70)//cvSize(640,480)  //---------MAXSIZE
	//);
	//for(int i = 0 ; i < ( object ? object->total : 0 ) ; i++)
	//{
	//	CvRect *r = (CvRect*)cvGetSeqElem(object, i);
	//	cvRectangle(ImgData->ImgOverlay,
	//	cvPoint(r->x, r->y),
	//	cvPoint(r->x + r->width, r->y + r->height),
	//	CV_RGB(255, 0, 0), 2, 8, 0);
	//}

	//int num = Object_Data->Object_n;
	//if(num == 0)
	//	return;
	//for(int i=0; i<num; i++)
	//{	
	//	for(int j = 0 ; j < ( object ? object->total : 0 ) ; j++)
	//	{
	//		double area, ratioA, ratioB;
	//		CvRect *r = (CvRect*)cvGetSeqElem(object, j);
	//		CRect A(Object_Data->Object_info[i].left,Object_Data->Object_info[i].top,Object_Data->Object_info[i].right, Object_Data->Object_info[i].bottom);
	//		CRect B(r->x,r->y,r->x + r->width, r->y + r->height);
	//		
	//		TwoRectOcclusion(A, B, &area, &ratioA, &ratioB);

	//		if(ratioA > 0.5 && ratioB > 0.5)
	//			Object_Data->Object_Type[i] = 1;
	//	}
	//}
}

void RecognizeHumanHaar(ImageData * ImgData, LPKTRPARAM Object_Data)
{
	// Human detection
	CvHaarClassifierCascade *cascade;
	CvMemStorage            *storage;
	
	CString str;
	str.Format("haarcascade_fullbody.xml");
	cascade = (CvHaarClassifierCascade*) cvLoad(str, 0, 0, 0);
	storage = cvCreateMemStorage(0);

	CvSize img_size = cvGetSize(ImgData->ImgGray);
	CvSeq *object = cvHaarDetectObjects(
		ImgData->ImgGray,
		cascade,
		storage,
		1.1, //1.1,//1.5, //-------------------SCALE FACTOR
		2, //2        //------------------MIN NEIGHBOURS
		0, //CV_HAAR_DO_CANNY_PRUNING
		cvSize(10,20),//cvSize( 30,30), // ------MINSIZE
		cvSize(40,80)//cvSize(640,480)  //---------MAXSIZE
	);

	for(int i = 0 ; i < ( object ? object->total : 0 ) ; i++)
	{
		CvRect *r = (CvRect*)cvGetSeqElem(object, i);
		cvRectangle(ImgData->ImgOverlay2,
		cvPoint(r->x, r->y),
		cvPoint(r->x + r->width, r->y + r->height),
		CV_RGB(0, 0, 255), 2, 8, 0);
	}

	int num = Object_Data->Object_n;
	if(num == 0)
	{
		cvClearSeq(object);
		cvReleaseMemStorage(&storage);
		cvReleaseHaarClassifierCascade( &cascade );
		return;
	}
	for(int i=0; i<num; i++)
	{	
		if(Object_Data->Object_Kalman[i].ntimes_propagated > 0)
			continue;
		for(int j = 0 ; j < ( object ? object->total : 0 ) ; j++)
		{
			double area, ratioA, ratioB;
			CvRect *r = (CvRect*)cvGetSeqElem(object, j);
			CRect A(Object_Data->Object_info[i].left,Object_Data->Object_info[i].top,Object_Data->Object_info[i].right, Object_Data->Object_info[i].bottom);
			CRect B(r->x,r->y,r->x + r->width, r->y + r->height);
		
			TwoRectOcclusion(A, B, &area, &ratioA, &ratioB);
			
			
			if(ratioA > 0.8 && ratioB > 0.3)
			{
				Object_Data->Object_Type[i] = 2;
				
			}
			
		}
	}
	cvClearSeq(object);
	cvReleaseMemStorage(&storage);
	cvReleaseHaarClassifierCascade( &cascade );
}


void TwoRectOcclusion(CRect A, CRect B, double * Area, double * RatioA, double * RatioB)
{
	CRect region;
	double AreaA = (A.right - A.left)*(A.bottom - A.top);
	double AreaB = (B.right - B.left)*(B.bottom - B.top);
	if(A.left >= B.right || A.right <= B.left || A.top >= B.bottom || A.bottom <= B.top)
	{
		*Area = 0;
		*RatioA = 0;
		*RatioB = 0;
	}
	else
	{
		if(A.left > B.left)
			region.left = A.left;
		else
			region.left = B.left;

		if(A.top > B.top)
			region.top = A.top;
		else
			region.top = B.top;

		if(A.right > B.right)
			region.right = B.right;
		else
			region.right = A.right;

		if(A.bottom > B.bottom)
			region.bottom = B.bottom;
		else
			region.bottom = A.bottom;

		*Area = (region.right - region.left)*(region.bottom - region.top);

		*RatioA = *Area/AreaA;
		*RatioB = *Area/AreaB;
	}	
}