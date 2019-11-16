package com.example.ddzclient;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;

import com.example.ddzclient.data.CppInputStream;
import com.example.ddzclient.data.CppOutputStream;
import com.example.ddzclient.data.data;
import com.example.ddzclient.data.data_common;
import com.example.ddzclient.data.stringToCharArray;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static com.example.ddzclient.bin.thebin;
import static com.example.ddzclient.data.data.TEAMNAME_MAXSIZE;
import static com.example.ddzclient.data.data.sendPackHeader;


public class TeamActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {
    private ListView listView = null;
    private List<String> listdata = new ArrayList<>();
    private int[] teams_id;
    private boolean isInTeamList = true;
    private boolean isRunning = true;

    public CppInputStream ips = null;
    public CppOutputStream ops = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_team);
        listView = findViewById(R.id.lv);
        listView.setOnItemClickListener(this);

        try {
            ips = new CppInputStream(thebin.socket.getInputStream());
            ops = new CppOutputStream(thebin.socket.getOutputStream());
        } catch (IOException e) {
            Log.e("sock错误", "TeamActivity.onCreate中发生socket错误");
            return;
        }

        if (ips == null || ops == null) {
            Log.e("sock错误", "TeamActivity.onCreate中发生socket错误");
            return;
        }

        new Thread(() -> {
            try {
                while(isRunning) {
                    while (isInTeamList) {
                        data.packHeader ph = new data().new packHeader();
                        ph.recv(ips);
                        switch (ph.stat) {
                            case data.login_status.JOIN: {
                                switch (ph.type) {
                                    case data.pk_type_join.DATA:
                                        OnData(ph.data_length);
                                        break;
                                    case data.pk_type_join.PYDATA:
                                        thebin.playerdata.user.recv(ips);
                                        break;
                                    case data.pk_type_join.CREATE:
                                        OnCreate(ph.sub_type);
                                        break;
                                    case data.pk_type_join.JOIN:
                                        OnJoin(ph.sub_type);
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            }
                            default:
                                //defRecv
                                break;
                        }
                    }
                    while (!isInTeamList) {
                        data.packHeader ph = new data().new packHeader();
                        ph.recv(ips);
                        switch (ph.stat) {
                            case data.login_status.JOIN: {
                                switch (ph.type) {
                                    case data.pk_type_join.JOIN:
                                        OnJoin(ph.sub_type);
                                        break;
                                    case data.pk_type_join.QUIT:
                                        OnQuit(ph.sub_type, ph.data_length);
                                        break;
                                    case data.pk_type_join.ENREDY:
                                        OnEnReady(ph.data_length);
                                        break;
                                    case data.pk_type_join.UNREDY:
                                        OnUnReady(ph.data_length);
                                        break;
                                    case data.pk_type_join.BEGIN:
                                        OnBegin();
                                        break;
                                    case data.pk_type_join.TMDATA:
                                        OnTmData();
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            }
                            default:
                                //defRecv
                                break;
                        }
                    }
                }
            } catch (IOException e) {
            }
        }, "thread5").start();

        new Thread(() -> {
            try {
                data.sendPackHeader(data.login_status.JOIN, data.pk_type_join.PYDATA, data.return_type.NOTHING, 0, ops);
                updateListData();
            } catch (IOException e) {
                Log.e("sock错误", "TeamActivity.onCreate中发生socket错误");
                return;
            }
        }, "thread4").start();

    }

    @Override
    protected void onResume() {
        super.onResume();
        if((!isInTeamList)&&(isRunning)){
            new Thread(()->{
                try {
                    if(ops!=null) {
                        data.sendPackHeader(data.login_status.JOIN, data.pk_type_join.TMDATA, data.return_type.NOTHING, 0, ops);
                    }
                }catch (IOException e){

                }
            }).start();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK){
                try {
                    if(ips!=null)
                    ips.close();
                    if(ops!=null)
                        ops.close();
                    thebin.socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        isRunning =false;
        try {
            if(ops!=null)
                ops.close();
            if(ips!=null)
                ips.close();
            if(thebin.socket!=null)
                thebin.socket.close();
        }catch (Exception e){
        }
    }

    private void OnJoin(int rt) throws IOException {

        if (rt == data.return_type.SUCCESS) {
            isInTeamList =false;//socket停止接收数据
            thebin.myTeam.recv(ips);
            //进入新teamActivity
            runOnUiThread(()->{
                setContentView(R.layout.activity_iteam);
                updateList();
            });
        } else if (rt == data.return_type.FAILED) {

        }
    }

    private void OnCreate(int sub_type) throws IOException {
        if (sub_type == data.return_type.SUCCESS) {
            isInTeamList =false;//socket停止接收数据
            thebin.myTeam.recv(ips);
            //进入新teamActivity
            runOnUiThread(()->{
                setContentView(R.layout.activity_iteam);
                updateList();
            });

//            runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    Intent intent = new Intent(TeamActivity.this, IteamActivity.class);//显示intent
//                    startActivity(intent);
//                    finish();
//                }
//            });
        } else {//创建失败
            Toast.makeText(TeamActivity.this, "创建房间失败", Toast.LENGTH_SHORT).show();
        }
    }

    private void OnData(int data_length) throws IOException {
        if (data_length != 0) {
            listdata.clear();
            teams_id = new int[data_length];
            data_common.team oteam = new data_common().new team();
            for (int i = 0; i < data_length; i++) {
                oteam.recv(ips);
                teams_id[i] = oteam.team_id;
                insertListData(oteam);
            }
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    showListData();
                }
            });
        }
    }

    private void updateListData() throws IOException {
        data.sendPackHeader(data.login_status.JOIN, data.pk_type_join.DATA, data.return_type.NOTHING, 0, ops);
    }

    private void insertListData(data_common.team oteam) {
        String teamStr = "序号:" + Integer.toString(listdata.size());
        teamStr += "    房间名:";
        int i = 0;
        while (oteam.team_name[i] != '\0') {
            teamStr += oteam.team_name[i];
            i++;
        }
        teamStr += "    人数:";
        teamStr += Integer.toString(oteam.player_num);
        listdata.add(teamStr);
    }

    private void showListData() {
        listView.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_expandable_list_item_1, listdata));
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        new Thread(() -> {
            try {
                sendPackHeader(data.login_status.JOIN, data.pk_type_join.JOIN, data.return_type.NOTHING, teams_id[position], ops);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }, "thread8").start();
        ;
    }

    public void onClickUpdateList(View view) {
        new Thread(() -> {
            try {
                updateListData();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
        ;
    }

    public void onClickCreateTeam(View view) {
        final EditText editText = new EditText(this);
        editText.setText(stringToCharArray.charArrayToString(thebin.playerdata.user.nickname) + "的房间");
        AlertDialog.Builder editDialog = new AlertDialog.Builder(this);
        editDialog.setIcon(R.mipmap.ic_launcher_round);
        editDialog.setTitle("请输入房间名称").setView(editText);
        editDialog.setNegativeButton("取消", null);
        editDialog.setPositiveButton("确定", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                if (editText.getText().toString().length() == 0) {
                    Toast.makeText(TeamActivity.this, "名称不能为空！", Toast.LENGTH_SHORT).show();
                    return;
                }
                new Thread(() -> {
                    char[] team_name = new char[TEAMNAME_MAXSIZE + 1];
                    stringToCharArray.stringToCharAry(team_name, editText.getText().toString());
                    try {
                        sendPackHeader(data.login_status.JOIN, data.pk_type_join.CREATE, data.return_type.NOTHING, 0, ops);
                        int i = 0;
                        while (i < team_name.length) {
                            ops.writeChar(team_name[i]);
                            i++;
                        }

                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                }, "thread7").start();
                Toast.makeText(TeamActivity.this, "将要创建房间，名为：" + editText.getText().toString(), Toast.LENGTH_SHORT).show();
            }
        });
        editDialog.show();
    }

    private boolean isPrepare = false;

    private void updateList(){
        listdata.clear();
        int i=0;
        String string = new String();
        while(i<thebin.myTeam.player_num){
            string="";
            string+="id:";
            string+=Integer.toString(thebin.myTeam.player[i].id);
            string+="    昵称:";
            string+= stringToCharArray.charArrayToString(thebin.myTeam.player[i].nickname);
            string+="    ";
            string+= thebin.myTeam.player[i].player_isPrepare==0?"等待准备":"已经准备";
            listdata.add(string);
            i++;
        }
        ((ListView)findViewById(R.id.lv2)).setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_expandable_list_item_1, listdata));
    }


    private void OnBegin() {
        //设置所以人为准备
        isPrepare = false;
        for(int i=0;i>data.PLAYER_NUMBER;i++){
            thebin.myTeam.player[i].player_isPrepare = 0;
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((Button)findViewById(R.id.btn_ready)).setText("准备");
                updateList();
            }
        });         //进入游戏主界面activity
        Intent intent = new Intent(this, RunActivity.class);//显示intent
        startActivity(intent);
        //finish();
    }

    private void OnTmData() throws IOException{
        thebin.myTeam.recv(ips);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                updateList();
            }
        });
    }

    private void OnUnReady(int id) {
        int i = 0;
        while (i < thebin.myTeam.player_num) {
            if (thebin.myTeam.player[i].id == id) {
                thebin.myTeam.player[i].player_isPrepare = 0;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateList();
                    }
                });
                if(id == thebin.playerdata.user.id) {
                    isPrepare = false;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ((Button)findViewById(R.id.btn_ready)).setText("准备");
                        }
                    });
                }
                break;
            }
            i++;
        }
    }

    private void OnEnReady(int id) {
        int i = 0;
        while (i < thebin.myTeam.player_num) {
            if (thebin.myTeam.player[i].id == id) {
                thebin.myTeam.player[i].player_isPrepare = 1;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateList();
                    }
                });
                if(id == thebin.playerdata.user.id) {
                    isPrepare = true;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ((Button)findViewById(R.id.btn_ready)).setText("取消准备");
                        }
                    });
                }
                break;
            }
            i++;
        }
    }

    private void OnQuit(int rt, int id) throws IOException{
        if (id != thebin.playerdata.user.id)
        {//不是自身退出
            Log.i("111", "OnQuit: 不是自己退出房间");
            thebin.myTeam.recv(ips);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    updateList();
                }
            });
        }
        else
        {//自己退出  回到房间列表页面
            Log.i("111", "OnQuit: 自己退出房间");
            isInTeamList = true;
            runOnUiThread(()->{
                setContentView(R.layout.activity_team);
                listView = findViewById(R.id.lv);
                listView.setOnItemClickListener(this);
                new Thread(()->{
                    try{
                        updateListData();
                    }
                    catch (Exception e){

                    }
                }).start();
            });

        }
    }

    public void onClickReady(View view) {
        new Thread(()->{
            try {
                if (isPrepare) {//准备了
                    data.sendPackHeader(data.login_status.JOIN, data.pk_type_join.UNREDY, data.return_type.NOTHING, 0, ops);
                } else {//没准备
                    data.sendPackHeader(data.login_status.JOIN, data.pk_type_join.ENREDY, data.return_type.NOTHING, 0, ops);
                }
            }catch (IOException e){

            }
        }).start();
    }

    public void onClickQuitTeam(View view) {
        new Thread(()->{
            try {
                data.sendPackHeader(data.login_status.JOIN, data.pk_type_join.QUIT, data.return_type.NOTHING, 0,ops);
            }catch (IOException e){
            }
        }).start();
    }
}
