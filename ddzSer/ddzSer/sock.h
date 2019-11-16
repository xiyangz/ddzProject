#pragma once
#include<memory>
#include"struct_login.h"
extern struct team_wrapper;
extern class DdzRun;
extern struct team_user_info;

class base_sock
{
public:
	base_sock(SOCKET sock, CString ip_str, UINT port)
	{
		this->sock = sock; 
		this->ip_str = ip_str; 
		this->port = port;
	}
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = MSG_WAITALL);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnClose() = 0;
	SOCKET sock;
	CString ip_str;
	UINT port;
};
class sLoginSock : public base_sock
{

public:
	sLoginSock(SOCKET lsock, CString ip_str, UINT port):base_sock(lsock,ip_str,port) {}
	void OnReceive();
	void OnClose();
	void Close();
	void SendPackHeader(login_status stat, int type, return_type sub_type, int data_length);
	bool isTeamLeader = false; //是否是房主  只有房主才能关闭房间  修改房间名称 等等
	team_wrapper* tnode = nullptr;//自己的房间
	team_user_info tui;
	bool isFirstDisconnect = true;
	bool isLoginSuc = false;
private:
	void OnNormLogin();
	void OnFaceLogin();
	register_user_info rui;
	std::unique_ptr<char> imageStr = nullptr;
	static size_t loginFaceDetectCallback(void* ptr, size_t size, size_t nmemb, void* stream);
	static size_t loginFaceSearchCallback(void* ptr, size_t size, size_t nmemb, void* stream);
	static size_t registerFaceDetectCallback(void* ptr, size_t size, size_t nmemb, void* stream);
	static size_t registerFaceSearchCallback(void* ptr, size_t size, size_t nmemb, void* stream);
	static size_t registerAddFaceCallback(void* ptr, size_t size, size_t nmemb, void* stream);
	void OnRegister();
	void OnData();
	void OnPyData();
	void OnCreate();
	void OnJoin(int team_id);
	void OnQuit();
	void OnEnReady();
	void OnUnReady();
	void OnTmData();
};

class sRunSock : public base_sock
{
public:
	sRunSock(SOCKET rsock, CString ip_str, UINT port, HANDLE handle) :base_sock(rsock, ip_str, port) { runHandle = handle; }
	void OnReceive();
	void OnClose();
	void Close();
	void SendrunHeader(run_status rs, int sq, int type);
	void OnCheck();
	void BeginDdzRun(team_wrapper* ptw);
public:
	bool isCheckRight = false;
	DdzRun* pRun = nullptr;
	int sequence;
	int player_id = -1;
	team_wrapper* pRunTeam = nullptr;
	HANDLE runHandle = NULL;
};
