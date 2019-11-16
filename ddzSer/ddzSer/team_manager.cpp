#include"pch.h"
#include "struct_common.h"
#include "team_manager.h"


TeamManager theTeamManager;
TeamManager theRunTeamManager;

TeamManager::TeamManager()
{
	initTeamList();
}

TeamManager::~TeamManager()
{
	endTeamList();
}

team_wrapper* TeamManager::CreateTeam(team_user_info creater, TCHAR* team_name)
{
	if (team_size != 0)
	{
		team_wrapper* q = team_head;
		while (q != nullptr)
		{
			if (q->pTeam != nullptr)
			{
				if (q->pTeam->team_id == creater.id)
					return nullptr;
			}
			q = q->next;
		}
	}
	team* p = new team;
	p->team_id = creater.id;
	p->isRun = false;
	StrCpy(p->player[0].nickname, creater.nickname);
	StrCpy(p->player[0].account, creater.account);
	StrCpy(p->player[0].key, creater.key);

	p->player[0].player_isPrepare = false;
	p->player[0].pRunSock = nullptr;
	StrCpy(p->player[1].nickname, TEXT("NULL"));
	StrCpy(p->player[1].account, TEXT("NULL"));
	p->player[1].player_isPrepare = false;
	p->player[1].pRunSock = nullptr;
	StrCpy(p->player[2].nickname, TEXT("NULL"));
	StrCpy(p->player[2].account, TEXT("NULL"));
	p->player[2].player_isPrepare = false;
	p->player[2].pRunSock = nullptr;

	p->player[0].id = creater.id;
	p->player[1].id = -1;
	p->player[2].id = -1;
	p->player_num = 1;
	StrCpy(p->team_name, team_name);

	team_wrapper* pteam = new team_wrapper;
	pteam->pTeam = p;
	pteam->front = nullptr;
	pteam->next = nullptr;

	inserttailTeam(pteam);
	return pteam;
}

bool TeamManager::CloseTeam(team_wrapper*& pteam)
{
	if (team_size != 0)
	{
		if (removeTeam(pteam))
		{
			if (pteam->pTeam != nullptr)
			{
				delete pteam->pTeam;
				pteam->pTeam = nullptr;
			}
			delete pteam;
			pteam = nullptr;
			return true;
		}

	}

	return false;
}

team_wrapper* TeamManager::JoinTeam(int team_id, sLoginSock* psock, team_user_info& joiner)
{
	if (team_size != 0)
	{
		team_wrapper* p = team_head->next;
		while (p != nullptr)
		{
			if (p->pTeam->team_id == team_id)
			{
				if (p->pTeam->player_num >= PLAYER_NUMBER)
					return nullptr;
				else
				{
					p->pTeam->player[p->pTeam->player_num].pLoginSock = psock;
					p->pTeam->player[p->pTeam->player_num].pRunSock = nullptr;
					StrCpy(p->pTeam->player[p->pTeam->player_num].account, joiner.account);
					StrCpy(p->pTeam->player[p->pTeam->player_num].nickname, joiner.nickname);
					StrCpy(p->pTeam->player[p->pTeam->player_num].key, joiner.key);
					p->pTeam->player[p->pTeam->player_num].id = joiner.id;
					p->pTeam->player[p->pTeam->player_num].player_isPrepare = false;
					p->pTeam->player_num++;
					return p;
				}
			}
			p = p->next;
		}
	}
	return nullptr;
}

bool TeamManager::QuitTeam(team_wrapper*& pteam, int joiner_id)
{
	if (team_size != 0 && (pteam->pTeam != nullptr))
	{
		//房间还有其他人
		if (pteam->pTeam->player_num > 1)
		{
			for (int i = 0; i < pteam->pTeam->player_num; i++)
			{
				if (pteam->pTeam->player[i].id == joiner_id)
				{
					//不是最后一个人  需要把后面的人放到前面
					if (i != (pteam->pTeam->player_num - 1))
					{
						for (; i < pteam->pTeam->player_num - 1; i++)
						{
							StrCpy(pteam->pTeam->player[i].account, pteam->pTeam->player[i + 1].account);
							StrCpy(pteam->pTeam->player[i].nickname, pteam->pTeam->player[i + 1].nickname);
							pteam->pTeam->player[i].id = pteam->pTeam->player[i + 1].id;
							pteam->pTeam->player[i].pLoginSock = pteam->pTeam->player[i + 1].pLoginSock;
						}
						//退出房间的是房主
						if (joiner_id == pteam->pTeam->team_id)
							pteam->pTeam->team_id = pteam->pTeam->player[0].id;
					}
					pteam->pTeam->player_num--;
					return true;
				}
			}

		}
		//房间只有一个人
		else if (pteam->pTeam->player[0].id == joiner_id)
		{
				return CloseTeam(pteam);
		}
	}
	return false;
}

bool TeamManager::ChangePrepare(team_wrapper* pteam, int player_id, bool is_prepare)
{
	for (int i = 0; i < pteam->pTeam->player_num; i++)
	{
		if (pteam->pTeam->player[i].id == player_id)
		{
			pteam->pTeam->player[i].player_isPrepare = is_prepare;
			return true;
		}
	}
	return false;
}

void TeamManager::initTeamList()
{
	team_head = new team_wrapper;
	team_head->next = nullptr;
	team_head->front = nullptr;
	team_head->pTeam = nullptr;

	team_tail = team_head;

	team_size = 0;
}

void TeamManager::endTeamList()
{
	while (team_head != nullptr)
	{
		team_wrapper* p = team_head;
		team_head = team_head->next;
		delete p;
	}
}

void TeamManager::getAllTeamData(team** teamArray, int ArrayLength)
{
	int count = team_size > ArrayLength ? ArrayLength : team_size;
	team_wrapper* p = team_head->next;
	for (int i = 0; i < count; i++)
	{
		teamArray[i] = p->pTeam;
		p = p->next;
	}
}
//需要自己new
bool TeamManager::inserttailTeam(team_wrapper* const pteam)
{
	pteam->next = nullptr;

	team_tail->next = pteam;
	pteam->front = team_tail;

	team_tail = pteam;

	team_size++;

	return true;
}

//需要自己delete
bool TeamManager::removeTeam(team_wrapper* const pteam)
{
	if (team_tail == team_head)
		return false;

	team_wrapper* p = pteam->front;
	team_wrapper* q = pteam->next;
	p->next = q;
	if (q != nullptr)
		q->front = p;
	team_size--;
	if (team_size == 0)
		team_tail = team_head;
	return true;
}

team_wrapper* TeamManager::getTeamByTeamId(int team_id)
{
	if (team_tail == team_head)
		return nullptr;
	team_wrapper* p = team_head->next;
	while (p != nullptr)
	{
		if (p->pTeam->team_id == team_id)
			return p;
		p = p->next;
	}
	return nullptr;
}

