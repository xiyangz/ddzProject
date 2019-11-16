#include"struct_team.h"
#include"struct_trans.h"
#include "sock.h"
#pragma once

struct common_user_info
{
	TCHAR account[ACCOUNT_MAXSIZE + 1];
	TCHAR nickname[NICKNAME_MAXSIZE + 1];
};

//��Ϸ׼���׶λ����theTeamManager ��ʼ����theTeamManager�Ƴ�������theRunTeamManager
struct team
{
	int team_id;//ʹ�ô����ߵ�id
	TCHAR team_name[TEAMNAME_MAXSIZE + 1];
	int player_num;//���䵱ǰ����
	team_user_info player[PLAYER_NUMBER];
	bool isRun = false;
	HANDLE runThread = NULL;
};

struct trans_team
{
	int team_id;//ʹ�ô����ߵ�id
	TCHAR team_name[TEAMNAME_MAXSIZE + 1];
	int player_num;//���䵱ǰ����
	trans_user_info player[PLAYER_NUMBER];
	trans_team() {};
	trans_team(const team&);
	trans_team operator=(const team&);
	trans_team(const trans_team&);
	trans_team operator=(const trans_team&);
};


struct user_check
{
	int id;
	int pass_number;
};

struct team_wrapper
{
	team* pTeam;
	team_wrapper* front;
	team_wrapper* next;
};

struct team_ready
{
	team_wrapper* pnode;
	team_ready* front;
	team_ready* next;
};

#define defRecv {TCHAR buf[2048];Receive(buf, sizeof(buf));}

CString trim(CString& str);
void trim(TCHAR* str);
