package com.example.ddzclient.data;

import java.io.IOException;

public class data {
    public static final int ACCOUNT_MAXSIZE = 10;
    public static final int PASSWORD_MAXSIZE = 16;
    public static final int NICKNAME_MAXSIZE = 10;
    public static final int TEAMNAME_MAXSIZE = 13;
    public static final int KEY_SIZE = 10;//密钥
    public static final int ONLINE_STATUS_ON = 1;
    public static final int ONLINE_STATUS_OFF = 0;
    public static final int PLAYER_NUMBER = 3;//玩家数量
    public static final int HANDPOKER_MAXSIZE = 20;//每位玩家最大拥有牌书数
    public static final int PKR_COUNT = 54;  //总牌数
    public static final int JOKER = 52;     //小王
    public static final int EXIT_CODE_OPENMAIN = 6;

    public class login_status {
        public static final int READY = 0;//准备阶段
        public static final int JOIN = 1; //组队阶段
        public static final int ALL = 2; //所有阶段
    }

    //数据包头的类型码
    public class return_type {
        public static final int NOTHING = 0; //空
        public static final int SUCCESS = 1;//成功
        public static final int FAILED = 2;  //失败
        public static final int ACCOUNT_EXISTED = 3; //账号存在阶段
        public static final int INPUT_ERROR = 4; //输入有误
    }

    //数据包头的类型码
    public class pk_type_ready {
        public static final int NORM_LOGIN = 0; //账号密码登录
        public static final int FACE_LOGIN = 1; //人脸登录
        public static final int REGISTER  = 2; //注册
    }

    public class pk_type_join {
        public static final int DATA = 0; //房间信息
        public static final int PYDATA = 1;//玩家信息
        public static final int TMDATA = 2;//队伍信息
        public static final int CREATE = 3; //创建房间
        public static final int JOIN = 4; //加入
        public static final int QUIT = 5; //退出房间
        public static final int ENREDY = 6;  //准备
        public static final int UNREDY = 7;//取消准备
        public static final int BEGIN = 8;//开始游戏
    }

    public class pk_type_all {
        public static final int HEARTBEAT = 66; //心跳包
        public static final int GAMESTATUS = 67;//游戏阶段
        public static final int TIMER = 68; //时间
    }

    public class packHeader {
        public int stat;
        public int type;//数据类型，是心跳包还是什么
        public int sub_type;
        public int data_length;//数据段的长度，这样你还可以一并解决粘包额问题

        public void recv(CppInputStream is) throws IOException {
            stat = is.readInt();
            type = is.readInt();
            sub_type = is.readInt();
            data_length = is.readInt();
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(stat);
            os.writeInt(type);
            os.writeInt(sub_type);
            os.writeInt(data_length);
        }
    }

    public static void sendPackHeader(int status, int type, int sub_type, int data_length,CppOutputStream ops) throws IOException{
        data.packHeader ph = new data().new packHeader();
        ph.stat = status;
        ph.type = type;
        ph.sub_type = sub_type;
        ph.data_length = data_length;
        ph.send(ops);
    }

}
