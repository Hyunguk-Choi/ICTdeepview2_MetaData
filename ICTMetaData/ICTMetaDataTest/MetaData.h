enum TECHNOLOGYTYPE
{
	DETECT1 = 0,
	DETECT2 = 1,
	FACE = 2,
	CAR_PEOPLE = 3,
	SOCIALFORCE = 4
};

struct OBJECT_DATA
{
	int ObjectID;
	int x;
	int y;
	int Width;
	int Height;
	int Type;
	int DirectionFlag;
	int ntimes_updated;
	int Rank[3];
};

struct ICT_META_DATA
{
	int CameraNumber;
	int FrameNumber;
	int TechnologyType;
	int NumberOfObject = 0;
	OBJECT_DATA pObjectData[10];
};

extern "C" bool PASCAL EXPORT ClientInit(char* vIP, int vPort);
extern "C" void PASCAL EXPORT ClientClose();
extern "C" bool __stdcall SendMetaData(ICT_META_DATA* pICTMetaData);

#ifndef DLLDEMO_EXPORT
#pragma comment(lib, "ICTMetaData")
#endif