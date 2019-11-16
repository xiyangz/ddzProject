package com.example.ddzclient;
import com.example.ddzclient.data.data;
import com.example.ddzclient.data.data_common;
import com.example.ddzclient.data.data_run;
import java.net.Socket;

public  class bin {
    public static bin thebin = new bin();
    //存储各种数据et
    //112.124.45.232  192.168.1.106
    public String ser_ip = "112.124.45.232";
    public int ser_login_port = 8881;
    public int ser_run_port = 8882;
    public Socket socket = null;
//    public CppOutputStream ops = null;
//    public CppInputStream ips =null;
    public String str_account =null;
    public String str_password =null;
    public data_run.playerData playerdata= new data_run().new playerData();
    public char[] team_name = new char[data.TEAMNAME_MAXSIZE + 1];
    public data_common.team myTeam = new data_common().new team();

}
