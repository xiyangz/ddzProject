#include"pch.h"
#include"ddzrun.h"
#include "team_manager.h"



DdzRun::DdzRun(team_wrapper* ptw)
{
	m_pRunTeam = ptw;
	//分配sequence
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		playerdata[i].sequence = i;
	}
	//随机抽取玩家位置
	CTime time;
	int randInts[3] = { 0,1,2 };
	for (int k = 0; k < PLAYER_NUMBER; k++)
	{
		time = CTime::GetTickCount();
		srand(time.GetSecond() * time.GetMinute() * time.GetHour());
		int n =  rand() % (PLAYER_NUMBER - k);
		playerdata[randInts[n]].psock = ptw->pTeam->player[k].pRunSock;
		playerdata[randInts[n]].id = ptw->pTeam->player[k].id;
		playerdata[randInts[n]].sequence = randInts[n];
		randInts[n] = randInts[PLAYER_NUMBER - 1 - k];
	}
}

DdzRun::~DdzRun()
{
}

void DdzRun::Start()
{/* 1.开始游戏(BEGIN)
		1.0发送游戏阶段
	    1.1告知玩家sequence
		1.2等待玩家接受sequence成功	
	2.发牌 DEAL
		2.0改变阶段并发送游戏阶段
		2.1初始化卡牌
		2.2选牌
		2.3发牌
		2.4等待玩家接受卡牌完毕
	3.抢地主 LOOT
		3.0改变阶段并发送游戏阶段
		3.1每位玩家陆续抢或不抢地主
		3.2产生地主并发送地主牌
		3.3等待玩家接受地主牌完毕
	4.出牌 POKE
	5.结束 结算 END*/
	//1.0
	SendRunStatus();
	//1.2
	SendSequence();
	//1.4
	WaitPlayerFinish(isRecvSequenceOver);
	//2.0
	status = run_status::DEAL;
	SendRunStatus();
	//2.1
	InitPoker();
	//2.2
	SelectPoker();
	//2.3
	SendPoker();
	//2.4
	WaitPlayerFinish(isRecvPokerOver);
	//3.0
	status = run_status::LOOT;
	SendRunStatus();
	//3.1-2
	Loot();
	//3.3
	WaitPlayerFinish(isRecvLooterpkOver);
	//4.0
	status = run_status::POKE;
	SendRunStatus();
	//4.x
	Poke();
	//5
	Sleep(2000);
	status = run_status::END;
	EndPoker();
}

void DdzRun::OnReceive(const runHeader& rh,int sequence)
{
	if ((run_status::NORM == rh.rs) && (rh.type == (int)pk_type_norm::HEARTBEAT))
	{
		//收到心跳包 说明玩家在线  如果长期没收到需要判断为掉线
	}
	else
	{
		switch (rh.rs)
		{
		case run_status::BEGIN:
			OnMsgBegin(sequence,(pk_type_begin)rh.type);
			break;
		case run_status::DEAL:
			OnMsgDeal(sequence, (pk_type_deal)rh.type);
			break;
		case run_status::LOOT:
			OnMsgLoot(sequence,(pk_type_loot)rh.type);
			break;
		case run_status::POKE:
			OnMsgPoke(sequence,(pk_type_poke)rh.type);
			break;
		case run_status::END:
			OnMsgEnd(sequence, (pk_type_end)rh.type);
			break;
		}
	}
}

void DdzRun::PlayerDisconnected(int sequence)
{
}

void DdzRun::OnMsgBegin(int sequence, pk_type_begin type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgBegin sequence 范围错误" << endl;
		return;
	}
	if (type == pk_type_begin::RECV_SEQ_SUC) {
		isRecvSequenceOver[sequence] = true;
	}
}

void DdzRun::OnMsgDeal(int sequence, pk_type_deal type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgDeal sequence 范围错误" << endl;
		return;
	}
	if (type == pk_type_deal::ENPOKE) {
		isRecvPokerOver[sequence] = true;
	}
}

void DdzRun::OnMsgLoot(int sequence, pk_type_loot type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgLoot sequence 范围错误" << endl;
		return;
	}
	//loot阶段任何人可以消息
	/*if (sequence != turn) {
		cout << "OnMsgLoot sequence与当前回合不符合" << endl;
		return;
	}*/
	if (type == pk_type_loot::ENLOOT) {
		isLootOver = true;
		isloot[sequence] = true;
	}
	else if (type == pk_type_loot::UNLOOT) {
		isLootOver = true;
		isloot[sequence] = false;
	}
	else if (type == pk_type_loot::RECV_PKR_SUC) {
		isRecvLooterpkOver[sequence] = true;
	}
}

void DdzRun::OnMsgPoke(int sequence, pk_type_poke type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgPoke sequence 范围错误" << endl;
		return;
	}
	if (sequence != turn) {
		cout << "OnMsgPoke sequence与当前回合不符合" << endl;
		return;
	}

	if (type == pk_type_poke::ENPLAY) {
		//出牌
		playerdata[sequence].psock->Receive(&ready_poke_pokers_num, sizeof(int));
		for (int i = 0,pos=0; i < ready_poke_pokers_num; i++) {
			playerdata[sequence].psock->Receive(&pos, sizeof(int));
			if (pos >= HANDPOKER_MAXSIZE || pos < 0)
			{
				cout << "OnMsgPoke 接受的卡牌pos不符合规则" << endl;
				return;
			}
			ready_poke_pokers_pos[i] = pos;
			ready_poke_pokers[i] = handpoker[sequence][pos];
		}
		isPokeOver = true;
	}
	else if (type == pk_type_poke::UNPLAY) {
		//不出
		ready_poke_pokers_num = 0;
		isPokeOver = true;
	}
}

void DdzRun::OnMsgEnd(int sequence, pk_type_end type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgEnd sequence 范围错误" << endl;
		return;
	}
}

void DdzRun::SendRunStatus()
{
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		playerdata[i].psock->SendrunHeader(status, 0, (int)pk_type_norm::GAMESTATUS);
	}
}

void DdzRun::SendTurn()
{
	
	if (status == run_status::LOOT){
		for (int i = 0; i < PLAYER_NUMBER; i++)
			playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_loot::TURN);
	}
	//打牌阶段要告诉玩家  此回合的玩家是不是先手
	else if (status == run_status::POKE) {
		int isFirst = is_first_turn ? 1 : 0;
		for (int i = 0; i < PLAYER_NUMBER; i++)
		{
			playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_poke::TURN);
			playerdata[i].psock->Send(&isFirst, sizeof(int));
		}
	}
}

void DdzRun::WaitPlayerFinish(bool* bary)
{
	bool it = false;
	int wait_time = 0;
	while ((!it) && wait_time<=(WAIT_TIME_MAX*10))
	{
		Sleep(100);
		++wait_time;
		for (int i = 0; i < PLAYER_NUMBER; i++)
		{
			if (bary[i] == false)
			{
				it = false;
				break;
			}
			else
			{
				it = true;
			}
		}
	}
	if (wait_time > (WAIT_TIME_MAX * 10))
	{//玩家掉线处理
		for (int i = 0; i < PLAYER_NUMBER; i++)
		{
			if (bary[i] == false)
			{
				PlayerDisconnected(i);
				break;
			}
		}
	}
}

void DdzRun::SendSequence()
{
	for (int i = 0; i < PLAYER_NUMBER; i++)
		playerdata[i].psock->SendrunHeader(status, i, (int)pk_type_begin::SEQUENCE);
}

void DdzRun::InitPoker()
{
	int pos = 0;
	nPoker[pos].color = pkr_color::JOKER;
	nPoker[pos].kind = pkr_kind::POKER_B_JOKER;
	++pos;
	nPoker[pos].color = pkr_color::JOKER;
	nPoker[pos].kind = pkr_kind::POKER_L_JOKER;
	++pos;
	int nkind = (int)pkr_kind::POKER_2;
	int ncolor = 0;
	while (nkind >= (int)pkr_kind::POKER_3)
	{
		ncolor = 0;
		while (ncolor <= 39)
		{
			nPoker[pos].kind = (pkr_kind)nkind;
			nPoker[pos].color = (pkr_color)ncolor;
			ncolor += 13;
			pos++;
		}

		nkind--;
		if (nkind == 12)
			nkind = 11;
	}
}

void DdzRun::SelectPoker()
{
	//发牌  初始化 本地
	int selectint[PKR_COUNT];
	for (int i = 0; i < PKR_COUNT; i++)
		selectint[i] = i;
	int remain = PKR_COUNT - 1;//剩余位置
	int belong = 0;
	srand((unsigned int)GetTickCount64());
	for (int i = 0; i < PKR_COUNT - 3; i++)
	{//随机分配51张牌
		int pos = rand() % (remain + 1);
		nPoker[selectint[pos]].belong = (pkr_belong)belong;
		selectint[pos] = selectint[remain];
		remain--;
		belong++;
		if (belong == 3)
			belong = 0;
	}

	for (int i = 0; i < 3; i++)
	{//最后三张是地主牌
		nPoker[selectint[i]].belong = pkr_belong::THREE;
	}

	for (int i = 0,j=0; i < PKR_COUNT; i++) {
		//不直接在上面赋值  是因为上面顺序会错
		if (nPoker[i].belong == pkr_belong::THREE) {
			threepk[j] = nPoker[i];
			j++;
		}
	}
}

void DdzRun::SendPoker()
{
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{//设置玩家手牌数量
		handpoker_number[i] = 17;
		//告知玩家将要发送卡牌
		playerdata[i].psock->SendrunHeader(status, i, (int)pk_type_deal::POKER);
	}

	int i0 = 0, i1 = 0, i2 = 0;
	for (int i = 0; i < PKR_COUNT; i++)
	{
		if (nPoker[i].belong == pkr_belong::PLAYER1)
		{
			handpoker[(int)pkr_belong::PLAYER1][i0] = nPoker[i];
			++i0;
			playerdata[(int)pkr_belong::PLAYER1].psock->Send(&(nPoker[i]), sizeof(poker));
		}
		else if (nPoker[i].belong == pkr_belong::PLAYER2)
		{
			handpoker[(int)pkr_belong::PLAYER2][i1] = nPoker[i];
			++i1;
			playerdata[(int)pkr_belong::PLAYER2].psock->Send(&(nPoker[i]), sizeof(poker));
		}
		else if (nPoker[i].belong == pkr_belong::PLAYER3)
		{
			handpoker[(int)pkr_belong::PLAYER3][i2] = nPoker[i];
			++i2;
			playerdata[(int)pkr_belong::PLAYER3].psock->Send(&(nPoker[i]), sizeof(poker));
		}
	}
}

void DdzRun::AddThreeToHandpoker()
{
	int pos_move = 3;
	int handpokerIndex = 16;
	int threepokerIndex = 2;
	while ((threepokerIndex) >= 0 && (handpokerIndex >= 0)) 
	{
		if (handpoker[looter_sequence][handpokerIndex].kind > threepk[threepokerIndex].kind)
		{//kind越大，排在越前
			handpoker[looter_sequence][handpokerIndex + pos_move] = threepk[threepokerIndex];
			threepokerIndex--;
			pos_move--;
		}
		else if (handpoker[looter_sequence][handpokerIndex].kind == threepk[threepokerIndex].kind) {
			if (handpoker[looter_sequence][handpokerIndex].color > threepk[threepokerIndex].color) {
				//颜色越小 排在越前
				handpoker[looter_sequence][handpokerIndex + pos_move] = handpoker[looter_sequence][handpokerIndex];
				handpokerIndex--;
			}
			else {
				handpoker[looter_sequence][handpokerIndex + pos_move] = threepk[threepokerIndex];
				threepokerIndex--;
				pos_move--;
			}
		}
		else {//kind越小，排在越后
			handpoker[looter_sequence][handpokerIndex + pos_move] = handpoker[looter_sequence][handpokerIndex];
			handpokerIndex--;
		}
	}
	while (threepokerIndex >= 0) {
		handpoker[looter_sequence][threepokerIndex] = threepk[threepokerIndex];
		threepokerIndex--;
	}
}

void DdzRun::Loot()
{//抢地主规则：1. 超过两个抢地主  抢四次
 //           2. 等于1个时   抢三次
 //           3. 没人抢时   随机选一个当地主
	int delay_msec;//是 秒/10 不是毫秒
	turn = 0;
	for (int i = 0, j = 0; j < PLAYER_NUMBER + 1; i++, j++)
	{
		isLootOver = false;
		delay_msec = 0;
		remain_second = 15;
		//告知玩家当前回合
		SendTurn();
		//等待玩家操作
		while (!isLootOver)
		{
			if (delay_msec == 159)
			{//超时  服务器采用默认操作
				isLootOver = true;
				isloot[i] = false;
				remain_second = 15;
			}
			else if ((delay_msec % 10) == 0)
			{
				SendRemainSecond();
				remain_second--;
			}
			Sleep(100);
			delay_msec++;
		}
		//告知所有玩家当前玩家抢地主情况
		SendLootRlt();
		//根据抢地主情况来判断下回合是谁
		if (i == 2)
		{
			int lootNum = 0;//三个人中抢地主人数
			int index = 0;
			for (int k = 0; k < PLAYER_NUMBER; k++)
			{
				if (isloot[k])
					++lootNum;
			}
			//判断人数
			switch (lootNum)
			{
			case 0://没人抢  随机选
			{
				CTime time = CTime::GetTickCount();
				srand(time.GetSecond() * time.GetMinute() * time.GetHour());
				isloot[rand() % PLAYER_NUMBER] = true;
				++j;
			}
			break;
			case 1:
				++j;//一个人抢   只进行3次
				break;
			case 2:
			{
				for (int j = 0; j < PLAYER_NUMBER; j++)
				{//选出最近一次抢地主的  再次抢地主
					if (isloot[j])
					{
						turn = j;
						break;
					}
				}
			}
			break;
			case 3:
				turn = 0;
				break;
			}
		}
		else
			turn = GetNextTurn();//正常改变回合

	}

	
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		if (isloot[i])
		{
			//设置玩家为地主
			playerdata[i].isLandOwner = true;
			//设置地主sequence
			looter_sequence = i;
			//地主玩家手牌加3 牌库地主牌属性设置
			handpoker_number[i] += 3;
			for (int j = 0; j < PKR_COUNT; j++)
			{
				if (nPoker[j].belong == pkr_belong::THREE)
					nPoker[j].belong = (pkr_belong)i;
			}
			//将地主玩家handpoker加上那三张
			AddThreeToHandpoker();
			break;
		}
	}
	//告知玩家地主数据
	SendLooterData();
}

void DdzRun::SendLootRlt()
{
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		if (isloot[turn])
			playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_loot::ENLOOT);
		else
			playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_loot::UNLOOT);
	}
}

void DdzRun::SendLooterData()
{
	looter_data ld;
	ld.sequence = looter_sequence;
	for (int i = 0; i < 3; i++) {
		ld.threepk[i] = threepk[i];
	}
	for (int i = 0; i < PLAYER_NUMBER; i++) {
		playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_loot::LOOT_RLT);
		playerdata[i].psock->Send(&ld, sizeof(looter_data));
	}
	/*
	struct looter_data
	{
		int sequence;
		poker threepk[3];
	}
	*/
}

void DdzRun::Poke()
{
	turn = looter_sequence;
	int delay_msec;
	bool isRightPoke;
	while (handpoker_number[GetLastTurn()] != 0)
	{
		delay_msec = 0;
		remain_second = 15;
		isRightPoke = false;
		//等待出牌 并 通知剩余时间
		SendTurn();
		while (!isRightPoke)
		{
			isPokeOver = false;
			while (!isPokeOver)
			{
				if (delay_msec == 159)
				{//超时  服务器采用默认操作
					isPokeOver = true;
					if (is_first_turn){
						//第一个人不能不出牌
						ready_poke_pokers_num = 1;
						ready_poke_pokers[0] = handpoker[turn][handpoker_number[turn] - 1];
						ready_poke_pokers_pos[0] = handpoker_number[turn] - 1;
					}
					else {
						ready_poke_pokers_num = 0;
					}
					remain_second = 15;
				}
				else if ((delay_msec % 10) == 0)
				{
					SendRemainSecond();
					remain_second--;
				}
				Sleep(100);
				delay_msec++;
			}
			//已经出牌
			isRightPoke = CheckPoker();//判断出牌是否合理
			if (!isRightPoke)
				playerdata[turn].psock->SendrunHeader(status, turn, (int)pk_type_poke::UNPOKE);
		}
		ChangePokerData();
		PlayPoker();

		turn = GetNextTurn();
	}
	SendTurn();
}

void DdzRun::SendRemainSecond()
{
	if (status == run_status::LOOT) {
		for (int i = 0; i < PLAYER_NUMBER; i++) {
			playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_loot::TIME);
			playerdata[i].psock->Send(&remain_second, sizeof(int));
		}
	}
	else if (status == run_status::POKE) {
		for (int i = 0; i < PLAYER_NUMBER; i++) {
			playerdata[i].psock->SendrunHeader(status, turn, (int)pk_type_poke::TIME);
			playerdata[i].psock->Send(&remain_second, sizeof(int));
		}
	}
	//向所有玩家发生剩余时间
				/*
				struct remain_time
				{
					int rem_sec;
					int sequence;
					bool is_first_turn;
				}
				*/
}

void DdzRun::EndPoker()
{
	if (m_pRunTeam->pTeam->isRun == true)
	{
		theRunTeamManager.removeTeam(m_pRunTeam);
		theTeamManager.inserttailTeam(m_pRunTeam);
		m_pRunTeam->pTeam->isRun = false;
		for (int i = 0; i < PLAYER_NUMBER; i++)
		{
			closesocket(playerdata[i].psock->sock);
		}
	}
	//告诉所以sock游戏结束  isRun设置为false
	//设置游戏结束  设置所有人取消准备
}

bool DdzRun::CheckPoker()
{
	int length = ready_poke_pokers_num;
	poker* p = ready_poke_pokers;
	int poker_count = 0;//点数
	//检查timepoker中的牌
	//先手
	if (is_first_turn)
	{	 //一张随便出  两张必须一样  三张必须一样  四张可为三带一可为四个相同  五张 连续单  三带二   
		 //六张飞机  四代二  三对二 连续单   七张连续单  八张
		 //A当成13 2当成14 
		switch (length)
		{
			//从左到右判断  放在左边会先判断
		case 1:
			return IsOne(p,length);
		case 2://一对子  或者 大小王
			return (IsTwoSame(p, length) || IsTwoJoker(p, length));
		case 3://三张相同
			return IsThreeSame(p, length);
		case 4: // 四张火  三带一   火判断要先于三带一
			return (IsBomb(p, length) || IsThreeWithOne(p, length));
		case 5://三带对 顺子
			return (IsThreeWithTwo(p, length) || IsContinual(p, length));
		case 6://四带二   顺子  连对  飞机
			return (IsFourWithTwo(p, length) || IsContinual(p, length)
				|| IsDoubleContinual(p, length) || IsPlaneWithzero(p, length));
		case 7://顺子
			return IsContinual(p, length);
		case 8://飞机  顺子  连对
			return (IsPlaneWithtwo(p, length) || IsContinual(p, length) || IsDoubleContinual(p, length));
		case 9://飞机  顺子
			return (IsContinual(p, length) || IsPlaneWithzero(p, length));
		case 10://连对 顺子 飞机带二
			return (IsContinual(p, length) || IsDoubleContinual(p, length) || IsPlaneWithtwo(p, length));
		case 11://顺子  
			return IsContinual(p, length);
		case 12://顺子 连对 飞机带0 飞机带1
			return (IsContinual(p, length) || IsDoubleContinual(p, length)
				|| IsPlaneWithzero(p, length) || IsPlaneWithone(p, length));
		case 14://连对
			return IsDoubleContinual(p, length);
		case 15://飞机带0 飞机带2
			return (IsPlaneWithzero(p,length)||IsPlaneWithtwo(p, length));
		case 16://连对 飞机带一
			return (IsDoubleContinual(p, length) || IsPlaneWithone(p, length));
		case 18://连对
			return IsDoubleContinual(p, length);
		case 20://连对 飞机带一 飞机带二
			return (IsDoubleContinual(p, length) || IsPlaneWithone(p, length) || IsPlaneWithtwo(p, length));
		default:
			return false;
		}
	}
	else//后手 //后手需要与最近一次出牌的牌比大小
	{
		//后手可以不出牌 
		if (length == 0)
			return true;
		else
		{//接下来的肯定是出了牌
			//前者是王炸  无解
			if ((last_pokers_type == pkr_type::TWOJOKER))
				return false;
			//自己是王炸 
			else if((length==2)&&(IsTwoJoker(p,length)))
				return true;
			//前者出的是炸弹 你得是炸弹且比他大 或者 王炸(但到这里你已经不是王炸了)
			else if (last_pokers_type == pkr_type::FOURSAME)
			{
				if (length == 4)
				{
					int poker_count = last_pokers_count;
					if (IsBomb(p, length))
					{
						if (poker_count < last_pokers_count)
							return true;
						else//出牌失败  得恢复原来的点数
							last_pokers_count = poker_count;
					}
				}
				return false;
			}
			//前者出的是其他类型，你可以是王炸(但到这里你已经不是王炸了) 炸弹 或者同类型但比前者大
			else
			{
				if((length==4)&&(IsBomb(p,length)))
					return true;
				else
				{
					//获取上次出的牌数
					int last_turn = GetLastTurn();
					int last_two_turn = (last_turn - 1) < 0 ? 2 : (last_turn - 1);
					int last_length = lastedpoker_number[last_turn] == 0 ? lastedpoker_number[last_two_turn] : lastedpoker_number[last_turn];
					//在此，如果张数不同就不能出
					if (length != last_length)
						return false;
					poker_count = last_pokers_count;
					switch (last_pokers_type)
					{
					//张数一样必然满足函数
					case pkr_type::SINGLE:
							return FuncOfLastJudgeWrapper(&DdzRun::IsOne, p, length);
						break;
					case pkr_type::TWOSAME:
							return FuncOfLastJudgeWrapper(&DdzRun::IsTwoSame, p, length);
						break;
					case pkr_type::THRWHZRE:
							return FuncOfLastJudgeWrapper(&DdzRun::IsThreeSame, p, length);
						break;
					case pkr_type::THRWHONE:
							return FuncOfLastJudgeWrapper(&DdzRun::IsThreeWithOne, p, length);
						break;
					case pkr_type::THRWHTWO:
							return FuncOfLastJudgeWrapper(&DdzRun::IsThreeWithTwo, p, length);
						break;
					case pkr_type::CONTINUE:
							return FuncOfLastJudgeWrapper(&DdzRun::IsContinual, p, length);
						break;
					case pkr_type::DOUBLECN:
							return FuncOfLastJudgeWrapper(&DdzRun::IsDoubleContinual, p, length);
						break;
					case pkr_type::PLANEZRE:
						return FuncOfLastJudgeWrapper(&DdzRun::IsPlaneWithzero, p, length);
						break;
					case pkr_type::PLANEONE://不可以出火 但是会在前面就判断到 
						return FuncOfLastJudgeWrapper(&DdzRun::IsPlaneWithone, p, length);
						break;
					case pkr_type::PLANETWO:
						if (length == 15)
						{//排除是飞机带0
							if (FuncOfLastTestWrapper(&DdzRun::IsPlaneWithzero, p, length))
								return false;
						}
						else if (length == 20)
						{//排除是飞机带1
							if (FuncOfLastTestWrapper(&DdzRun::IsPlaneWithone, p, length))
								return false;
						}
						return FuncOfLastJudgeWrapper(&DdzRun::IsPlaneWithtwo, p, length);
						break;
					case pkr_type::FURWHTWO:
						return FuncOfLastJudgeWrapper(&DdzRun::IsFourWithTwo, p, length);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	return false;
}

void DdzRun::ChangePokerData()
{	
	//玩家手牌减少  牌数据更改
	//将牌添加到lastedpoker 并告知玩家
	//如果是没出牌 则isContinueUnpoke++
	if (ready_poke_pokers_num == 0)
	{
		lastedpoker_number[turn] = ready_poke_pokers_num;
		isContinueUnpoke++;
		//连续两次不出  下回合设置成先手 中断连续
		if (isContinueUnpoke == 2)
		{
			is_first_turn = true;
			isContinueUnpoke = 0;
		}
	}
	else
	{
		//加入上回合出的牌
		lastedpoker_number[turn] = ready_poke_pokers_num;
		for (int k = 0; k < ready_poke_pokers_num; k++)
		{
			lastedpoker[turn][k] = ready_poke_pokers[k];
		}
		//改变玩家手牌
		 //改变手牌数据
		handpoker_number[turn] -= ready_poke_pokers_num;
		int pos = 0;
		for (int i = 0; i < handpoker_number[turn]; i++) {
			while (((i+pos) == ready_poke_pokers_pos[pos]) && (pos < ready_poke_pokers_num)) {
				pos++;
			}
			if (pos > 0)
				handpoker[turn][i] = handpoker[turn][i + pos];
		}
		//出了牌  中断连续不出 下回合也不可能是先手
		isContinueUnpoke = 0;
		is_first_turn = false;
	}
}

int DdzRun::GetLastTurn()
{
	return (turn - 1) < 0 ? (PLAYER_NUMBER - 1) : (turn - 1);
}

int DdzRun::GetNextTurn()
{
	return (turn + 1) == PLAYER_NUMBER ? 0 : (turn + 1);
}
//告知玩家 本回合出牌情况  要告知玩家卡牌
void DdzRun::PlayPoker	()
{
	for (int j = 0; j < PLAYER_NUMBER; j++) 
	{
		playerdata[j].psock->SendrunHeader(status, turn, (int)pk_type_poke::ENPOKE);
		//告知玩家出的牌类型和点数
		playerdata[j].psock->Send(&last_pokers_type, sizeof(int));
		playerdata[j].psock->Send(&last_pokers_count, sizeof(int));
		//告知玩家出的卡牌
		playerdata[j].psock->Send(&ready_poke_pokers_num, sizeof(int));
		for (int i = 0; i < ready_poke_pokers_num; i++)
		{
			playerdata[j].psock->Send(&ready_poke_pokers[i], sizeof(poker));
		}
	}
}
//单张
bool DdzRun::IsOne(poker* cpoker, int length)
{
	last_pokers_count = int(cpoker[0].kind);
	last_pokers_type = pkr_type::SINGLE;
	return true;
}
//判断对子
bool DdzRun::IsTwoSame(poker* cpoker, int length)
{
	if (cpoker[0].kind == cpoker[1].kind)
	{
		last_pokers_count = int(cpoker[length - 1].kind);
		last_pokers_type = pkr_type::TWOSAME;
		return true;
	}
	return false;
}
//判断三张相同
bool DdzRun::IsThreeSame(poker* cpoker, int length)
{
	if (cpoker[0].kind == cpoker[1].kind && cpoker[1].kind == cpoker[2].kind)
	{
		last_pokers_count = int(cpoker[length - 1].kind);
		last_pokers_type = pkr_type::THRWHZRE;
		return true;
	}
	return false;
}
//三带一  四张火会算在内  在此判断之前判断是否是火
bool DdzRun::IsThreeWithOne(poker* cpoker, int length)
{//三带一能带大小王
	if ((cpoker[0].kind == cpoker[1].kind) && (cpoker[1].kind == cpoker[2].kind))
	{
		last_pokers_count = int(cpoker[0].kind);
		last_pokers_type = pkr_type::THRWHONE;
		return true;
	}
	else if ((cpoker[1].kind == cpoker[2].kind) && (cpoker[2].kind == cpoker[3].kind))
	{
		last_pokers_count = int(cpoker[1].kind);
		last_pokers_type = pkr_type::THRWHONE;
		return true;
	}
	return false;
}
//三带二
bool DdzRun::IsThreeWithTwo(poker* cpoker, int length)
{
	if ((cpoker[0].kind == cpoker[1].kind) && (cpoker[1].kind == cpoker[2].kind) && (cpoker[3].kind == cpoker[4].kind))
	{
		last_pokers_count = int(cpoker[0].kind);
		last_pokers_type = pkr_type::THRWHTWO;
		return true;
	}
	else if ((cpoker[0].kind == cpoker[1].kind) && (cpoker[2].kind == cpoker[3].kind) && (cpoker[3].kind == cpoker[4].kind))
	{
		last_pokers_count = int(cpoker[2].kind);
		last_pokers_type = pkr_type::THRWHTWO;
		return true;
	}
	return false;
}
//四带二
bool DdzRun::IsFourWithTwo(poker* cpoker, int length)
{
	int i = 0;
	while (i < 3)
	{
		if ((cpoker[i].kind == cpoker[i + 1].kind)
			&& (cpoker[i + 1].kind == cpoker[i + 2].kind)
			&& (cpoker[i + 2].kind == cpoker[i + 3].kind))
		{
			last_pokers_count = int(cpoker[i].kind);
			last_pokers_type = pkr_type::FURWHTWO;
			return true;
		}
		i++;
	}
	return false;
}
//判断顺子(5张以上)(从3到A，2和王不能用)的牌
bool DdzRun::IsContinual(poker* cpoker, int length)
{
	int i = 0;
	while (i < (length - 1))
	{
		if ((int)cpoker[i].kind != ((int)cpoker[i + 1].kind + 1))
			return 0;
		i++;
	}
	last_pokers_count = int(cpoker[length - 1].kind);
	last_pokers_type = pkr_type::CONTINUE;
	return true;
}
//判断王炸
bool DdzRun::IsTwoJoker(poker* cpoker, int length)
{
	if ((length == 2) && (cpoker[0].color == pkr_color::JOKER) && (cpoker[1].color == pkr_color::JOKER))
	{
		last_pokers_count = 1;
		last_pokers_type = pkr_type::TWOJOKER;
		return true;
	}
	return false;
}
//判断炸弹
bool DdzRun::IsBomb(poker* cpoker, int length)
{
	if ((cpoker[0].kind == cpoker[1].kind) && (cpoker[1].kind == cpoker[2].kind) && (cpoker[2].kind == cpoker[3].kind))
	{
		last_pokers_count = int(cpoker[length - 1].kind);
		last_pokers_type = pkr_type::FOURSAME;
		return true;
	}
	return false;
}
//连对(3双以上)(从3到A，2和王不能用)的对子
bool DdzRun::IsDoubleContinual(poker* cpoker, int length)
{
	int i = 0;
	//判断第一对相同  然后后续比较就能保证相同
	if (cpoker[i].kind != cpoker[i + 1].kind)
		return false;
	while (i < (length - 2))
	{
		if (((int)cpoker[i].kind != ((int)cpoker[i + 2].kind + 1))
			|| ((int)cpoker[i + 1].kind != ((int)cpoker[i + 3].kind + 1)))
			return false;
		i += 2;
	}
	last_pokers_count = int(cpoker[length - 1].kind);
	last_pokers_type = pkr_type::DOUBLECN;
	return true;
}
//飞机不带  至少2个连续大小(从3到A)的三张
bool DdzRun::IsPlaneWithzero(poker* cpoker, int length)
{
	int i = 0;
	//判断第一对相同  然后后续比较就能保证相同
	if ((cpoker[i].kind != cpoker[i + 1].kind) || (cpoker[i + 1].kind != cpoker[i + 2].kind))
		return false;
	while (i < (length - 3))
	{
		if (((int)cpoker[i].kind != ((int)cpoker[i + 3].kind + 1))
			|| ((int)cpoker[i + 1].kind != ((int)cpoker[i + 4].kind + 1))
			|| ((int)cpoker[i + 2].kind != ((int)cpoker[i + 5].kind + 1)))
			return false;
		i += 3;
	}
	last_pokers_count = int(cpoker[length - 1].kind);
	last_pokers_type = pkr_type::PLANEZRE;
	return true;
}
//飞机带单  至少2个连续大小(从3到A)的三张 在12张时有可能找到飞机带0认为是带单
bool DdzRun::IsPlaneWithone(poker* cpoker, int length)
{
	//两种情况：
	//1.单牌都在一边  例如: 1.1：KQJ|999888777|     1.2：|888777666|433
	//2.单牌两边都有  例如: J|999888777|53
	// 错误的: AAA222333568
	//寻找规则:
	//1.从0-（length-1)后面的中间的和前面的一样时 记录一次则找到一个三连
	//2.接下来需要有两个三连  且 每个三连比前者小1
	//3.此规则在12张时会误判飞机带0
	int i = 0;
	int dup_record = 0;
	int single_pokers_num = length / 4;
	while (i < (length - 1))
	{
		//找前后有没有相同的牌
		if ((cpoker[i].kind == cpoker[i + 1].kind) && (cpoker[i + 1].kind == cpoker[i + 2].kind))
		{//发现一次三连  立马将i+=3;
			if (dup_record != 0)
			{//前面有三连  需要比较是不是连续的
				if ((int(cpoker[i].kind) + 1) == int(cpoker[i - 3].kind))
				{
					dup_record++;
				}
				else
				{
					dup_record = 1;
				}
				
			}
			else
			{
				dup_record++;
			}
			//三带一嘛  几张单牌 就需要几个三连
			if (dup_record == single_pokers_num)
			{
				last_pokers_count = int(cpoker[i].kind);
				last_pokers_type = pkr_type::PLANEONE;
				return true;
			}
			i += 3;
		}
		else
		{
			if(i >= single_pokers_num)
				return false;
			dup_record = 0;
			i++;
		}
	}
	return false;
}
//飞机带对  至少2个连续大小(从3到A)的三张 此规则在15张时有可能找到飞机带0认为是带单，20张的时候会误判飞机带1
bool DdzRun::IsPlaneWithtwo(poker* cpoker, int length)
{
	//两种情况：
		//1.单牌都在一边  例如: 1.1：KKQQJJ|999888777|     1.2：|888777666|554433
		//2.单牌两边都有  例如: JJ|999888777|5533
		// 错误的: AAA222333556688
		//寻找规则:
		//1.从0-（length-1)后面的中间的和前面的一样时 记录一次则找到一个三连
		//2.接下来需要有两个三连  且 每个三连比前者小1
		//3.此规则在15张时会误判飞机带0，20张的时候会误判飞机带1
	int i = 0;
	int dup_record = 0;
	int plane_num = length / 5;
	while (i < (length - 1))
	{
		//找前后有没有相同的牌
		if ((cpoker[i].kind == cpoker[i + 1].kind) && (cpoker[i + 1].kind == cpoker[i + 2].kind))
		{//发现一次三连  立马将i+=3;
			if (dup_record != 0)
			{//前面有三连  需要比较是不是连续的
				if ((int(cpoker[i].kind) + 1) == int(cpoker[i - 3].kind))
				{
					dup_record++;
				}
				else
				{
					dup_record = 1;
				}

			}
			else
			{
				dup_record++;
			}
			//三带二嘛  几张单牌 就需要几个三连
			if (dup_record == plane_num)
			{
				last_pokers_count = int(cpoker[i].kind);
				last_pokers_type = pkr_type::PLANETWO;
				return true;
			}
			i += 3;
		}
		else
		{
			if (i >= (2 * plane_num))
				return false;
			dup_record = 0;
			i++;
		}
	}
	return false;
}

bool DdzRun::FuncOfLastJudgeWrapper(bool(DdzRun::*pf)(poker*, int),poker* p,int length)
{
	int poker_count = last_pokers_count;
	if ((this->*pf)(p, length))
	{
		if (poker_count < last_pokers_count)
			return true;
		else//出牌失败  得恢复原来的点数
			last_pokers_count = poker_count;
	}
	return false;
}

bool DdzRun::FuncOfLastTestWrapper(bool(DdzRun::*pf)(poker*, int), poker* p, int length)
{
	int poker_count = last_pokers_count;
	pkr_type pt = last_pokers_type;
	if ((this->*pf)(p, length))
	{
		//牌型判断成功  得恢复原来的点数和类型
		last_pokers_count = poker_count;
		last_pokers_type = pt;
		return true;
	}
	else
		return false;
}
