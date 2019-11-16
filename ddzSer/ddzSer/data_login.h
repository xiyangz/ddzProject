#pragma once


enum login_status
{
	READY,//准备阶段
	JOIN, //组队阶段
	ALL   //所有阶段
};



//数据包头的类型码
enum class return_type
{
	NOTHING,  //空
	SUCCESS, //成功
	FAILED,   //失败
	ACCOUNT_EXISTED, //账号存在
	INPUT_ERROR,//输入有误
};

enum class pk_type_ready
{
	NORM_LOGIN, //账号密码登录
	FACE_LOGIN, //人脸登录
	REGISTER,  //注册
	UPLOADIMG,  //上传照片
};
enum class pk_type_join
{
	DATA,   //房间信息
	PYDATA, //玩家信息
	TMDATA, //队伍信息
	CREATE, //创建房间
	JOIN,   //加入
	QUIT,   //退出房间
	ENREDY, //准备
	UNREDY, //取消准备  
	BEGIN,  //开始游戏
};

struct packHeader
{
	login_status stat;
	int type;//数据类型，是心跳包还是什么
	return_type sub_type;
	int data_length;//数据段的长度，这样你还可以一并解决粘包额问题
};

extern int szLoginPort;
extern int szRunPort;

