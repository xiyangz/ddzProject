package com.example.ddzclient;

import androidx.appcompat.app.AppCompatActivity;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.example.ddzclient.data.CppInputStream;
import com.example.ddzclient.data.CppOutputStream;
import com.example.ddzclient.data.data;
import com.example.ddzclient.data.data_common;
import com.example.ddzclient.data.stringToCharArray;

import java.net.Socket;

import static com.example.ddzclient.bin.thebin;

public class  LoginActivity extends AppCompatActivity {
    public static String account=null;
    public static String password=null;
    private CppInputStream ips = null;
    private CppOutputStream ops =null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        if(thebin.str_password!=null && thebin.str_password!=null){
            Login(thebin.str_password,thebin.str_password);
        }
    }
    @Override
    protected void onStart() {
        super.onStart();
        if((account!=null)&&(password!=null)) {
            ((EditText)findViewById(R.id.et_account)).setText(account);
            ((EditText)findViewById(R.id.et_password)).setText(password);
        }
    }
    //点击返回按钮
    public void onBack(View view) {
        Intent intent = new Intent(this, MainActivity.class);//显示intent
        startActivity(intent);
        finish();
    }
    //点击登录按钮
    public void onLogin(View view) {
        EditText et_account = findViewById(R.id.et_account);
        EditText et_password = findViewById(R.id.et_password);
        String str_act = et_account.getText().toString();
        String str_pwd = et_password.getText().toString();
        if(str_act.length() == 0) {
            Toast.makeText(this, "账号不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        else if(str_pwd.length()==0){
            Toast.makeText(this, "密码不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        Login(str_act,str_pwd);
    }
    private ProgressDialog waitingDialog = null;
    //onLogin中调用的函数
    public void Login(String str_act,String str_pwd){
        new Thread(()->{
            try
            {
                LoginActivity.this.runOnUiThread(()->
                {   waitingDialog= new ProgressDialog(LoginActivity.this);
                    waitingDialog.setMessage("等待中...");
                    waitingDialog.setIndeterminate(true);
                    waitingDialog.setCancelable(false);
                    waitingDialog.show();
                });
                thebin.socket = new Socket(thebin.ser_ip,thebin.ser_login_port);
                ips = new CppInputStream(thebin.socket.getInputStream());
                ops = new CppOutputStream(thebin.socket.getOutputStream());

                data_common.user_login ul =new data_common().new user_login();
                stringToCharArray.stringToCharAry(ul.account,str_act);
                stringToCharArray.stringToCharAry(ul.password,str_pwd);
                data.sendPackHeader(data.login_status.READY,data.pk_type_ready.NORM_LOGIN,data.return_type.NOTHING,0,ops);
                ul.send(ops);
                data.packHeader ph2 = new data().new packHeader();
                ph2.recv(ips);
                waitingDialog.dismiss();
                if(ph2.stat == data.login_status.READY) {
                    if(ph2.type == data.pk_type_ready.NORM_LOGIN){
                        if(ph2.sub_type == data.return_type.SUCCESS){
                            //登录成功
                            thebin.playerdata.user.id = ips.readInt();
                            thebin.playerdata.key = ips.readString(data.KEY_SIZE + 1);
                            //保存用户名和密码
                            thebin.str_account = str_act;
                            thebin.str_password = str_pwd;
                            //创建新activity
                            Intent intent = new Intent(this, TeamActivity.class);//显示intent
                            startActivity(intent);
                            //结束自己
                            finish();
                            return;

                        }
                        else if(ph2.sub_type == data.return_type.INPUT_ERROR){
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(LoginActivity.this,"账号或密码错误",Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                        else if(ph2.sub_type == data.return_type.FAILED){
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(LoginActivity.this,"登陆失败,账号可能已经在线",Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }

                }
                ips.close();
                ops.close();
                thebin.socket.close();
            }
            catch (Exception e)
            {
                waitingDialog.dismiss();
                runOnUiThread(()->
                {
                    Toast.makeText(LoginActivity.this,"网络异常",Toast.LENGTH_SHORT).show();
                });
                Log.i("ERROR", "onLogin: "+e.getMessage());
            }
        },"thread1").start();
    }
}
