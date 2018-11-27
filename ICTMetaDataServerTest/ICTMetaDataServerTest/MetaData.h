enum TECHNOLOGYTYPE
{
	DETECT1 = 0,			//���� ���� ���
	DETECT2 = 1,			//���� ���� ���
	FACE = 2,				//�� ���� ���
	CAR_PEOPLE = 3,			//���� �� ������ ���� ���
	SOCIALFORCE = 4			//�̻� ��Ȳ ���� ���
};

struct OBJECT_DATA
{
	int ObjectID;			//�ý��� �� ����� ��ü�� ���� ���̵�
	int x;					//�ش� ��ü�� ������ �� x ��ǥ
	int y;					//�ش� ��ü�� ������ �� y ��ǥ
	int Width;				//��ü ���⿵���� ����
	int Height;				//��ü ���⿵���� ����
	int Type;				//�ش� ��ü�� Ÿ��
	int DirectionFlag;		//����
	int ntimes_updated;		//����
	int Rank[3];			//�� �ν� ���
};

struct ICT_META_DATA
{
	int CameraNumber;				//ī�޶��ȣ
	int FrameNumber;				//������ ��ȣ(RTSP Ÿ�ӽ�����)
	int TechnologyType;				//�����
	int NumberOfObject = 0;			//����� ��ü�� ����
	OBJECT_DATA pObjectData[10];	//����� ��ü�� ����Ÿ
};

extern "C" bool PASCAL EXPORT ClientInit(char* vIP, int vPort);			//Ŭ���̾�Ʈ ���� �ʱ�ȭ
extern "C" void PASCAL EXPORT ClientClose();							//Ŭ���̾�Ʈ ���� ����
extern "C" bool __stdcall SendMetaData(ICT_META_DATA* pICTMetaData);	//����� ��Ÿ����Ÿ ����

#ifndef DLLDEMO_EXPORT
#pragma comment(lib, "ICTMetaData")
#endif