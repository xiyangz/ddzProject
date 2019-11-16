package com.example.ddzclient;

import androidx.appcompat.app.AppCompatActivity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.widget.Toast;
import com.example.ddzclient.data.CppInputStream;
import com.example.ddzclient.data.CppOutputStream;
import com.example.ddzclient.data.data;
import com.example.ddzclient.data.data_run;

import java.io.IOException;
import java.net.Socket;

import static com.example.ddzclient.bin.thebin;

public class RunActivity extends AppCompatActivity {
    private boolean isRun = true;
    private RunView runView = null;
    public  int ScreenX = 0;
    public  int ScreenY = 0;

    Socket socket = null;
    CppInputStream ips = null;
    CppOutputStream ops = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        DisplayMetrics dm = getResources().getDisplayMetrics();
        ScreenX = dm.widthPixels;
        ScreenY = dm.heightPixels;
        //new才会触发surfaceView::create
        runView = new RunView(this);
        //更改横屏
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //防止屏幕熄灭
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(runView);
        //开辟线程用来与服务器通信
        new Thread(() -> {
            try {
                socket = new Socket(thebin.ser_ip, thebin.ser_run_port);
                ips = new CppInputStream(socket.getInputStream());
                ops = new CppOutputStream(socket.getOutputStream());

                //发送checkStruct;
                data_run.checkStruct cs = new data_run().new checkStruct();
                cs.id = thebin.playerdata.user.id;
                cs.key = thebin.playerdata.key;
                cs.team_id = thebin.myTeam.team_id;
                cs.send(ops);

                data_run.runHeader trh = new data_run().new runHeader();
                trh.recv(ips);

                if ((trh.run_satus == data_run.run_status.BEGIN) && (trh.type == data_run.pk_type_begin.CHECK_SUC)) {

                } else {
                      runOnUiThread(new Runnable() {
                          @Override
                          public void run() {
                              Toast.makeText(RunActivity.this, "进入游戏失败,即将退出", Toast.LENGTH_SHORT).show();
                          }
                      });
                      finish();
                      return;
                }
                data_run.runHeader rh = new data_run().new runHeader();
                while (isRun) {
                    rh.recv(ips);
                    if (rh.type == data_run.pk_type_norm.GAMESTATUS) {
                        runView.changeStatus(rh.run_satus);
                        continue;
                    }
                    switch (rh.run_satus) {
                        case data_run.run_status.BEGIN:
                            OnBegin(rh.type, rh.turn_sq);
                            break;
                        case data_run.run_status.DEAL:
                            OnDeal(rh.type);
                            break;
                        case data_run.run_status.LOOT:
                            OnLoot(rh.turn_sq, rh.type);
                            break;
                        case data_run.run_status.POKE:
                            OnPoke(rh.turn_sq, rh.type);
                            break;
                        case data_run.run_status.END:
                            OnEnd(rh.turn_sq);
                            break;
                    }
                }

            } catch (IOException e) {
                if(runView.game_status != data_run.run_status.END)
                {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(RunActivity.this,"游戏异常退出,极有可能是其他玩家退出",Toast.LENGTH_SHORT).show();
                        }
                    });
                    runView.game_status = data_run.run_status.END;
                    runView.drawRunMain();
                }
            } catch (Exception e) {

            }
        }).start();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        runView.handleTouch(event);
        return true;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK){
            //moveTaskToBack(true);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }



    public void ClickLoot(boolean lootOrNot) {
        new Thread(() -> {
            try {
                if (lootOrNot)
                    data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_loot.ENLOOT, ops);
                else
                    data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_loot.UNLOOT, ops);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();

    }

    public void ClickUnPoke() {
        new Thread(() -> {
            try {
                data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_poke.UNPLAY, ops);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    public void ClickExit(){
//        //创建新activity
//        Intent intent = new Intent(this, MainActivity.class);//显示intent
//        startActivity(intent);
        //结束自己
        finish();
    }

    //指明要出的牌的位置 数组下标
    public void ClickPoke(int[] pokepokers, int pokepokers_num) {
        new Thread(() -> {
            try {
                data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_poke.ENPLAY, ops);
                //告知出牌数量
                ops.writeInt(pokepokers_num);
                for (int i = 0; i < pokepokers_num; i++) {
                    ops.writeInt(pokepokers[i]);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    public void OnBegin(int type, int seq) throws IOException {
        if (type == data_run.pk_type_begin.SEQUENCE) {
            runView.sequence = seq;
            runView.last_seq = (seq - 1) < 0 ? 2 : (seq - 1);
            runView.next_seq = (seq + 1) > 2 ? 0 : (seq + 1);
            data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_begin.RECV_SEQ_SUC, ops);
            //有头图像的话 现在可以布置头像

            //开始发送心跳包  因为这时候确定其他队员都已经进入游戏
            new Thread(() -> {
                try {
                    while (isRun) {
                        data_run.runHeader hbrh = new data_run().new runHeader();
                        hbrh.run_satus = data_run.run_status.NORM;
                        hbrh.type = data_run.pk_type_norm.HEARTBEAT;
                        hbrh.turn_sq = runView.sequence;
                        hbrh.send(ops);
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                } catch (IOException e) {

                }
            }).start();
        }
    }

    public void OnDeal(int type) throws IOException, InterruptedException {
        if (type == data_run.pk_type_deal.POKER) {
            //接受卡牌
            for (int i = 0; i < 17; i++) {
                runView.handpoker[i].recv(ips);
            }
            //设置手牌数量
            for (int i = 0; i < data.PLAYER_NUMBER; i++) {
                runView.handpoker_num[i] = 17;
            }
            //并且更改绘制手牌的handpoker_pos_x;
            runView.changeHandpokerPosX();
            //展示卡牌
            runView.isShowPoker = true;
            runView.drawRunMain();
            //告知服务器接受完毕
            data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_deal.ENPOKE, ops);
        }
    }

    public void OnLoot(int turn, int type) throws IOException {
        if (type == data_run.pk_type_loot.ENLOOT) {
            runView.recvLoot(turn, true);
        } else if (type == data_run.pk_type_loot.UNLOOT) {
            runView.recvLoot(turn, false);
        } else if (type == data_run.pk_type_loot.TIME) {
            runView.remain_sec = ips.readInt();
            runView.recvTime();
        } else if (type == data_run.pk_type_loot.TURN) {
            runView.changeTurnOnLoot(turn);
        } else if (type == data_run.pk_type_loot.LOOT_RLT) {
            data_run.looter_data ld = new data_run().new looter_data();
            ld.recv(ips);
            runView.setLootResult(ld);
            //告知服务器接受地主牌成功
            data_run.sendRunHeader(runView.game_status, runView.sequence, data_run.pk_type_loot.RECV_PKR_SUC, ops);
        }
    }

    public void OnPoke(int turn, int type) throws Exception {
        if (type == data_run.pk_type_poke.TURN) {
            runView.changeTurnOnPoke(turn);
        }
        else if (type == data_run.pk_type_poke.TIME) {
            runView.remain_sec = ips.readInt();
            runView.recvTime();
        }
        else if (type == data_run.pk_type_poke.UNPOKE) {
            //自己回合出牌失败  也就是出牌不符合规则
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(RunActivity.this, "出牌不符合规则", Toast.LENGTH_SHORT).show();
                }
            });
        }
        else if (type == data_run.pk_type_poke.ENPOKE) {
            //接受返回的卡牌   出的牌时0就说明不要
            runView.changePokedpokerData();
        }
    }

    public void OnEnd(int type) throws IOException{
        if(type == data_run.pk_type_end.NORM){

        }
        else if(type == data_run.pk_type_end.PALYER_DISCONNECT) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(RunActivity.this, "其他玩家掉线", Toast.LENGTH_SHORT).show();
                }
            });
            ips.close();
            ops.close();
            socket.close();

        }
    }


}
