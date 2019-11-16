#pragma once


enum login_status
{
	READY,//׼���׶�
	JOIN, //��ӽ׶�
	ALL   //���н׶�
};



//���ݰ�ͷ��������
enum class return_type
{
	NOTHING,  //��
	SUCCESS, //�ɹ�
	FAILED,   //ʧ��
	ACCOUNT_EXISTED, //�˺Ŵ���
	INPUT_ERROR,//��������
};

enum class pk_type_ready
{
	NORM_LOGIN, //�˺������¼
	FACE_LOGIN, //������¼
	REGISTER,  //ע��
	UPLOADIMG,  //�ϴ���Ƭ
};
enum class pk_type_join
{
	DATA,   //������Ϣ
	PYDATA, //�����Ϣ
	TMDATA, //������Ϣ
	CREATE, //��������
	JOIN,   //����
	QUIT,   //�˳�����
	ENREDY, //׼��
	UNREDY, //ȡ��׼��  
	BEGIN,  //��ʼ��Ϸ
};

struct packHeader
{
	login_status stat;
	int type;//�������ͣ�������������ʲô
	return_type sub_type;
	int data_length;//���ݶεĳ��ȣ������㻹����һ�����ճ��������
};

extern int szLoginPort;
extern int szRunPort;

