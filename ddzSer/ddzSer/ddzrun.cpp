#include"pch.h"
#include"ddzrun.h"
#include "team_manager.h"



DdzRun::DdzRun(team_wrapper* ptw)
{
	m_pRunTeam = ptw;
	//����sequence
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		playerdata[i].sequence = i;
	}
	//�����ȡ���λ��
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
{/* 1.��ʼ��Ϸ(BEGIN)
		1.0������Ϸ�׶�
	    1.1��֪���sequence
		1.2�ȴ���ҽ���sequence�ɹ�	
	2.���� DEAL
		2.0�ı�׶β�������Ϸ�׶�
		2.1��ʼ������
		2.2ѡ��
		2.3����
		2.4�ȴ���ҽ��ܿ������
	3.������ LOOT
		3.0�ı�׶β�������Ϸ�׶�
		3.1ÿλ���½������������
		3.2�������������͵�����
		3.3�ȴ���ҽ��ܵ��������
	4.���� POKE
	5.���� ���� END*/
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
		//�յ������� ˵���������  �������û�յ���Ҫ�ж�Ϊ����
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
		cout << "OnMsgBegin sequence ��Χ����" << endl;
		return;
	}
	if (type == pk_type_begin::RECV_SEQ_SUC) {
		isRecvSequenceOver[sequence] = true;
	}
}

void DdzRun::OnMsgDeal(int sequence, pk_type_deal type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgDeal sequence ��Χ����" << endl;
		return;
	}
	if (type == pk_type_deal::ENPOKE) {
		isRecvPokerOver[sequence] = true;
	}
}

void DdzRun::OnMsgLoot(int sequence, pk_type_loot type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgLoot sequence ��Χ����" << endl;
		return;
	}
	//loot�׶��κ��˿�����Ϣ
	/*if (sequence != turn) {
		cout << "OnMsgLoot sequence�뵱ǰ�غϲ�����" << endl;
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
		cout << "OnMsgPoke sequence ��Χ����" << endl;
		return;
	}
	if (sequence != turn) {
		cout << "OnMsgPoke sequence�뵱ǰ�غϲ�����" << endl;
		return;
	}

	if (type == pk_type_poke::ENPLAY) {
		//����
		playerdata[sequence].psock->Receive(&ready_poke_pokers_num, sizeof(int));
		for (int i = 0,pos=0; i < ready_poke_pokers_num; i++) {
			playerdata[sequence].psock->Receive(&pos, sizeof(int));
			if (pos >= HANDPOKER_MAXSIZE || pos < 0)
			{
				cout << "OnMsgPoke ���ܵĿ���pos�����Ϲ���" << endl;
				return;
			}
			ready_poke_pokers_pos[i] = pos;
			ready_poke_pokers[i] = handpoker[sequence][pos];
		}
		isPokeOver = true;
	}
	else if (type == pk_type_poke::UNPLAY) {
		//����
		ready_poke_pokers_num = 0;
		isPokeOver = true;
	}
}

void DdzRun::OnMsgEnd(int sequence, pk_type_end type)
{
	if (sequence >= PLAYER_NUMBER) {
		cout << "OnMsgEnd sequence ��Χ����" << endl;
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
	//���ƽ׶�Ҫ�������  �˻غϵ�����ǲ�������
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
	{//��ҵ��ߴ���
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
	//����  ��ʼ�� ����
	int selectint[PKR_COUNT];
	for (int i = 0; i < PKR_COUNT; i++)
		selectint[i] = i;
	int remain = PKR_COUNT - 1;//ʣ��λ��
	int belong = 0;
	srand((unsigned int)GetTickCount64());
	for (int i = 0; i < PKR_COUNT - 3; i++)
	{//�������51����
		int pos = rand() % (remain + 1);
		nPoker[selectint[pos]].belong = (pkr_belong)belong;
		selectint[pos] = selectint[remain];
		remain--;
		belong++;
		if (belong == 3)
			belong = 0;
	}

	for (int i = 0; i < 3; i++)
	{//��������ǵ�����
		nPoker[selectint[i]].belong = pkr_belong::THREE;
	}

	for (int i = 0,j=0; i < PKR_COUNT; i++) {
		//��ֱ�������渳ֵ  ����Ϊ����˳����
		if (nPoker[i].belong == pkr_belong::THREE) {
			threepk[j] = nPoker[i];
			j++;
		}
	}
}

void DdzRun::SendPoker()
{
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{//���������������
		handpoker_number[i] = 17;
		//��֪��ҽ�Ҫ���Ϳ���
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
		{//kindԽ������Խǰ
			handpoker[looter_sequence][handpokerIndex + pos_move] = threepk[threepokerIndex];
			threepokerIndex--;
			pos_move--;
		}
		else if (handpoker[looter_sequence][handpokerIndex].kind == threepk[threepokerIndex].kind) {
			if (handpoker[looter_sequence][handpokerIndex].color > threepk[threepokerIndex].color) {
				//��ɫԽС ����Խǰ
				handpoker[looter_sequence][handpokerIndex + pos_move] = handpoker[looter_sequence][handpokerIndex];
				handpokerIndex--;
			}
			else {
				handpoker[looter_sequence][handpokerIndex + pos_move] = threepk[threepokerIndex];
				threepokerIndex--;
				pos_move--;
			}
		}
		else {//kindԽС������Խ��
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
{//����������1. ��������������  ���Ĵ�
 //           2. ����1��ʱ   ������
 //           3. û����ʱ   ���ѡһ��������
	int delay_msec;//�� ��/10 ���Ǻ���
	turn = 0;
	for (int i = 0, j = 0; j < PLAYER_NUMBER + 1; i++, j++)
	{
		isLootOver = false;
		delay_msec = 0;
		remain_second = 15;
		//��֪��ҵ�ǰ�غ�
		SendTurn();
		//�ȴ���Ҳ���
		while (!isLootOver)
		{
			if (delay_msec == 159)
			{//��ʱ  ����������Ĭ�ϲ���
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
		//��֪������ҵ�ǰ������������
		SendLootRlt();
		//����������������ж��»غ���˭
		if (i == 2)
		{
			int lootNum = 0;//������������������
			int index = 0;
			for (int k = 0; k < PLAYER_NUMBER; k++)
			{
				if (isloot[k])
					++lootNum;
			}
			//�ж�����
			switch (lootNum)
			{
			case 0://û����  ���ѡ
			{
				CTime time = CTime::GetTickCount();
				srand(time.GetSecond() * time.GetMinute() * time.GetHour());
				isloot[rand() % PLAYER_NUMBER] = true;
				++j;
			}
			break;
			case 1:
				++j;//һ������   ֻ����3��
				break;
			case 2:
			{
				for (int j = 0; j < PLAYER_NUMBER; j++)
				{//ѡ�����һ����������  �ٴ�������
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
			turn = GetNextTurn();//�����ı�غ�

	}

	
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		if (isloot[i])
		{
			//�������Ϊ����
			playerdata[i].isLandOwner = true;
			//���õ���sequence
			looter_sequence = i;
			//����������Ƽ�3 �ƿ��������������
			handpoker_number[i] += 3;
			for (int j = 0; j < PKR_COUNT; j++)
			{
				if (nPoker[j].belong == pkr_belong::THREE)
					nPoker[j].belong = (pkr_belong)i;
			}
			//���������handpoker����������
			AddThreeToHandpoker();
			break;
		}
	}
	//��֪��ҵ�������
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
		//�ȴ����� �� ֪ͨʣ��ʱ��
		SendTurn();
		while (!isRightPoke)
		{
			isPokeOver = false;
			while (!isPokeOver)
			{
				if (delay_msec == 159)
				{//��ʱ  ����������Ĭ�ϲ���
					isPokeOver = true;
					if (is_first_turn){
						//��һ���˲��ܲ�����
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
			//�Ѿ�����
			isRightPoke = CheckPoker();//�жϳ����Ƿ����
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
	//��������ҷ���ʣ��ʱ��
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
	//��������sock��Ϸ����  isRun����Ϊfalse
	//������Ϸ����  ����������ȡ��׼��
}

bool DdzRun::CheckPoker()
{
	int length = ready_poke_pokers_num;
	poker* p = ready_poke_pokers;
	int poker_count = 0;//����
	//���timepoker�е���
	//����
	if (is_first_turn)
	{	 //һ������  ���ű���һ��  ���ű���һ��  ���ſ�Ϊ����һ��Ϊ�ĸ���ͬ  ���� ������  ������   
		 //���ŷɻ�  �Ĵ���  ���Զ� ������   ����������  ����
		 //A����13 2����14 
		switch (length)
		{
			//�������ж�  ������߻����ж�
		case 1:
			return IsOne(p,length);
		case 2://һ����  ���� ��С��
			return (IsTwoSame(p, length) || IsTwoJoker(p, length));
		case 3://������ͬ
			return IsThreeSame(p, length);
		case 4: // ���Ż�  ����һ   ���ж�Ҫ��������һ
			return (IsBomb(p, length) || IsThreeWithOne(p, length));
		case 5://������ ˳��
			return (IsThreeWithTwo(p, length) || IsContinual(p, length));
		case 6://�Ĵ���   ˳��  ����  �ɻ�
			return (IsFourWithTwo(p, length) || IsContinual(p, length)
				|| IsDoubleContinual(p, length) || IsPlaneWithzero(p, length));
		case 7://˳��
			return IsContinual(p, length);
		case 8://�ɻ�  ˳��  ����
			return (IsPlaneWithtwo(p, length) || IsContinual(p, length) || IsDoubleContinual(p, length));
		case 9://�ɻ�  ˳��
			return (IsContinual(p, length) || IsPlaneWithzero(p, length));
		case 10://���� ˳�� �ɻ�����
			return (IsContinual(p, length) || IsDoubleContinual(p, length) || IsPlaneWithtwo(p, length));
		case 11://˳��  
			return IsContinual(p, length);
		case 12://˳�� ���� �ɻ���0 �ɻ���1
			return (IsContinual(p, length) || IsDoubleContinual(p, length)
				|| IsPlaneWithzero(p, length) || IsPlaneWithone(p, length));
		case 14://����
			return IsDoubleContinual(p, length);
		case 15://�ɻ���0 �ɻ���2
			return (IsPlaneWithzero(p,length)||IsPlaneWithtwo(p, length));
		case 16://���� �ɻ���һ
			return (IsDoubleContinual(p, length) || IsPlaneWithone(p, length));
		case 18://����
			return IsDoubleContinual(p, length);
		case 20://���� �ɻ���һ �ɻ�����
			return (IsDoubleContinual(p, length) || IsPlaneWithone(p, length) || IsPlaneWithtwo(p, length));
		default:
			return false;
		}
	}
	else//���� //������Ҫ�����һ�γ��Ƶ��Ʊȴ�С
	{
		//���ֿ��Բ����� 
		if (length == 0)
			return true;
		else
		{//�������Ŀ϶��ǳ�����
			//ǰ������ը  �޽�
			if ((last_pokers_type == pkr_type::TWOJOKER))
				return false;
			//�Լ�����ը 
			else if((length==2)&&(IsTwoJoker(p,length)))
				return true;
			//ǰ�߳�����ը�� �����ը���ұ����� ���� ��ը(�����������Ѿ�������ը��)
			else if (last_pokers_type == pkr_type::FOURSAME)
			{
				if (length == 4)
				{
					int poker_count = last_pokers_count;
					if (IsBomb(p, length))
					{
						if (poker_count < last_pokers_count)
							return true;
						else//����ʧ��  �ûָ�ԭ���ĵ���
							last_pokers_count = poker_count;
					}
				}
				return false;
			}
			//ǰ�߳������������ͣ����������ը(�����������Ѿ�������ը��) ը�� ����ͬ���͵���ǰ�ߴ�
			else
			{
				if((length==4)&&(IsBomb(p,length)))
					return true;
				else
				{
					//��ȡ�ϴγ�������
					int last_turn = GetLastTurn();
					int last_two_turn = (last_turn - 1) < 0 ? 2 : (last_turn - 1);
					int last_length = lastedpoker_number[last_turn] == 0 ? lastedpoker_number[last_two_turn] : lastedpoker_number[last_turn];
					//�ڴˣ����������ͬ�Ͳ��ܳ�
					if (length != last_length)
						return false;
					poker_count = last_pokers_count;
					switch (last_pokers_type)
					{
					//����һ����Ȼ���㺯��
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
					case pkr_type::PLANEONE://�����Գ��� ���ǻ���ǰ����жϵ� 
						return FuncOfLastJudgeWrapper(&DdzRun::IsPlaneWithone, p, length);
						break;
					case pkr_type::PLANETWO:
						if (length == 15)
						{//�ų��Ƿɻ���0
							if (FuncOfLastTestWrapper(&DdzRun::IsPlaneWithzero, p, length))
								return false;
						}
						else if (length == 20)
						{//�ų��Ƿɻ���1
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
	//������Ƽ���  �����ݸ���
	//������ӵ�lastedpoker ����֪���
	//�����û���� ��isContinueUnpoke++
	if (ready_poke_pokers_num == 0)
	{
		lastedpoker_number[turn] = ready_poke_pokers_num;
		isContinueUnpoke++;
		//�������β���  �»غ����ó����� �ж�����
		if (isContinueUnpoke == 2)
		{
			is_first_turn = true;
			isContinueUnpoke = 0;
		}
	}
	else
	{
		//�����ϻغϳ�����
		lastedpoker_number[turn] = ready_poke_pokers_num;
		for (int k = 0; k < ready_poke_pokers_num; k++)
		{
			lastedpoker[turn][k] = ready_poke_pokers[k];
		}
		//�ı��������
		 //�ı���������
		handpoker_number[turn] -= ready_poke_pokers_num;
		int pos = 0;
		for (int i = 0; i < handpoker_number[turn]; i++) {
			while (((i+pos) == ready_poke_pokers_pos[pos]) && (pos < ready_poke_pokers_num)) {
				pos++;
			}
			if (pos > 0)
				handpoker[turn][i] = handpoker[turn][i + pos];
		}
		//������  �ж��������� �»غ�Ҳ������������
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
//��֪��� ���غϳ������  Ҫ��֪��ҿ���
void DdzRun::PlayPoker	()
{
	for (int j = 0; j < PLAYER_NUMBER; j++) 
	{
		playerdata[j].psock->SendrunHeader(status, turn, (int)pk_type_poke::ENPOKE);
		//��֪��ҳ��������ͺ͵���
		playerdata[j].psock->Send(&last_pokers_type, sizeof(int));
		playerdata[j].psock->Send(&last_pokers_count, sizeof(int));
		//��֪��ҳ��Ŀ���
		playerdata[j].psock->Send(&ready_poke_pokers_num, sizeof(int));
		for (int i = 0; i < ready_poke_pokers_num; i++)
		{
			playerdata[j].psock->Send(&ready_poke_pokers[i], sizeof(poker));
		}
	}
}
//����
bool DdzRun::IsOne(poker* cpoker, int length)
{
	last_pokers_count = int(cpoker[0].kind);
	last_pokers_type = pkr_type::SINGLE;
	return true;
}
//�ж϶���
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
//�ж�������ͬ
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
//����һ  ���Ż��������  �ڴ��ж�֮ǰ�ж��Ƿ��ǻ�
bool DdzRun::IsThreeWithOne(poker* cpoker, int length)
{//����һ�ܴ���С��
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
//������
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
//�Ĵ���
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
//�ж�˳��(5������)(��3��A��2����������)����
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
//�ж���ը
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
//�ж�ը��
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
//����(3˫����)(��3��A��2����������)�Ķ���
bool DdzRun::IsDoubleContinual(poker* cpoker, int length)
{
	int i = 0;
	//�жϵ�һ����ͬ  Ȼ������ȽϾ��ܱ�֤��ͬ
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
//�ɻ�����  ����2��������С(��3��A)������
bool DdzRun::IsPlaneWithzero(poker* cpoker, int length)
{
	int i = 0;
	//�жϵ�һ����ͬ  Ȼ������ȽϾ��ܱ�֤��ͬ
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
//�ɻ�����  ����2��������С(��3��A)������ ��12��ʱ�п����ҵ��ɻ���0��Ϊ�Ǵ���
bool DdzRun::IsPlaneWithone(poker* cpoker, int length)
{
	//���������
	//1.���ƶ���һ��  ����: 1.1��KQJ|999888777|     1.2��|888777666|433
	//2.�������߶���  ����: J|999888777|53
	// �����: AAA222333568
	//Ѱ�ҹ���:
	//1.��0-��length-1)������м�ĺ�ǰ���һ��ʱ ��¼һ�����ҵ�һ������
	//2.��������Ҫ����������  �� ÿ��������ǰ��С1
	//3.�˹�����12��ʱ�����зɻ���0
	int i = 0;
	int dup_record = 0;
	int single_pokers_num = length / 4;
	while (i < (length - 1))
	{
		//��ǰ����û����ͬ����
		if ((cpoker[i].kind == cpoker[i + 1].kind) && (cpoker[i + 1].kind == cpoker[i + 2].kind))
		{//����һ������  ����i+=3;
			if (dup_record != 0)
			{//ǰ��������  ��Ҫ�Ƚ��ǲ���������
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
			//����һ��  ���ŵ��� ����Ҫ��������
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
//�ɻ�����  ����2��������С(��3��A)������ �˹�����15��ʱ�п����ҵ��ɻ���0��Ϊ�Ǵ�����20�ŵ�ʱ������зɻ���1
bool DdzRun::IsPlaneWithtwo(poker* cpoker, int length)
{
	//���������
		//1.���ƶ���һ��  ����: 1.1��KKQQJJ|999888777|     1.2��|888777666|554433
		//2.�������߶���  ����: JJ|999888777|5533
		// �����: AAA222333556688
		//Ѱ�ҹ���:
		//1.��0-��length-1)������м�ĺ�ǰ���һ��ʱ ��¼һ�����ҵ�һ������
		//2.��������Ҫ����������  �� ÿ��������ǰ��С1
		//3.�˹�����15��ʱ�����зɻ���0��20�ŵ�ʱ������зɻ���1
	int i = 0;
	int dup_record = 0;
	int plane_num = length / 5;
	while (i < (length - 1))
	{
		//��ǰ����û����ͬ����
		if ((cpoker[i].kind == cpoker[i + 1].kind) && (cpoker[i + 1].kind == cpoker[i + 2].kind))
		{//����һ������  ����i+=3;
			if (dup_record != 0)
			{//ǰ��������  ��Ҫ�Ƚ��ǲ���������
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
			//��������  ���ŵ��� ����Ҫ��������
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
		else//����ʧ��  �ûָ�ԭ���ĵ���
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
		//�����жϳɹ�  �ûָ�ԭ���ĵ���������
		last_pokers_count = poker_count;
		last_pokers_type = pt;
		return true;
	}
	else
		return false;
}
