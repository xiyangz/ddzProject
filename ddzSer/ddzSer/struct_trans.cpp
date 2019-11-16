#include "pch.h"
#include "struct_trans.h"

trans_user_info::trans_user_info(const team_user_info& tui)
{
	id = tui.id;
	StrCpy(account, tui.account);
	StrCpy(nickname, tui.nickname);
	player_isPrepare = tui.player_isPrepare;
}

trans_user_info trans_user_info::operator=(const team_user_info& tui)
{
	return trans_user_info(tui);
}

trans_user_info::trans_user_info(const trans_user_info& tui)
{
	StrCpy(account, tui.account);
	StrCpy(nickname, tui.nickname);
	id = tui.id;
	player_isPrepare = tui.player_isPrepare;
}

trans_user_info trans_user_info::operator=(const trans_user_info& tui)
{
	return trans_user_info(tui);
}
