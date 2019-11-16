#include "sock.h"
#include "struct_common.h"
#pragma once

class DdzRun
{
private:
	bool isRecvSequenceOver[PLAYER_NUMBER] = { false,false,false };//玩家接受sequence是否完毕
	bool isRecvPokerOver[PLAYER_NUMBER] = { false,false,false };//玩家接受卡牌是否完毕
	bool isRecvLooterpkOver[PLAYER_NUMBER] = { false,false,false };//玩家接受地主牌是否完毕
	bool isPokeOver = false;//玩家是否出牌完毕
	bool isPoke[PLAYER_NUMBER] = { false,false,false };//玩家出牌还是不出牌  第一位玩家必须出牌
	run_status status = run_status::BEGIN; // 游戏阶段
	int turn = 0;//目前是谁的回合sequence
	int looter_sequence = 0;//地主的sequence 游戏开始时他先出  //可不需要
	int remain_second = 15;//剩余时间
	bool isLootOver = false;//是否发出抢或不抢地主请求
	bool isloot[PLAYER_NUMBER] = { false,false,false };//请求是抢地主或者不抢地主
	poker nPoker[PKR_COUNT];//总牌数据
	poker threepk[3];//三张地主牌
	int power = 10; //积分
	poker handpoker[PLAYER_NUMBER][HANDPOKER_MAXSIZE];//三个人手牌数据  按id排序
	int handpoker_number[PLAYER_NUMBER] = {0,0,0};//玩家手牌剩余数量

	poker lastedpoker[PLAYER_NUMBER][HANDPOKER_MAXSIZE];//上次已经出的牌
	int lastedpoker_number[PLAYER_NUMBER] = {0,0,0};//上次已经出的牌的数量

	//poker* usedpoker[PLAYER_NUMBER][HANDPOKER_MAXSIZE];//已经出的牌
	//int usedpoker_number[PLAYER_NUMBER];//已经出的牌的数量
	const int WAIT_TIME_MAX = 15;//最多等15秒  不然就认为掉线

public:
	DdzRun(team_wrapper* ptw);
	~DdzRun();
	void Start();
	void OnReceive(const runHeader& rh,int sequence);
	void PlayerDisconnected(int sequence);
private:
	//在每个阶段 服务器收到客户端消息需要的处理
	void OnMsgBegin(int sequence,pk_type_begin type);
	void OnMsgDeal(int sequence, pk_type_deal type);
	void OnMsgLoot(int sequence, pk_type_loot type);
	void OnMsgPoke(int sequence, pk_type_poke type);
	void OnMsgEnd(int sequence, pk_type_end type);
public:
	playerData playerdata[PLAYER_NUMBER];//数组下标就是sequence
	
public:
	void SendRunStatus();//发生游戏阶段
	void SendTurn();//游戏回合  使用turn,status

	void WaitPlayerFinish(bool* bary);//不能无限等待

	void SendSequence();//1.3 告知玩家顺序

	void InitPoker();  //2.1 设置牌数据
	void SelectPoker();//2.2 按顺序选牌
	void SendPoker();  //2.3 发牌并且把牌保存在handpoker
	void AddThreeToHandpoker();//将三张地主牌加到地主手牌

	void Loot();//3抢地主
	void SendLootRlt();//根据turn 和 isloot每回合告诉所有玩家此玩家抢还是没抢地主
	void SendLooterData();//发生地主玩家信息

	void Poke();//4打牌阶段
	void SendRemainSecond();//发送剩余时间

	void EndPoker();    //游戏结束

	bool CheckPoker();//检查出牌是否合理
	void ChangePokerData();//更改牌数据
	int GetLastTurn();//获取上个回合
	int GetNextTurn();//获取下个回合
	void PlayPoker();//告知玩家出的牌
private:
	int ready_poke_pokers_pos[HANDPOKER_MAXSIZE];
	poker ready_poke_pokers[HANDPOKER_MAXSIZE];
	int ready_poke_pokers_num;//本回合玩家要出的牌的数量
	int last_pokers_count = 0; //此回合之前出牌的点数  后手点数必须大于前面的点数 简单的出牌不通过这个判断  判断函数会改变此值
	pkr_type last_pokers_type = pkr_type::SINGLE; //此回合之前出牌的类型  判断函数会改变此值
	bool is_first_turn = true;//是不是第一个出牌的回合
	int isContinueUnpoke = 0; //判断是否有2次不出  有的话就触发is_first_turn = true;
	//1.cpoker已经从大到小排序了,且J<Q<K<A<2<joker<JOKER
	//2.length必须已经根据规则判断了，函数中不会判断
	//3.所以函数会改变 last_pokers_type和last_pokers_count值  判断前请把它们保存到别处
	//单张  虽然只要是一张必然是 但需要记录点数和类型
	bool IsOne(poker* cpoker, int length);
	//判断对子
	bool IsTwoSame(poker* cpoker, int length);
	//判断三张相同
	bool IsThreeSame(poker* cpoker, int length);
	//判断三带一
	bool IsThreeWithOne(poker* cpoker, int length);
	//判断三带二
	bool IsThreeWithTwo(poker* cpoker, int length);
	//判断四带二
	bool IsFourWithTwo(poker* cpoker, int length);
	//判断顺子
	bool IsContinual(poker* cpoker, int length);
	//判断王炸
	bool IsTwoJoker(poker* cpoker, int length);
	//判断炸弹
	bool IsBomb(poker* cpoker, int length);
	//判断连对
	bool IsDoubleContinual(poker* cpoker, int length);
	//判断不带的两个以上飞机   666555
	bool IsPlaneWithzero(poker* cpoker, int length);
	//判断带单飞机       55566678 35556667  34555666
	bool IsPlaneWithone(poker* cpoker, int length);
	//判断带双飞机       5556667788 3355566677  3344555666
	bool IsPlaneWithtwo(poker* cpoker, int length);
	//后手大小比较函数
	bool FuncOfLastJudgeWrapper(bool (DdzRun::*pf)(poker*, int), poker* p, int length);
	//后手大小测试函数
	bool FuncOfLastTestWrapper(bool (DdzRun::*pf)(poker*, int), poker* p, int length);
	bool gspoke[PLAYER_NUMBER];//是否都受到game_status POKE
	bool enpoker[PLAYER_NUMBER];
private:
	team_wrapper* m_pRunTeam;
	bool isPlayerCon[PLAYER_NUMBER] = { true,true,true };
};