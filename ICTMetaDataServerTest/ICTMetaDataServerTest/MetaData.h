enum TECHNOLOGYTYPE
{
	DETECT1 = 0,			//전역 추적 모듈
	DETECT2 = 1,			//지역 추적 모듈
	FACE = 2,				//얼굴 검출 모듈
	CAR_PEOPLE = 3,			//차량 및 보행자 검출 모듈
	SOCIALFORCE = 4			//이상 상황 검출 모듈
};

struct OBJECT_DATA
{
	int ObjectID;			//시스템 내 검출된 객체의 고유 아이디
	int x;					//해당 객체의 프레임 내 x 좌표
	int y;					//해당 객체의 프레임 내 y 좌표
	int Width;				//객체 검출영역의 넓이
	int Height;				//객체 검출영역의 높이
	int Type;				//해당 객체의 타입
	int DirectionFlag;		//예비
	int ntimes_updated;		//예비
	int Rank[3];			//얼굴 인식 결과
};

struct ICT_META_DATA
{
	int CameraNumber;				//카메라번호
	int FrameNumber;				//프레임 번호(RTSP 타임스템프)
	int TechnologyType;				//기술명
	int NumberOfObject = 0;			//검출된 객체의 갯수
	OBJECT_DATA pObjectData[10];	//검출된 객체의 데이타
};

extern "C" bool PASCAL EXPORT ClientInit(char* vIP, int vPort);			//클라이언트 소켓 초기화
extern "C" void PASCAL EXPORT ClientClose();							//클라이언트 소켓 종료
extern "C" bool __stdcall SendMetaData(ICT_META_DATA* pICTMetaData);	//검출된 메타데이타 전송

#ifndef DLLDEMO_EXPORT
#pragma comment(lib, "ICTMetaData")
#endif