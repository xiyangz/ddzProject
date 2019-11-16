#pragma once



class TeamManager
{
public:
	TeamManager();
	~TeamManager();
public:
	team_wrapper* CreateTeam(team_user_info creater, TCHAR* team_name);
	bool CloseTeam(team_wrapper*& pTeam);//ֻ����ر� �� �ж��Ƿ��Ƿ����Ĺ����sock����
	team_wrapper* JoinTeam(int team_id, sLoginSock* psock, team_user_info& joiner);//����ʧ�ܻ᷵�� nullptr
	bool QuitTeam(team_wrapper*& pTeam, int joiner_id);//����˳�����  
	bool ChangePrepare(team_wrapper* pTeam, int player_id, bool is_prepare);

private:
	team_wrapper* team_head = nullptr;
	team_wrapper* team_tail = nullptr;
	int team_size = 0;

public:
	void initTeamList();
	void endTeamList(); //�������ʱ��Ҫ����
	inline int getTeamSize() { return team_size; };
	void getAllTeamData(team** teamArray, int ArrayLength);
	bool inserttailTeam(team_wrapper* const pTeam);
	bool removeTeam(team_wrapper* pTeam);
	team_wrapper* getTeamByTeamId(int team_id);
};

extern TeamManager theTeamManager;
extern TeamManager theRunTeamManager;
