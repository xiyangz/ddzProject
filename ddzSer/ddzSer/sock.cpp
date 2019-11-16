#include"pch.h"
#include"struct_login.h"
#include "team_manager.h"
#include "ddzrun.h"
#include "struct_common.h"
#include "struct_team.h"
#include"sock.h"
#include"face.h"
#include "include/json/json.h"


void sLoginSock::OnReceive()
{
	try
	{
		while (true)
		{
			packHeader ph;
			Receive(&ph, sizeof(packHeader));
			if (tnode != nullptr)
			{//游戏开始不准操作
				if (tnode->pTeam->isRun == true)
				{
					Sleep(1);
					continue;
				}
			}
			switch (ph.stat)
			{
			case READY:
			{
				switch (ph.type)
				{
				case (int)pk_type_ready::NORM_LOGIN:
				{
					OnNormLogin();
					break;
				}
				case(int)pk_type_ready::FACE_LOGIN:
				{
					OnFaceLogin();
					break;
				}
				case (int)pk_type_ready::REGISTER:
				{
					OnRegister();
					break;
				}
				default:
					break;
				}
			}
			break;
			case JOIN:
			{
				switch (ph.type)
				{
				case (int)pk_type_join::DATA:
				{
					OnData();
					break;
				}
				case (int)pk_type_join::PYDATA:
				{
					OnPyData();
					break;
				}
				case (int)pk_type_join::CREATE:
				{
					OnCreate();
					break;
				}
				case (int)pk_type_join::JOIN:
				{
					OnJoin(ph.data_length);
					break;
				}
				case (int)pk_type_join::QUIT:
				{
					OnQuit();
					break;
				}
				case (int)pk_type_join::ENREDY:
				{
					OnEnReady();
					break;
				}
				case (int)pk_type_join::UNREDY:
				{
					OnUnReady();
					break;
				}
				case (int)pk_type_join::TMDATA:
				{
					OnTmData();
					break;
				}
				default:
					break;
				}
			}
			break;
			case ALL:
			case (int)pk_type_norm::HEARTBEAT:
			{
				break;
			}
			default:
				break;
			}
		}
	}
	catch (CString& s)
	{
		if (s == "SOCK CLOSE!")
		{//客户端套接字关闭
			OnClose();
		}
	}
	catch (...)
	{

	}
}

void sLoginSock::OnClose()
{
	if (tnode != nullptr)
	{
		if (!tnode->pTeam->isRun)
		{
			theTeamManager.QuitTeam(tnode, tui.id);
			if (tnode != nullptr)
			{
				for (int i = 0; i < (tnode->pTeam->player_num); i++)
				{
					tnode->pTeam->player[i].pLoginSock->SendPackHeader(login_status::JOIN, (int)pk_type_join::QUIT, return_type::SUCCESS, tui.id);
					tnode->pTeam->player[i].pLoginSock->Send(&trans_team(*(tnode->pTeam)), sizeof(trans_team));
				}
			}
		}
	}
	if (isLoginSuc)
	{
		CString sql_str;
		sql_str.Format(TEXT("update online_info set online_status = %d where id=%d"), ONLINE_STATUS_OFF, tui.id);
		sql.updateSql(sql_str);
	}
}

void sLoginSock::SendPackHeader(login_status stat, int type, return_type sub_type, int data_length)
{
	packHeader ph;
	ph.stat = stat;
	ph.data_length = data_length;
	ph.sub_type = sub_type;
	ph.type = type;
	Send(&ph, sizeof(packHeader));
}

void sLoginSock::OnNormLogin()
{
	login_user_info user;
	Receive(&user, sizeof(login_user_info));
	CString sql_str;
	sql_str.Format(TEXT("select id,nickname from users where account=\'%s\' and password=\'%s\'"), user.account, user.password);
	sql.selectSql(sql_str);
	int exist = 0;
	while (!sql.m_pRecordset->adoEOF)
	{
		//保存id 昵称
		tui.id = sql.m_pRecordset->GetCollect(TEXT("id"));
		StrCpyW(tui.nickname, (_bstr_t)sql.m_pRecordset->GetCollect(TEXT("nickname")));
		sql.m_pRecordset->MoveNext();
		exist++;
		break;
	}
	if (exist == 0)
	{
		SendPackHeader(login_status::READY, (int)pk_type_ready::NORM_LOGIN, return_type::INPUT_ERROR, 0);
		isLoginSuc = false;
		return;
	}
	sql_str.Format(TEXT("select online_status from online_info where id=\'%d\'"), tui.id);
	sql.selectSql(sql_str);
	int online_status = -1;
	online_status = sql.m_pRecordset->GetCollect(TEXT("online_status"));
	if (online_status == 1)
	{
		SendPackHeader(login_status::READY, (int)pk_type_ready::NORM_LOGIN, return_type::FAILED, 0);
		isLoginSuc = false;
		return;
	}
	isLoginSuc = true;
	SendPackHeader(login_status::READY, (int)pk_type_ready::NORM_LOGIN, return_type::SUCCESS, 0);

	//保存用户名
	StrCpy(tui.account, user.account);
	trim(tui.account);
	trim(tui.nickname);
	tui.pLoginSock = this;
	//42'*' —— 122'z'
	CTime time = CTime::GetTickCount();
	srand(time.GetSecond() * time.GetMinute() * time.GetHour());
	for (TCHAR& tch : tui.key)
	{
		tch = (rand() % 81) + 42;
	}
	tui.key[KEY_SIZE] = _TEXT('\0');
	Send(&tui.id, sizeof(int));
	Send(&tui.key, sizeof(tui.key));
	sql_str.Format(TEXT("update online_info set online_status = %d ,ip=\'%s\',port=%d where id=%d"), ONLINE_STATUS_ON, ip_str, port, tui.id);
	sql.updateSql(sql_str);
}

void sLoginSock::OnFaceLogin() 
{
	int strLen = 0;
	Receive(&strLen, sizeof(int));
	if (strLen < 0 && strLen>(1024*2048))
	{
		//失败
		SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::INPUT_ERROR,0);
		return;
	}
	TCHAR* str = new TCHAR[strLen + 1];
	imageStr.reset(new char[strLen + 1]);
	Receive(str, sizeof(TCHAR) * strLen);
	str[strLen] = TEXT('\0');
	wcstombs(imageStr.get(), str, strLen + 1);
	faceDetect(accessToken, imageStr.get(),this->loginFaceDetectCallback,this);
	delete[] str;
}

size_t sLoginSock::loginFaceDetectCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	sLoginSock* lthis = (sLoginSock*)stream;
	// 获取到的body存放在ptr中，先将其转换为string格式
	std::string faceDetect_result = std::string((char*)ptr, size * nmemb);
	Json::Value root;
	Json::Reader reader;
	cout << faceDetect_result << endl;
	if (reader.parse(faceDetect_result, root))
	{
		if (root["error_code"].asInt() == 0)
		{
			faceSearch(accessToken, lthis->imageStr.get(), loginFaceSearchCallback, lthis);
			return size * nmemb;
		}
	}
	// "parse fail";
	lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::INPUT_ERROR, 0);
	return size * nmemb;
}

size_t sLoginSock::loginFaceSearchCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	// 获取到的body存放在ptr中，先将其转换为string格式
	string	faceSearch_result = std::string((char*)ptr, size * nmemb);
	sLoginSock* lthis = (sLoginSock*)stream;
	cout << faceSearch_result <<endl;
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(faceSearch_result, root))
	{
		if (root["error_code"].asInt() == 0)
		{
			int i = 0;
			if (root["result"]["user_list"][i]["score"].asInt() >= 80)
			{
				string accounta = root["result"]["user_list"][i]["user_id"].asString();
				if (accounta.length() > ACCOUNT_MAXSIZE)
				{
					lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::FAILED, 0);
					lthis->isLoginSuc = false;
					return size * nmemb;
				}
				TCHAR account[ACCOUNT_MAXSIZE + 1];
				mbstowcs(account, accounta.c_str(), ACCOUNT_MAXSIZE + 1);
				trim(account);
				CString sql_str;
				sql_str.Format(TEXT("select id,nickname from users where account=\'%s\'"), account);
				sql.selectSql(sql_str);
				int exist = 0;
				while (!sql.m_pRecordset->adoEOF)
				{
					//保存id 昵称
					lthis->tui.id = sql.m_pRecordset->GetCollect(TEXT("id"));
					StrCpyW(lthis->tui.nickname, (_bstr_t)sql.m_pRecordset->GetCollect(TEXT("nickname")));
					sql.m_pRecordset->MoveNext();
					exist++;
					break;
				}
				if (exist == 0)
				{
					lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::FAILED, 0);
					lthis->isLoginSuc = false;
					return size * nmemb;
				}
				sql_str.Format(TEXT("select online_status from online_info where id=\'%d\'"), lthis->tui.id);
				sql.selectSql(sql_str);
				int online_status = -1;
				online_status = sql.m_pRecordset->GetCollect(TEXT("online_status"));
				if (online_status == 1)
				{
					lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::FAILED, 0);
					lthis->isLoginSuc = false;
					return size* nmemb;
				}
				lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::SUCCESS, 0);
				lthis->isLoginSuc = true;
				//保存用户名
				StrCpy(lthis->tui.account, account);
				trim(lthis->tui.nickname);
				lthis->tui.pLoginSock = lthis;
				//42'*' —— 122'z'
		
				CTime time = CTime::GetTickCount();
				srand(time.GetSecond() * time.GetMinute() * time.GetHour());
				for (TCHAR& tch : lthis->tui.key)
				{
					tch = (rand() % 81) + 42;
				}
				lthis->tui.key[KEY_SIZE] = _TEXT('\0');
				lthis->Send(&(lthis->tui.id), sizeof(int));
				lthis->Send(&(lthis->tui.key), sizeof(lthis->tui.key));
				sql_str.Format(TEXT("update online_info set online_status = %d ,ip=\'%s\',port=%d where id=%d"), ONLINE_STATUS_ON, lthis->ip_str, lthis->port, lthis->tui.id);
				sql.updateSql(sql_str);
				return size * nmemb;
			}
		}
	}

	// "parse fail";
	lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::FACE_LOGIN, return_type::INPUT_ERROR, 0);
	return size * nmemb;

}

size_t sLoginSock::registerFaceDetectCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	sLoginSock* lthis = (sLoginSock*)stream;
	// 获取到的body存放在ptr中，先将其转换为string格式
	std::string faceDetect_result = std::string((char*)ptr, size * nmemb);
	Json::Value root;
	Json::Reader reader;
	cout << faceDetect_result << endl;
	if (reader.parse(faceDetect_result, root))
	{
		if (root["error_code"].asInt() == 0)
		{
			faceSearch(accessToken, lthis->imageStr.get(), registerFaceSearchCallback, lthis);
			return size * nmemb;
		}
	}
	// "parse fail";
	lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::INPUT_ERROR, 0);
	return size * nmemb;
}

size_t sLoginSock::registerFaceSearchCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	// 获取到的body存放在ptr中，先将其转换为string格式
	string	faceSearch_result = std::string((char*)ptr, size * nmemb);
	sLoginSock* lthis = (sLoginSock*)stream;
	cout << faceSearch_result << endl;
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(faceSearch_result, root))
	{
		if (root["error_code"].asInt() == 0)
		{
			int i = 0;
			if (root["result"]["user_list"][i]["score"].asInt() >= 80)
			{
				lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::ACCOUNT_EXISTED, 0);
				return 1;
			}
			else
			{//没注册 可以注册
				char accounta[ACCOUNT_MAXSIZE + 1];
				wcstombs(accounta, lthis->rui.account, ACCOUNT_MAXSIZE + 1);
				addFace(accessToken, lthis->imageStr.get(),accounta, registerAddFaceCallback,lthis);
			}
		}
		else if (root["error_code"].asInt() == 222207)
		{//第一个注册的人
			char accounta[ACCOUNT_MAXSIZE + 1];
			wcstombs(accounta, lthis->rui.account, ACCOUNT_MAXSIZE + 1);
			addFace(accessToken, lthis->imageStr.get(), accounta, registerAddFaceCallback, lthis);
		}
	}
	lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::INPUT_ERROR, 0);
	return 0;
}

size_t sLoginSock::registerAddFaceCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	// 获取到的body存放在ptr中，先将其转换为string格式
	string addFace_result = std::string((char*)ptr, size * nmemb);
	sLoginSock* lthis = (sLoginSock*)stream;
	cout << addFace_result << endl;
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(addFace_result, root))
	{
		if (root["error_code"].asInt() == 0)
		{//人脸添加成功  开始添加账号
			CString sql_str;
			sql_str.Format(TEXT(" insert into users(account,password,nickname) values(\'%s\',\'%s\',\'%s\') "
				" declare @i int "
				" select @i=id from users where account = \'%s\' "
				" insert into online_info(id,online_status,ip,port) values(@i,%d,\' \',0) "
			), lthis->rui.account, lthis->rui.password, lthis->rui.nickname, lthis->rui.account, ONLINE_STATUS_OFF);
			int i = sql.insertSql(sql_str);
			if (i == 0)
			{
				lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::FAILED, 0);
				cout << "fatal ERROR 数据库插入失败" << endl;
				return 0;
			}
			lthis->SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::SUCCESS, 0);
			return size * nmemb;
		}
	}
	
}

void sLoginSock::OnRegister()
{
	Receive(&rui, sizeof(register_user_info));
	int strLen = 0;
	Receive(&strLen, sizeof(int));
	if (strLen < 0 && strLen>(1024 * 2048))
	{
		//失败
		SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::INPUT_ERROR, 0);
		return;
	}
	TCHAR* str = new TCHAR[strLen + 1];
	imageStr.reset(new char[strLen + 1]);
	Receive(str, sizeof(TCHAR) * strLen);
	str[strLen] = TEXT('\0');
	wcstombs(imageStr.get(), str, strLen + 1);

	//检测账号是否存在
	CString sql_str;
	sql_str.Format(TEXT("select COUNT(*) as count from users where account=\'%s\'"), rui.account);
	sql.selectSql(sql_str);
	int i = sql.m_pRecordset->GetCollect(TEXT("count"));
	if (i == 1)
	{
		SendPackHeader(login_status::READY, (int)pk_type_ready::REGISTER, return_type::ACCOUNT_EXISTED, 0);
		delete[] str;
		return;
	}

	faceDetect(accessToken, imageStr.get(), this->registerFaceDetectCallback, this);
	delete[] str;
}

void sLoginSock::OnData()
{
	int count = theTeamManager.getTeamSize();
	SendPackHeader(login_status::JOIN, (int)pk_type_join::DATA, return_type::SUCCESS, count);
	if (count != 0)
	{
		team** teamArray = new team * [count];
		theTeamManager.getAllTeamData(teamArray, count);
		for (int i = 0; i < count; i++)
		{
			Send(&((trans_team)*teamArray[i]), sizeof(trans_team));
		}
		delete[] teamArray;
	}
}

void sLoginSock::OnPyData()
{
	SendPackHeader(login_status::JOIN, (int)pk_type_join::PYDATA, return_type::SUCCESS, sizeof(trans_user_info));
	Send(&trans_user_info(tui), sizeof(trans_user_info));
}

void sLoginSock::OnCreate()
{
	TCHAR team_name[TEAMNAME_MAXSIZE + 1];
	Receive(team_name, sizeof(team_name));
	if ((tnode = theTeamManager.CreateTeam(tui, team_name)) != nullptr)
	{
		tnode->pTeam->player[0].pLoginSock = this;
		SendPackHeader(login_status::JOIN, (int)pk_type_join::CREATE, return_type::SUCCESS, 0);
		Send(&trans_team(*(tnode->pTeam)), sizeof(trans_team));
	}
	else
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::CREATE, return_type::FAILED, 0);
	}

}

void sLoginSock::OnJoin(int team_id)
{
	//已经加入房间不能加入其他房间
	if (tnode != nullptr)
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::JOIN, return_type::FAILED, 0);
	}
	else
	{
		if ((tnode = theTeamManager.JoinTeam(team_id, this, tui)) != nullptr)
		{
			for (int i = 0; i < (tnode->pTeam->player_num); i++)
			{
				tnode->pTeam->player[i].pLoginSock->SendPackHeader(login_status::JOIN, (int)pk_type_join::JOIN, return_type::SUCCESS, tui.id);
				tnode->pTeam->player[i].pLoginSock->Send(&trans_team(*(tnode->pTeam)), sizeof(trans_team));
			}
		}
	}
}

void sLoginSock::OnQuit()
{
	if (tnode == nullptr)
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::QUIT, return_type::FAILED, 0);
	}
	else
	{
		theTeamManager.QuitTeam(tnode, tui.id);
		SendPackHeader(login_status::JOIN, (int)pk_type_join::QUIT, return_type::SUCCESS, tui.id);
		if (tnode != nullptr)
		{
			for (int i = 0; i < (tnode->pTeam->player_num); i++)
			{
				tnode->pTeam->player[i].pLoginSock->SendPackHeader(login_status::JOIN, (int)pk_type_join::QUIT, return_type::SUCCESS,tui.id);
				tnode->pTeam->player[i].pLoginSock->Send(&trans_team(*(tnode->pTeam)), sizeof(trans_team));
			}
		}
		tnode = nullptr;

	}
}

void sLoginSock::OnEnReady()
{
	if (tnode == nullptr)
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::ENREDY, return_type::FAILED, 0);
	}
	else
	{
		bool allprepare = true;
		for (int i = 0; i < (tnode->pTeam->player_num); i++)
		{
			if (tnode->pTeam->player[i].id == tui.id)
				tnode->pTeam->player[i].player_isPrepare = true;
			tnode->pTeam->player[i].pLoginSock->SendPackHeader(login_status::JOIN, (int)pk_type_join::ENREDY, return_type::SUCCESS, tui.id);
			if (!(tnode->pTeam->player[i].player_isPrepare))
				allprepare = false;
		}

		//三人都准备了  开始游戏
		if (allprepare && (tnode->pTeam->player_num == PLAYER_NUMBER))
		{
			//设置游戏开始  禁止其他操作
			tnode->pTeam->isRun = true;
			theTeamManager.removeTeam(tnode);
			theRunTeamManager.inserttailTeam(tnode);

			for (int i = 0; i < PLAYER_NUMBER; i++)
			{
				tnode->pTeam->player[i].player_isPrepare = false;
				tnode->pTeam->player[i].pLoginSock->SendPackHeader(login_status::JOIN, (int)pk_type_join::BEGIN, return_type::NOTHING, 0);
			}	
		}
	}
}

void sLoginSock::OnUnReady()
{
	if (tnode == nullptr)
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::UNREDY, return_type::FAILED, 0);
	}
	else
	{
		for (int i = 0; i < (tnode->pTeam->player_num); i++)
		{
			if (tnode->pTeam->player[i].id == tui.id)
				tnode->pTeam->player[i].player_isPrepare = false;
			tnode->pTeam->player[i].pLoginSock->SendPackHeader( login_status::JOIN, (int)pk_type_join::UNREDY, return_type::SUCCESS, tui.id);
		}

	}
}

void sLoginSock::OnTmData()
{
	if (tnode == nullptr)
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::TMDATA, return_type::FAILED, 0);
	}
	else
	{
		SendPackHeader(login_status::JOIN, (int)pk_type_join::TMDATA, return_type::SUCCESS, tui.id);
		Send(&trans_team(*(tnode->pTeam)), sizeof(trans_team));
	}
}

void sRunSock::OnReceive()
{
	try
	{
		OnCheck();
		while (true)
		{
			if (pRun != nullptr)
			{//身份通过 且游戏运行了
				if (pRunTeam->pTeam->isRun)
				{
					runHeader rh;
					Receive(&rh, sizeof(runHeader));
					pRun->OnReceive(rh, sequence);

				}
				else
				{
					Close();
					break;
				}
			}
			else
				Sleep(100);
		}
	}
	catch (CString & s)
	{
		if (s == "SOCK CLOSE!")
		{//客户端套接字关闭
			OnClose();
		}
	}
	catch (...)
	{

	}
}

void sRunSock::OnClose()
{
	//只要玩家断开连接就会触发  有可能是游戏中  也有可能是游戏结束
	//游戏中断线才处理
	if (pRunTeam != nullptr)
	{
		if (pRunTeam->pTeam->isRun)
		{
			pRunTeam->pTeam->isRun = false;
			theRunTeamManager.removeTeam(pRunTeam);
			theTeamManager.inserttailTeam(pRunTeam);

			for (int i = 0; i < PLAYER_NUMBER; i++)
			{
				if (i != sequence)
				{
					pRun->playerdata[i].psock->SendrunHeader(run_status::END, (int)pk_type_end::PALYER_DISCONNECT, (int)pk_type_norm::GAMESTATUS);
					closesocket(pRun->playerdata[i].psock->sock);
				}
			}
			if (pRunTeam->pTeam->runThread != NULL)
			{
				TerminateThread(pRunTeam->pTeam->runThread, 0);
				pRunTeam->pTeam->runThread = NULL;
			}
			if (player_id != -1)
			{
				theTeamManager.QuitTeam(pRunTeam, player_id);
			}
			closesocket(sock);
		}
		else//游戏结束后退出
		{
			if (player_id != -1)
			{
				for (int i = 0; i < pRunTeam->pTeam->player_num; i++) 
				{
					if (pRunTeam->pTeam->player[i].id == player_id)
						pRunTeam->pTeam->player[i].pRunSock = nullptr;
				}	
			}
				
		}
	}
}

void sRunSock::Close()
{
	closesocket(sock);
}

void sRunSock::SendrunHeader(run_status rs, int sq, int type)
{
	runHeader rh;
	rh.rs = rs;
	rh.sq = sq;
	rh.type = type;
	Send(&rh, sizeof(runHeader));
}

//验证成功的话  根据提供的tw->pTeam->player的id绑定sRunsock的this指针
void sRunSock::OnCheck()
{
	std::cout <<this<<"OnCheck() start" << endl;
	checkStruct cs;
	std::cout << this << "OnCheck() receive start" << endl;
	Receive(&cs.team_id, sizeof(int));
	Receive(&cs.id, sizeof(int));
	Receive(cs.key, sizeof(cs.key));
	std::cout << this << "OnCheck() receive end" << endl;
	//Receive(&cs, sizeof(cs)); 不能用  因为结构体会被填充字节 导致问题
	team_wrapper* tw = theRunTeamManager.getTeamByTeamId(cs.team_id);
	if (tw != nullptr)
	{
		bool isAllPrepare = true;
		bool isHaveThisPlayer = false;
		int index;
		for(int i=0;i<PLAYER_NUMBER;i++)
		{
			if (tw->pTeam->player[i].id == cs.id)
			{
				isHaveThisPlayer = true;
				index = i;
				player_id = cs.id;
			}
			if (tw->pTeam->player[i].player_isPrepare = false)
				isAllPrepare = false;
		}
		if (!StrCmpW(tw->pTeam->player[index].key, cs.key) && isHaveThisPlayer)
			isCheckRight = true;
		else
			isCheckRight = false;
		if (isAllPrepare)
		{
			//登录成功需要等待其他玩家连接
			pRunTeam = tw;
			tw->pTeam->player[index].pRunSock = this;
			//判断是否是最后一个玩家  是的话就真的 开始游戏了
			bool allsockin = true;
			for (int i = 0; i < PLAYER_NUMBER; i++)
			{
				if (tw->pTeam->player[i].pRunSock == nullptr)
				{
					allsockin = false;
					break;
				}
			}
			std::cout << this << "OnCheck() suc end" << endl;
			if (allsockin)
			{
				bool allCheckRight = true;
				for (int i = 0; i < PLAYER_NUMBER; i++)
				{
					if (!(tw->pTeam->player[i].pRunSock->isCheckRight))
					{
						allCheckRight = false;
						break;
					}
				}
				if (allCheckRight)
				{
					for (int i = 0; i < PLAYER_NUMBER; i++)
					{
						tw->pTeam->player[i].pRunSock->SendrunHeader(run_status::BEGIN, 0, (int)pk_type_begin::CHECK_SUC);//告诉客户端成功
					}
					BeginDdzRun(tw);
				}
				else
				{
					for (int i = 0; i < PLAYER_NUMBER; i++)
					{
						tw->pTeam->player[i].pRunSock->SendrunHeader(run_status::BEGIN, 0, (int)pk_type_begin::CHECK_FAIL);//告诉客户端错误
						tw->pTeam->player[i].pRunSock->Close();//验证失败 关闭套接字
						tw->pTeam->player[i].pRunSock->OnClose();
					}
					std::cout << this << "OnCheck() fail end" << endl;
				}
			}	
			return;
		}
	}
	SendrunHeader(run_status::BEGIN, 0, (int)pk_type_begin::CHECK_FAIL);//告诉客户端错误
	Close();//验证失败 关闭套接字
	OnClose();
	std::cout << this << "OnCheck() fail end" << endl;
}

//创建DdzRun对象  设置sRunSock的pRun isRun 和 Sequence
void sRunSock::BeginDdzRun(team_wrapper* ptw)
{
	//之所以要new是因为如果是局部变量出了函数就会释放
	DdzRun* pdr = new DdzRun(ptw);
	//需要把team所有玩家的sRunSock的pRun都设置一下
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		for (int j = 0; j < PLAYER_NUMBER; j++)
		{
			//找sequence
			if (pdr->playerdata[i].psock == ptw->pTeam->player[j].pRunSock)
			{//给每个sRunSock设置sequence
				ptw->pTeam->player[j].pRunSock->sequence = i;
				break;
			}
		}
		ptw->pTeam->player[i].pRunSock->pRun = pdr;
	}
	HANDLE hd = CreateThread(NULL, 0,
		[](LPVOID pdr) -> DWORD WINAPI
		{//开辟一个线程 来运行start函数  知道游戏结束
			((DdzRun*)pdr)->Start();
			delete pdr;
			return 0;
		}, pdr, 0, NULL);

	for (int i = 0; i < PLAYER_NUMBER; i++) 
	{
		ptw->pTeam->player[i].pRunSock->pRunTeam = ptw;
	}
	ptw->pTeam->runThread = hd;
}

int base_sock::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	//1.当recv返回值为0是表示对方已经正常(msdn说是完美的)的断开连接. 
	//2.当recv返回socket_error时, 说明对方已经断开连接, 但是可能是非正常断开
	int len = recv(sock, (char*)lpBuf, nBufLen, nFlags);
	if (len <= 0)
		throw CString("SOCK CLOSE!");
	return len;
}

int base_sock::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	return send(sock, (char*)lpBuf, nBufLen, nFlags);
}
