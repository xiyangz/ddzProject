package com.example.ddzclient.data;

import java.io.IOException;

public class data_run {
    public class pkr_kind
    {
        public static final int POKER_3 = 0;
        public static final int POKER_4 = 1;
        public static final int POKER_5 = 2;
        public static final int POKER_6 = 3;
        public static final int POKER_7 = 4;
        public static final int POKER_8 = 5;
        public static final int POKER_9 = 6;
        public static final int POKER_1 = 7;
        public static final int POKER_J = 8;
        public static final int POKER_Q = 9;
        public static final int POKER_K = 10;
        public static final int POKER_A = 11;
        //这么设计是为了斗地主判断规则  3-A可以连续  A到2不能连续
        public static final int POKER_2 = 13;
        public static final int POKER_L_JOKER = 22;//小王
        public static final int POKER_B_JOKER = 23;//大王
    };

    public class pkr_color
    {
        public static final int SPADE = 0;     //黑桃
        public static final int HEARTS = 13;   //红桃
        public static final int CLUB = 26;      //梅花
        public static final int DIAMONDS = 39;  //方块
        public static final int JOKER = 30;     //小王
    };

    public class pkr_type
    {                                           //满足张数 n取1，2，3 ...
        public static final int SINGLE = 0;     //1     //单张
        public static final int TWOSAME = 1;    //2     //一对(两张相同)//两张大小相同的牌，从3(最小)到2(最大)
        public static final int THRWHZRE = 2; 	//3     //三带零
        public static final int THRWHONE = 3; 	//4     //三带一 （三张并带上任意一张牌）
        public static final int THRWHTWO = 4; 	//5     //三带二（一对）//一对要符合一对的规则
        public static final int CONTINUE = 5; 	//5-12  //顺子(5张以上)(从3到A，2和王不能用)的牌
        public static final int DOUBLECN = 6;   //2n<=20//连对(3双以上)(从3到A，2和王不能用)的对子
        public static final int PLANEZRE = 7;   //6<=3n<=18//飞机 不带  至少2个连续大小(从3到A)的三张
        public static final int PLANEONE = 8;   //8<=4n<=20//飞机 带单  至少2个连续大小(从3到A)的三张
        public static final int PLANETWO = 9;   //10<=5n<=20//飞机 带对  至少2个连续大小(从3到A)的三张
        public static final int FURWHTWO = 10; 	//6	    //四带二//有两种牌型，一个四张带上两个单张，或一个四张带上两对，能被比自己小的炸弹盖过【四张带二张和四张带二对属于不同的牌型，不能彼此盖过】
        public static final int FOURSAME = 11; 	//4	    //炸弹
        public static final int TWOJOKER = 12;  //2     //王炸，这是最大的组合
    };

    public class pkr_sequence
    {
        public static final int PLAYER1 = 0;  //玩家一
        public static final int PLAYER2 = 1;  //玩家二
        public static final int PLAYER3 = 2;  //玩家三
    };

    public class pkr_belong
    {
        public static final int PLAYER1 = 0;  //玩家一
        public static final int PLAYER2 = 1;  //玩家二
        public static final int PLAYER3 = 2;  //玩家三
        public static final int THREE = 3;	 //地主的三张牌
        public static final int PLAYED = 4;	 //已经打出
    };

    public class poker
    {
        public int kind;//
        public int color;
        public int belong;//分配

        public void recv(CppInputStream is) throws IOException {
            kind = is.readInt();
            color = is.readInt();
            belong = is.readInt();
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(kind);
            os.writeInt(color);
            os.writeInt(belong);
        }
    };

    public class run_status
    {
        public static final int BEGIN = 0; //开始前的一些操作
        public static final int DEAL = 1;  //发牌阶段
        public static final int LOOT = 2;  //抢地主阶段
        public static final int POKE = 3;  //打牌阶段
        public static final int END = 4;  //游戏结束
        public static final int NORM = 5; //特定阶段 心跳 和 改变游戏阶段
    };

    public class pk_type_norm
    {
        public static final int HEARTBEAT = 66;//心跳
        public static final int GAMESTATUS =67; //游戏阶段
    };

    public class pk_type_begin
    {
        public static final int CHECK_SUC = 0; //检查身份成功
        public static final int CHECK_FAIL = 1;//检查身份失败
        public static final int SEQUENCE = 2; //玩家顺序
        public static final int RECV_SEQ_SUC = 3;//接受玩家顺序成功

    };
    public class pk_type_deal
    {
        public static final int POKER = 0;   //发送卡牌
        public static final int ENPOKE = 1;  //卡牌接受完毕
    };
    public class pk_type_loot
    {
        public static final int ENLOOT = 0;	  //抢地主
        public static final int UNLOOT = 1;  //不抢
        public static final int TIME = 2;   //剩余时间(包含回合)
        public static final int LOOT_RLT = 3;//抢地主结果
        public static final int RECV_PKR_SUC = 4;//接受卡牌成功
        public static final int TURN = 5;//游戏回合
    };
    public class pk_type_poke
    {
        public static final int ENPOKE = 0;   //出牌成功(服务器返回结果)
        public static final int UNPOKE = 1;   //出牌失败(服务器返回结果)
        public static final int ENPLAY = 2;   //出牌
        public static final int UNPLAY = 3;   //不出
        public static final int TIME = 4;    //剩余时间(包含回合）
        public static final int TURN = 5;//游戏回合
    };

    public class pk_type_end
    {
        public static final int NORM = 0;  //正常结束
        public static final int PALYER_DISCONNECT = 1;//玩家掉线
    };


    public class checkStruct
    {
        public int team_id;
        public int id;
        public char[] key = new char[data.KEY_SIZE + 1];

        public void recv(CppInputStream is) throws IOException {
            team_id = is.readInt();
            id = is.readInt();
            key = is.readString(data.KEY_SIZE + 1);
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(team_id);
            os.writeInt(id);
            os.writeString(key);
        }
    };

    public class looter_data
    {
        public int sequence;
        public poker[] threepk = new poker[3];

        public looter_data(){
            for(int i=0;i<3;i++){
                threepk[i] = new poker();
            }
        }

        public void recv(CppInputStream is) throws IOException {
            sequence = is.readInt();
            for(int i=0;i<3;i++) {
                threepk[i].recv(is);
            }
        }
//        public void send(CppOutputStream os) throws IOException{
//            os.writeInt(sequence);
//            for(int i=0;i<3;i++) {
//                threepk[i].send(os);
//            }
    };

    public class  runHeader
     {
        public int run_satus;
        public int turn_sq;
        public int type;//数据类型

        public void recv(CppInputStream is) throws IOException {
            run_satus = is.readInt();
            turn_sq = is.readInt();
            type = is.readInt();
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(run_satus);
            os.writeInt(turn_sq);
            os.writeInt(type);
        }
    };

    public static void sendRunHeader (int run_status, int turn_seq, int type,CppOutputStream ops)throws IOException
    {
        runHeader rh = new data_run().new runHeader();
        rh.run_satus = run_status;
        rh.turn_sq = turn_seq;
        rh.type = type;
        rh.send(ops);
    }

    public class  playerData
    {
        public int  sequence;//玩家的序号 本次游戏中的
        public boolean isLandOwner;//是否是地主
        public char[] key = new char[data.KEY_SIZE + 1];
        public data_common.trans_user_info user = new data_common().new trans_user_info();

        public void recv(CppInputStream is) throws IOException {
            sequence = is.readInt();
            isLandOwner = is.readBool();
            key = is.readString(data.KEY_SIZE + 1);
            user.recv(is);
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(sequence);
            os.writeBool(isLandOwner);
            os.writeString(key);
            user.send(os);
        }
    };


}
