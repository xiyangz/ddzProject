#include "struct_team.h"
#pragma once

struct trans_user_info//user_clt
{
	int id;
	TCHAR account[ACCOUNT_MAXSIZE + 1];
	TCHAR nickname[NICKNAME_MAXSIZE + 1];
	int player_isPrepare;
	//´ý²¹³ä
	trans_user_info() { };
	trans_user_info(const team_user_info&);
	trans_user_info operator=(const team_user_info&);
	trans_user_info(const trans_user_info&);
	trans_user_info operator=(const trans_user_info&);
};