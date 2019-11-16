#include"pch.h"
#include"struct_common.h"

trans_team::trans_team(const team& tm)
{
	team_id = tm.team_id;
	StrCpy(team_name, tm.team_name);
	player_num = tm.player_num;
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		player[i].id = tm.player[i].id;
		StrCpy(player[i].account, tm.player[i].account);
		StrCpy(player[i].nickname, tm.player[i].nickname);
		player[i].player_isPrepare = tm.player[i].player_isPrepare;
	}
}

trans_team trans_team::operator=(const team& tm)
{
	return trans_team(tm);
}

trans_team::trans_team(const trans_team& tt)
{
	team_id = tt.team_id;
	for (int i = 0; i < PLAYER_NUMBER; i++)
		player[i] = tt.player[i];
	player_num = tt.player_num;
	StrCpy(team_name, tt.team_name);
}

trans_team trans_team::operator=(const trans_team& tt)
{
	return trans_team(tt);
}
