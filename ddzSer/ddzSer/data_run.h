#pragma once
enum class pkr_kind
{
	POKER_3 = 0,
	POKER_4,
	POKER_5,
	POKER_6,
	POKER_7,
	POKER_8,
	POKER_9,
	POKER_10,
	POKER_J,
	POKER_Q,
	POKER_K,
	POKER_A,
	//这么设计是为了斗地主判断规则  3-A可以连续  A与2不能连续
	POKER_2 = 13,
	POKER_L_JOKER = 22,//小王
	POKER_B_JOKER//大王
};

enum class pkr_color
{
	SPADE = 0,      //黑桃 0 b
	HEARTS = 13,    //红桃 1 r
	CLUB = 26,      //梅花 2 m
	DIAMONDS = 39,  //方块 3 f
	JOKER = 30     //小王
};
//(1) 单张：大小顺序从3(最小)到大王(最大)；
//
//(2) 一对：两张大小相同的牌，从3(最小)到2(最大)；
//
//(3) 三张：三张大小相同的牌；
//
//(4) 三带一：三张并带上任意一张牌，例如6 - 6 - 6 - 8，根据三张的大小来比较，例如9 - 9 - 9 - 3盖过8 - 8 - 8 - A；
//
//(5) 三带一对：三张并带上一对，类似扑克中的副路(Full House)，
//             根据三张的大小来比较，例如Q - Q - Q - 6 - 6盖过10 - 10 - 10 - K - K；
//
//(6)顺子：至少5张连续大小(从3到A，2和怪不能用)的牌，例如8 - 9 - 10 - J - Q；
//
//(7) 连对：至少3个连续大小(从3到A，2和怪不能用)的对子，例如10 - 10 - J - J - Q - Q - K - K；
//
//(8) 三张的顺子：至少2个连续大小(从3到A)的三张，例如4 - 4 - 4 - 5 - 5 - 5；
//
//(9) 飞机：每个三张都带上额外的一个单张，例如7 - 7 - 7 - 8 - 8 - 8 - 3 - 6，尽管三张2不能用，但能够带上单张2和怪；
//
//(10) 三张带一对的顺子：每个三张都带上额外的一对，只需要三张是连续的就行，
//                     例如8 - 8 - 8 - 9 - 9 - 9 - 4 - 4 - J - J，尽管三张2不能用，
//                     但能够带上一对2【不能带一对怪，因为两张怪的大小不一样】，这里要注意，
//                     三张带上的单张和一对不能是混合的，例如3 - 3 - 3 - 4 - 4 - 4 - 6 - 7 - 7就是不合法的；
//
//(11) 炸弹：四张大小相同的牌，炸弹能盖过除火箭外的其他牌型，大的炸弹能盖过小的炸弹；
//
//(12) 王炸：一对王，这是最大的组合，能够盖过包括炸弹在内的任何牌型；
//
//(13) 四带二：有两种牌型，一个四张带上两个单张，
//            例如6 - 6 - 6 - 6 - 8 - 9，或一个四张带上两对，
//            例如J - J - J - J - 9 - 9 - Q - Q，四带二只根据四张的大小来比较，
//            只能盖过比自己小的同样牌型的四带二【四张带二张和四张带二对属于不同的牌型，不能彼此盖过】，
//            不能盖过其他牌型，四带二
enum class pkr_type
{                   //满足张数 n取1，2，3 ...     
	SINGLE,         //1     //单张  
	TWOSAME,        //2     //一对(两张相同)//两张大小相同的牌，从3(最小)到2(最大)
	THRWHZRE,		//3     //三带零
	THRWHONE,		//4     //三带一 （三张并带上任意一张牌）
	THRWHTWO,		//5     //三带二（一对）//一对要符合一对的规则
	CONTINUE,		//5-12  //顺子(5张以上)(从3到A，2和王不能用)的牌
	DOUBLECN,       //2n<=20//连对(3双以上)(从3到A，2和王不能用)的对子
	PLANEZRE,       //6<=3n<=18//飞机 不带  至少2个连续大小(从3到A)的三张
	PLANEONE,       //8<=4n<=20//飞机 带单  至少2个连续大小(从3到A)的三张
	PLANETWO,       //10<=5n<=20//飞机 带对  至少2个连续大小(从3到A)的三张
	FURWHTWO,		//6	    //四带二//有两种牌型，一个四张带上两个单张，或一个四张带上两对，能被比自己小的炸弹盖过【四张带二张和四张带二对属于不同的牌型，不能彼此盖过】
	FOURSAME,		//4	    //炸弹
	TWOJOKER        //2     //王炸，这是最大的组合
};

enum pkr_sequence
{
	PLAYER1 = 0,  //玩家一
	PLAYER2 = 1,  //玩家二
	PLAYER3 = 2,  //玩家三
};

enum class pkr_belong
{
	PLAYER1 = 0,  //玩家一
	PLAYER2 = 1,  //玩家二
	PLAYER3 = 2,  //玩家三
	THREE,	 //地主的三张牌
	PLAYED	 //已经打出
};

struct poker
{
	pkr_kind kind;//
	pkr_color color;
	pkr_belong belong;//分配  
};

enum class run_status
{
	BEGIN, //开始前的一些操作
	DEAL, //发牌阶段
	LOOT, //抢地主阶段
	POKE,  //打牌阶段
	END,  //游戏结束
	NORM, //特定阶段 心跳 和 改变游戏阶段
};

enum class pk_type_norm
{
	HEARTBEAT = 66,//心跳包
	GAMESTATUS, //游戏阶段
};
enum  class pk_type_begin
{
	CHECK_SUC, //检查身份成功
	CHECK_FAIL,//检查身份失败
	SEQUENCE, //玩家顺序
	RECV_SEQ_SUC,//接受玩家顺序成功
};
enum  class pk_type_deal
{
	POKER,   //发送卡牌
	ENPOKE,  //卡牌接受完毕
};
enum  class pk_type_loot
{
	ENLOOT,	  //抢地主
	UNLOOT,   //不抢
	TIME,    //剩余时间(包含回合)
	LOOT_RLT,//抢地主结果
	RECV_PKR_SUC,//接受卡牌成功
	TURN,    //谁的回合
};
enum class pk_type_poke
{
	ENPOKE,   //出牌成功(服务器返回结果)
	UNPOKE,   //出牌失败(服务器返回结果)
	ENPLAY,   //出牌
	UNPLAY,   //不出
	TIME,    //剩余时间(包含回合）
	TURN,    //谁的回合
};

enum class pk_type_end
{
	NORM , //正常结束
	PALYER_DISCONNECT,//玩家掉线
};

struct checkStruct
{
	int team_id;
	int id;
	TCHAR key[KEY_SIZE + 1];
};

struct looter_data
{
	int sequence;
	poker threepk[3];
};

struct runHeader
{
	run_status rs;
	int sq;
	int type;//数据类型
};

extern class sRunSock;
struct playerData
{
	int id;
	int sequence;
	bool isLandOwner = false;
	sRunSock* psock = nullptr;
};




