#pragma once
extern class sLoginSock;

struct team_user_info //原user_ser
{
	int id;
	TCHAR account[ACCOUNT_MAXSIZE + 1];
	TCHAR nickname[NICKNAME_MAXSIZE + 1];
	TCHAR key[KEY_SIZE + 1];
	bool player_isPrepare;
	sLoginSock* pLoginSock;//玩家服务套接字
	sRunSock* pRunSock;//
	team_user_info() { player_isPrepare = false; pLoginSock = nullptr; pRunSock = nullptr; };
	//待补充
};