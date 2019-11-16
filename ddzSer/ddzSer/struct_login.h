#include"data_common.h"
#pragma once
extern class sLoginSock;
extern enum class run_status;

struct register_user_info//user_reg
{
	TCHAR account[ACCOUNT_MAXSIZE + 1];
	TCHAR password[PASSWORD_MAXSIZE + 1];
	TCHAR nickname[NICKNAME_MAXSIZE + 1];
};

struct login_user_info//user_login
{
	TCHAR account[ACCOUNT_MAXSIZE + 1];
	TCHAR password[PASSWORD_MAXSIZE + 1];
	login_user_info() {};
	login_user_info(const register_user_info&);
	login_user_info operator=(const register_user_info&);
	login_user_info(const login_user_info&);
	login_user_info operator=(const login_user_info&);
};