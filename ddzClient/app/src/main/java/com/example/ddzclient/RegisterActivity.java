package com.example.ddzclient;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;


import com.example.ddzclient.data.CppInputStream;
import com.example.ddzclient.data.CppOutputStream;
import com.example.ddzclient.data.data;
import com.example.ddzclient.data.data_common;
import com.example.ddzclient.data.stringToCharArray;
import com.example.ddzclient.utils.Base64Util;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.FileProvider;

import android.os.Environment;
import android.provider.MediaStore;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

import static com.example.ddzclient.bin.thebin;


public class RegisterActivity extends AppCompatActivity {


    private EditText et_nickname;
    private EditText et_account;
    private EditText et_password;
    private TextView mImageNameText;
    private Button resetBtn;
    private String imagePath;

    public static final int TAKE_PHOTO = 1;
    // 照片路径
    private Uri imageUri = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);

        et_nickname = findViewById(R.id.et_reg_nickname);
        et_account = findViewById(R.id.et_reg_act);
        et_password = findViewById(R.id.et_reg_pwd);
        mImageNameText = findViewById(R.id.imageName);
        resetBtn = findViewById(R.id.resetBtn);

        resetBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String nickName = et_nickname.getText().toString();
                String account = et_account.getText().toString();
                String password = et_password.getText().toString();

                if (nickName != null && nickName.length() != 0) {
                    et_nickname.setText("");
                }
                if (account != null && account.length() != 0) {
                    et_account.setText(null);
                }
                if (password != null && password.length() != 0) {
                    et_password.setText(null);
                }
                if (!"未上传头像".equals(mImageNameText.getText())) {
                    mImageNameText.setText("未上传头像");
                }
            }
        });
    }
    //选择图片
    public void selectImage(View view) {

        ActivityCompat.requestPermissions(RegisterActivity.this, new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);

        //创建File对象，用于存储拍照后的图片
        File outputImage = new File(Environment.getExternalStorageDirectory() + File.separator + Environment.DIRECTORY_DCIM + File.separator + "Camera" + File.separator + et_nickname.getText().toString() + "_" + System.currentTimeMillis() + ".jpg");
        if (outputImage.exists()) {
            outputImage.delete();
            try {
                outputImage.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (Build.VERSION.SDK_INT >= 24) {
            imageUri = FileProvider.getUriForFile(RegisterActivity.this, "com.example.ddzclient.fileprovider", outputImage);
        } else {
            imageUri = Uri.fromFile(outputImage);
        }

        //启动相机程序
        Intent intent = new Intent("android.media.action.IMAGE_CAPTURE");
        //intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        intent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri);
        startActivityForResult(intent, TAKE_PHOTO);
        this.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, imageUri));
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case 0:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Intent intent = new Intent("android.media.action.IMAGE_CAPTURE");
                    intent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri);
                    startActivityForResult(intent, TAKE_PHOTO);
                } else {
                    Toast.makeText(this, "还没有获取打开相机的权限！", Toast.LENGTH_LONG).show();
                }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent intent) {
        super.onActivityResult(requestCode, resultCode, intent);
        switch (requestCode) {
            case TAKE_PHOTO: {
                if (resultCode == RESULT_OK) {
                    Cursor cursor = null;
                    cursor = getContentResolver().query(imageUri, null, null, null, null);
                    if (cursor.moveToFirst()) {
                        int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DISPLAY_NAME);
                        imagePath = cursor.getString(columnIndex);
                        if (imagePath != null) {
                            mImageNameText.setText("已获得头像");
                            imagePath = null;
                        } else {
                            mImageNameText.setText("未获得头像");
                        }
                    }
                    //将拍摄的照片进行人脸检测和上传

                }
                break;
            }

        }
    }

    //图片压缩和上传
    public void compressBitmapAndUpload(Uri imgUri,CppOutputStream ops) throws Exception{
        // 设置参数
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true; // 只获取图片的大小信息，而不是将整张图片载入在内存中，避免内存溢出
        InputStream inputStream = getContentResolver().openInputStream(imgUri);
        int height = options.outHeight;
        int width= options.outWidth;
        int inSampleSize = 4; // 默认像素压缩比例，压缩为原图的1/2
        int minLen = Math.min(height, width); // 原图的最小边长
        if(minLen > 100) { // 如果原始图像的最小边长大于100dp（此处单位我认为是dp，而非px）
            float ratio = (float)minLen / 100.0f; // 计算像素压缩比例
            inSampleSize = (int)ratio;
        }
        options.inJustDecodeBounds = false; // 计算好压缩比例后，这次可以去加载原图了
        options.inSampleSize = inSampleSize; // 设置为刚才计算的压缩比例
        final  Bitmap bm = BitmapFactory.decodeStream(inputStream,null,options); // 解码文件
        //bitmap使用base64转码
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bm.compress(Bitmap.CompressFormat.JPEG, 10, baos);
        byte[] bytes = baos.toByteArray();
        //base64 encode
        String imgStr = Base64Util.encode(bytes);
        ops.writeInt(imgStr.length());
        ops.writeString(imgStr.toCharArray());
    }

    private ProgressDialog waitingDialog = null;
    public void OnCkRegister(View view) {
        String str_nkn = et_nickname.getText().toString();
        String str_act = et_account.getText().toString();
        String str_pwd = et_password.getText().toString();
        if (str_nkn != null && str_nkn.length() != 0) {
            System.out.println("昵称：" + str_nkn);
        } else {
            Toast.makeText(this, "昵称不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        if (str_act != null && str_act.length() != 0) {
            System.out.println("账号：" + str_act);
        } else {
            Toast.makeText(this, "账号不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        if (str_pwd != null && str_pwd.length() != 0) {
            System.out.println("密码：" + str_pwd);
        } else {
            Toast.makeText(this, "密码不能为空", Toast.LENGTH_SHORT).show();
            return;
        }

        if(str_nkn.length() == 0){
            Toast.makeText(this, "昵称不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        else if(str_act.length() == 0){
            Toast.makeText(this, "账号不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        else if(str_pwd.length() == 0){
            Toast.makeText(this, "密码不能为空", Toast.LENGTH_SHORT).show();
            return;
        }
        else if(str_nkn.length()>data.NICKNAME_MAXSIZE){
            Toast.makeText(this, "昵称太长", Toast.LENGTH_SHORT).show();
            return;
        }
        else if(str_act.length()>data.ACCOUNT_MAXSIZE){
            Toast.makeText(this, "账号太长", Toast.LENGTH_SHORT).show();
            return;
        }
        else if(str_pwd.length()>data.PASSWORD_MAXSIZE){
            Toast.makeText(this, "密码太长", Toast.LENGTH_SHORT).show();
            return;
        }

        if(imageUri == null) {
            Toast.makeText(this, "未获得头像", Toast.LENGTH_SHORT).show();
            return;
        }
        new Thread(()->{
            try{
                RegisterActivity.this.runOnUiThread(()->
                {
                    waitingDialog= new ProgressDialog(RegisterActivity.this);
                    waitingDialog.setMessage("等待中...");
                    waitingDialog.setIndeterminate(true);
                    waitingDialog.setCancelable(false);
                    waitingDialog.show();
                });
                Socket socket = new Socket(thebin.ser_ip,thebin.ser_login_port);
                CppInputStream cs = new CppInputStream(socket.getInputStream());
                CppOutputStream ops = new CppOutputStream(socket.getOutputStream());
                data_common.user_reg ur = new data_common().new user_reg();
                stringToCharArray.stringToCharAry(ur.account,str_act);
                stringToCharArray.stringToCharAry(ur.password,str_pwd);
                stringToCharArray.stringToCharAry(ur.nickname,str_nkn);
                data.packHeader ph = new data().new packHeader();
                ph.stat = data.login_status.READY;
                ph.type = data.pk_type_ready.REGISTER;
                ph.sub_type = data.return_type.NOTHING;
                ph.data_length = 0;
                ph.send(ops);
                ur.send(ops);
                compressBitmapAndUpload(imageUri,ops);
                data.packHeader ph2 = new data().new packHeader();
                ph2.recv(cs);
                waitingDialog.dismiss();
                switch (ph2.stat)
                {
                    case data.login_status.READY:
                    {
                        switch (ph2.type){
                            case data.pk_type_ready.REGISTER:{
                                if(ph2.sub_type == data.return_type.ACCOUNT_EXISTED)
                                {
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            Toast.makeText(RegisterActivity.this,"账号已经存在,或头像已经存在",Toast.LENGTH_SHORT).show();
                                        }
                                    });
                                }
                                else if(ph2.sub_type == data.return_type.INPUT_ERROR){
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            Toast.makeText(RegisterActivity.this,"人脸头像不清晰",Toast.LENGTH_SHORT).show();
                                        }
                                    });
                                }
                                else if(ph2.sub_type == data.return_type.FAILED){
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            Toast.makeText(RegisterActivity.this,"服务器出错",Toast.LENGTH_SHORT).show();
                                        }
                                    });
                                }
                                else if(ph2.sub_type == data.return_type.SUCCESS){
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            Toast.makeText(RegisterActivity.this,"注册成功",Toast.LENGTH_SHORT).show();
                                        }
                                    });
                                    returnLogin();
                                }

                            }
                            break;
                        }
                    }
                        break;
                }
                cs.close();
                ops.close();
                socket.close();
            }
            catch(Exception e)
            {
                waitingDialog.dismiss();
                runOnUiThread(()->
                {
                    Toast.makeText(RegisterActivity.this,"网络异常",Toast.LENGTH_SHORT).show();
                });
            }


        },"thread3").start();

    }

    public void returnLogin() {
        Intent intent = new Intent();
        intent.setClass(RegisterActivity.this, MainActivity.class);
        startActivity(intent);
    }


}
