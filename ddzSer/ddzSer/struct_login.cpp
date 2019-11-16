#include "pch.h"
#include "struct_login.h"

login_user_info::login_user_info(const register_user_info& rui)
{
	StrCpy(account, rui.account);
	StrCpy(password, rui.password);
}

login_user_info login_user_info::operator=(const register_user_info& rui)
{
	return login_user_info(rui);
}

login_user_info::login_user_info(const login_user_info& lui)
{
	StrCpy(account, lui.account);
	StrCpy(password, lui.password);
}

login_user_info login_user_info::operator=(const login_user_info& lui)
{
	return login_user_info(lui);
}
