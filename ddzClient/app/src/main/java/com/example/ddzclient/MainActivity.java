package com.example.ddzclient;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.FileProvider;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.example.ddzclient.data.CppInputStream;
import com.example.ddzclient.data.CppOutputStream;
import com.example.ddzclient.utils.Base64Util;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

import static com.example.ddzclient.bin.thebin;

import com.example.ddzclient.data.data;

public class MainActivity extends AppCompatActivity {

    public static final int TAKE_PHOTO = 1;
    // 照片路径
    private Uri imageUri;

    private CppInputStream ips = null;
    private CppOutputStream ops =null;
    public String testResult;
    public int testFlag;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);

        //创建File对象，用于存储拍照后的图片
        File outputImage = new File(Environment.getExternalStorageDirectory() + File.separator + Environment.DIRECTORY_DCIM + File.separator + "Camera" + File.separator +  "_" + System.currentTimeMillis() + ".jpg");
        if (outputImage.exists()) {
            outputImage.delete();
            try {
                outputImage.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (Build.VERSION.SDK_INT >= 24) {
            imageUri = FileProvider.getUriForFile(MainActivity.this, "com.example.ddzclient.fileprovider", outputImage);
        } else {
            imageUri = Uri.fromFile(outputImage);
        }
    }
    //点击注册
    public void registerAndUpload(View view) {
        Intent intent = new Intent(this, RegisterActivity.class);//显示intent
        startActivity(intent);
    }
    //点击账号密码登录按钮
    public void usualLogin(View view) {
        Intent intent = new Intent(this, LoginActivity.class);//显示intent
        startActivity(intent);
        finish();
    }
    //选择头像登录按钮点击事件
    public void selectAndCheck(View view) {
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE},1);
        //创建File对象，用于存储拍照后的图片
        File outputImage = new File(Environment.getExternalStorageDirectory(), "/DCIM/Camera/"+System.currentTimeMillis() + ".jpg");
        if(outputImage.exists())
        {
            outputImage.delete();
            try
            {
                outputImage.createNewFile();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }

        if (Build.VERSION.SDK_INT>=24)
        {
            imageUri = FileProvider.getUriForFile(MainActivity.this,"com.example.ddzclient.fileprovider",outputImage);
        }
        else
        {
            imageUri = Uri.fromFile(outputImage);
        }
        //启动相机程序
        Intent intent = new Intent("android.media.action.IMAGE_CAPTURE");
        intent.putExtra(MediaStore.EXTRA_OUTPUT,imageUri);
        startActivityForResult(intent,TAKE_PHOTO);
    }
    //图片压缩和上传
    @SuppressLint("ResourceType")
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
    //拍照成功后的回调
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        super.onActivityResult(requestCode, resultCode, intent);
        switch (requestCode) {
            case TAKE_PHOTO: {
                if (resultCode == RESULT_OK) {
                    Cursor cursor = null;
                    cursor = getContentResolver().query(imageUri, null, null, null, null);
                    if (cursor.moveToFirst()) {
                        int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DISPLAY_NAME);
                        String uploadFileName = cursor.getString(columnIndex);
                    }
                    //将拍摄的照片进行人脸检测和上传
                    new Thread(()->{
                            try {
                                MainActivity.this.runOnUiThread(()->
                                {
                                    waitingDialog= new ProgressDialog(MainActivity.this);
                                    waitingDialog.setMessage("等待中...");
                                    waitingDialog.setIndeterminate(true);
                                    waitingDialog.setCancelable(false);
                                    waitingDialog.show();
                                });
                                thebin.socket = new Socket(thebin.ser_ip,thebin.ser_login_port);
                                ips = new CppInputStream(thebin.socket.getInputStream());
                                ops = new CppOutputStream(thebin.socket.getOutputStream());
                                data.sendPackHeader(data.login_status.READY, data.pk_type_ready.FACE_LOGIN, data.return_type.NOTHING,0,ops);
                                compressBitmapAndUpload(imageUri,ops);
                                data.packHeader ph = new data().new packHeader();
                                ph.recv(ips);
                                waitingDialog.dismiss();
                                if(ph.stat == data.login_status.READY) {
                                    if (ph.type == data.pk_type_ready.FACE_LOGIN) {
                                        if (ph.sub_type == data.return_type.SUCCESS) {
                                            //登录成功
                                            thebin.playerdata.user.id = ips.readInt();
                                            thebin.playerdata.key = ips.readString(data.KEY_SIZE + 1);
                                            //创建新activity
                                            Intent tintent = new Intent(this, TeamActivity.class);//显示intent
                                            startActivity(tintent);
                                            //结束自己
                                            finish();
                                            return;

                                        } else if (ph.sub_type == data.return_type.FAILED) {
                                            runOnUiThread(()->
                                            {
                                                Toast.makeText(MainActivity.this,"登录失败，可能已经在线，账号未注册，人脸",Toast.LENGTH_SHORT).show();
                                            });

                                        } else if (ph.sub_type == data.return_type.INPUT_ERROR) {
                                            runOnUiThread(()->
                                            {
                                                Toast.makeText(MainActivity.this,"登录失败，未检测到人脸，或人脸不正确，或照片超过2M",Toast.LENGTH_SHORT).show();
                                            });
                                        }
                                    }
                                }
                                ips.close();
                                ops.close();
                                thebin.socket.close();
                            }catch (Exception e){
                                waitingDialog.dismiss();
                                Log.i("error", e.getMessage());
                            }
                        }).start();

                }
            }
        }
    }
    /*
     * 人脸登录测试方法
     * */
    public void testFaceLogin(InputStream inputStream) {

        if (inputStream == null) {
            testFlag = 5;
        } else {
            new Thread(()->{
                try {

                    thebin.socket = new Socket(thebin.ser_ip,thebin.ser_login_port);
                    ips = new CppInputStream(thebin.socket.getInputStream());
                    ops = new CppOutputStream(thebin.socket.getOutputStream());
                    data.sendPackHeader(data.login_status.READY, data.pk_type_ready.FACE_LOGIN, data.return_type.NOTHING,0,ops);
                    compressBitmapAndUpload1(inputStream,ops);
                    data.packHeader ph = new data().new packHeader();
                    ph.recv(ips);
                    if(ph.stat == data.login_status.READY) {
                        if (ph.type == data.pk_type_ready.FACE_LOGIN) {
                            if (ph.sub_type == data.return_type.SUCCESS) {
                                //登录成功
                                testFlag = 1;

                            } else if (ph.sub_type == data.return_type.FAILED) {
                                testFlag = 2;
                            } else if (ph.sub_type == data.return_type.INPUT_ERROR) {
                                testFlag = 3;
                            }
                        }
                    }
                    ips.close();
                    ops.close();
                    thebin.socket.close();
                }catch (Exception e){
                    testFlag = 4;
                    e.printStackTrace();
                }
            }).start();
        }

        /*
         * 根据不同情况来给出测试结果
         */
        if (testFlag == 0) {
            testResult = "测试出错！";
        } else if (testFlag == 1) {
            testResult = "人脸验证通过，登陆成功！";
        } else if (testFlag == 2) {
            testResult = "您已登录或该人脸还未绑定任何账号！";
        } else if (testFlag == 3) {
            testResult = "未检测到人脸或人脸检测失败！";
        } else if (testFlag == 4) {
            testResult = "网络出现异常！";
        } else if (testFlag == 5) {
            testResult = "还未上传照片！";
        }

    }

    @SuppressLint("ResourceType")
    public void compressBitmapAndUpload1(InputStream inputStream,CppOutputStream ops) throws Exception{
        // 设置参数
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true; // 只获取图片的大小信息，而不是将整张图片载入在内存中，避免内存溢出
        //InputStream inputStream = getContentResolver().openInputStream(imgUri);
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

}
