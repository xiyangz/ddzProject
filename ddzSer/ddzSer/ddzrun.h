#include "sock.h"
#include "struct_common.h"
#pragma once

class DdzRun
{
private:
	bool isRecvSequenceOver[PLAYER_NUMBER] = { false,false,false };//��ҽ���sequence�Ƿ����
	bool isRecvPokerOver[PLAYER_NUMBER] = { false,false,false };//��ҽ��ܿ����Ƿ����
	bool isRecvLooterpkOver[PLAYER_NUMBER] = { false,false,false };//��ҽ��ܵ������Ƿ����
	bool isPokeOver = false;//����Ƿ�������
	bool isPoke[PLAYER_NUMBER] = { false,false,false };//��ҳ��ƻ��ǲ�����  ��һλ��ұ������
	run_status status = run_status::BEGIN; // ��Ϸ�׶�
	int turn = 0;//Ŀǰ��˭�Ļغ�sequence
	int looter_sequence = 0;//������sequence ��Ϸ��ʼʱ���ȳ�  //�ɲ���Ҫ
	int remain_second = 15;//ʣ��ʱ��
	bool isLootOver = false;//�Ƿ񷢳���������������
	bool isloot[PLAYER_NUMBER] = { false,false,false };//���������������߲�������
	poker nPoker[PKR_COUNT];//��������
	poker threepk[3];//���ŵ�����
	int power = 10; //����
	poker handpoker[PLAYER_NUMBER][HANDPOKER_MAXSIZE];//��������������  ��id����
	int handpoker_number[PLAYER_NUMBER] = {0,0,0};//�������ʣ������

	poker lastedpoker[PLAYER_NUMBER][HANDPOKER_MAXSIZE];//�ϴ��Ѿ�������
	int lastedpoker_number[PLAYER_NUMBER] = {0,0,0};//�ϴ��Ѿ������Ƶ�����

	//poker* usedpoker[PLAYER_NUMBER][HANDPOKER_MAXSIZE];//�Ѿ�������
	//int usedpoker_number[PLAYER_NUMBER];//�Ѿ������Ƶ�����
	const int WAIT_TIME_MAX = 15;//����15��  ��Ȼ����Ϊ����

public:
	DdzRun(team_wrapper* ptw);
	~DdzRun();
	void Start();
	void OnReceive(const runHeader& rh,int sequence);
	void PlayerDisconnected(int sequence);
private:
	//��ÿ���׶� �������յ��ͻ�����Ϣ��Ҫ�Ĵ���
	void OnMsgBegin(int sequence,pk_type_begin type);
	void OnMsgDeal(int sequence, pk_type_deal type);
	void OnMsgLoot(int sequence, pk_type_loot type);
	void OnMsgPoke(int sequence, pk_type_poke type);
	void OnMsgEnd(int sequence, pk_type_end type);
public:
	playerData playerdata[PLAYER_NUMBER];//�����±����sequence
	
public:
	void SendRunStatus();//������Ϸ�׶�
	void SendTurn();//��Ϸ�غ�  ʹ��turn,status

	void WaitPlayerFinish(bool* bary);//�������޵ȴ�

	void SendSequence();//1.3 ��֪���˳��

	void InitPoker();  //2.1 ����������
	void SelectPoker();//2.2 ��˳��ѡ��
	void SendPoker();  //2.3 ���Ʋ��Ұ��Ʊ�����handpoker
	void AddThreeToHandpoker();//�����ŵ����Ƽӵ���������

	void Loot();//3������
	void SendLootRlt();//����turn �� islootÿ�غϸ���������Ҵ����������û������
	void SendLooterData();//�������������Ϣ

	void Poke();//4���ƽ׶�
	void SendRemainSecond();//����ʣ��ʱ��

	void EndPoker();    //��Ϸ����

	bool CheckPoker();//�������Ƿ����
	void ChangePokerData();//����������
	int GetLastTurn();//��ȡ�ϸ��غ�
	int GetNextTurn();//��ȡ�¸��غ�
	void PlayPoker();//��֪��ҳ�����
private:
	int ready_poke_pokers_pos[HANDPOKER_MAXSIZE];
	poker ready_poke_pokers[HANDPOKER_MAXSIZE];
	int ready_poke_pokers_num;//���غ����Ҫ�����Ƶ�����
	int last_pokers_count = 0; //�˻غ�֮ǰ���Ƶĵ���  ���ֵ����������ǰ��ĵ��� �򵥵ĳ��Ʋ�ͨ������ж�  �жϺ�����ı��ֵ
	pkr_type last_pokers_type = pkr_type::SINGLE; //�˻غ�֮ǰ���Ƶ�����  �жϺ�����ı��ֵ
	bool is_first_turn = true;//�ǲ��ǵ�һ�����ƵĻغ�
	int isContinueUnpoke = 0; //�ж��Ƿ���2�β���  �еĻ��ʹ���is_first_turn = true;
	//1.cpoker�Ѿ��Ӵ�С������,��J<Q<K<A<2<joker<JOKER
	//2.length�����Ѿ����ݹ����ж��ˣ������в����ж�
	//3.���Ժ�����ı� last_pokers_type��last_pokers_countֵ  �ж�ǰ������Ǳ��浽��
	//����  ��ȻֻҪ��һ�ű�Ȼ�� ����Ҫ��¼����������
	bool IsOne(poker* cpoker, int length);
	//�ж϶���
	bool IsTwoSame(poker* cpoker, int length);
	//�ж�������ͬ
	bool IsThreeSame(poker* cpoker, int length);
	//�ж�����һ
	bool IsThreeWithOne(poker* cpoker, int length);
	//�ж�������
	bool IsThreeWithTwo(poker* cpoker, int length);
	//�ж��Ĵ���
	bool IsFourWithTwo(poker* cpoker, int length);
	//�ж�˳��
	bool IsContinual(poker* cpoker, int length);
	//�ж���ը
	bool IsTwoJoker(poker* cpoker, int length);
	//�ж�ը��
	bool IsBomb(poker* cpoker, int length);
	//�ж�����
	bool IsDoubleContinual(poker* cpoker, int length);
	//�жϲ������������Ϸɻ�   666555
	bool IsPlaneWithzero(poker* cpoker, int length);
	//�жϴ����ɻ�       55566678 35556667  34555666
	bool IsPlaneWithone(poker* cpoker, int length);
	//�жϴ�˫�ɻ�       5556667788 3355566677  3344555666
	bool IsPlaneWithtwo(poker* cpoker, int length);
	//���ִ�С�ȽϺ���
	bool FuncOfLastJudgeWrapper(bool (DdzRun::*pf)(poker*, int), poker* p, int length);
	//���ִ�С���Ժ���
	bool FuncOfLastTestWrapper(bool (DdzRun::*pf)(poker*, int), poker* p, int length);
	bool gspoke[PLAYER_NUMBER];//�Ƿ��ܵ�game_status POKE
	bool enpoker[PLAYER_NUMBER];
private:
	team_wrapper* m_pRunTeam;
	bool isPlayerCon[PLAYER_NUMBER] = { true,true,true };
};