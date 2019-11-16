package com.example.ddzclient.data;

import java.io.IOException;

public class data_common {

    public class user_reg {
        public char[] account = new char[data.ACCOUNT_MAXSIZE + 1];
        public char[] password = new char[data.PASSWORD_MAXSIZE + 1];
        public char[] nickname = new char[data.NICKNAME_MAXSIZE + 1];

        public void recv(CppInputStream is) throws IOException {
            account = is.readString(data.ACCOUNT_MAXSIZE + 1);
            password = is.readString(data.PASSWORD_MAXSIZE + 1);
            nickname = is.readString(data.NICKNAME_MAXSIZE + 1);
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeString(account);
            os.writeString(password);
            os.writeString(nickname);
        }
    }


    public class user_login {
        public char[] account = new char[data.ACCOUNT_MAXSIZE + 1];
        public char[] password = new char[data.PASSWORD_MAXSIZE + 1];

        public void recv(CppInputStream is) throws IOException {
            account = is.readString(data.ACCOUNT_MAXSIZE + 1);
            password = is.readString(data.PASSWORD_MAXSIZE + 1);
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeString(account);
            os.writeString(password);
        }
    }



    public class trans_user_info//user_clt
    {
        public int id;
        public char[] account = new char[data.ACCOUNT_MAXSIZE + 1];
        public char[] nickname = new char[data.NICKNAME_MAXSIZE + 1];
        public int player_isPrepare;

        public void recv(CppInputStream is) throws IOException {
            id = is.readInt();
            account = is.readString(data.ACCOUNT_MAXSIZE + 1);
            nickname = is.readString(data.NICKNAME_MAXSIZE + 1);
            player_isPrepare = is.readInt();
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(id);
            os.writeString(account);
            os.writeString(nickname);
            os.writeInt(player_isPrepare);
        }
    }


    public class team {
        public int team_id;//使用创建者的id
        public char[] team_name;
        public int player_num;//房间当前人数
        public trans_user_info[] player;

        public team(){
            team_name = new char[data.TEAMNAME_MAXSIZE + 1];
            player = new trans_user_info[data.PLAYER_NUMBER];
            for(int i=0;i<data.PLAYER_NUMBER;i++) {
                player[i] = new trans_user_info();
            }
        }
        public void recv(CppInputStream is) throws IOException {
            team_id = is.readInt();
            team_name = is.readString(data.TEAMNAME_MAXSIZE + 1);
            player_num = is.readInt();
            for(int i=0;i<data.PLAYER_NUMBER;i++){
                player[i].recv(is);
            }
        }
        public void send(CppOutputStream os) throws IOException{
            os.writeInt(team_id);
            os.writeString(team_name);
            os.writeInt(player_num);
            for(int i=0;i<data.PLAYER_NUMBER;i++){
                player[i].send(os);
            }
        }
    }
}
