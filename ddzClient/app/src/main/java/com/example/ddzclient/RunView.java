package com.example.ddzclient;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

import com.example.ddzclient.data.data;
import com.example.ddzclient.data.data_run;

import java.io.IOException;
import java.io.InputStream;

public class RunView extends SurfaceView implements SurfaceHolder.Callback {

    public class bmpPosInfo {
        public int left_x = 0;
        public int top_y = 0;
        public int width = 0;
        public int height = 0;

        public int getRightX() {
            return left_x + width;
        }

        public int getBottomY() {
            return top_y + height;
        }

        public void drawBitmap(Bitmap bitmap, Canvas canvas) {
            canvas.drawBitmap(bitmap, null, new Rect(left_x, top_y, getRightX(), getBottomY()), null);
        }
    }

    public boolean isShowPoker = false;
    private RunActivity tra = null;
    //手牌
    private int handpoker_left_x = 0;
    private bmpPosInfo handpokerPos = new bmpPosInfo();
    private int handpoker_interval = 0;
    private int handpoker_select_height = 0;
    //出的牌
    private bmpPosInfo pokedpokerPos_left = new bmpPosInfo();
    private bmpPosInfo pokedpokerPos_mid = new bmpPosInfo();
    private bmpPosInfo pokedpokerPos_right = new bmpPosInfo();
    private int pokedpoker_interval = 0;
    //地主牌
    private bmpPosInfo lopokerPos = new bmpPosInfo();
    //按钮
    //通用按钮数据
    private bmpPosInfo btnfontPos = new bmpPosInfo();
    //具体按钮数据
    private bmpPosInfo enlootbtnbkPos = new bmpPosInfo();
    private bmpPosInfo unlootbtnbkPos = new bmpPosInfo();
    private bmpPosInfo enpokebtnbkPos = new bmpPosInfo();
    private bmpPosInfo unpokebtnbkPos = new bmpPosInfo();
    private bmpPosInfo hintbtnbkPos = new bmpPosInfo();
    //'抢地主' '不抢'  '不出'的字图片大小
    private bmpPosInfo resultftPos_left = new bmpPosInfo();
    private bmpPosInfo resultftPos_mid = new bmpPosInfo();
    private bmpPosInfo resultftPos_right = new bmpPosInfo();
    //时间
    private bmpPosInfo clockPos_left = new bmpPosInfo();
    private bmpPosInfo clockPos_loot_mid = new bmpPosInfo();
    private bmpPosInfo clockPos_poke_mid = new bmpPosInfo();
    private bmpPosInfo clockPos_right = new bmpPosInfo();

    private bmpPosInfo timePos_left = new bmpPosInfo();
    private bmpPosInfo timePos_mid = new bmpPosInfo();
    private bmpPosInfo timePos_right = new bmpPosInfo();
    private Paint clockPaint = new Paint();//时间数字画笔
    //用户头像位置
    private bmpPosInfo userPos_left = new bmpPosInfo();
    private bmpPosInfo userPos_mid = new bmpPosInfo();
    private bmpPosInfo userPos_right = new bmpPosInfo();

    private bmpPosInfo pkrnumbkPos_left = new bmpPosInfo();
    private bmpPosInfo pkrnumbkPos_right = new bmpPosInfo();
    private bmpPosInfo pkrnumftPos_left = new bmpPosInfo();
    private bmpPosInfo pkrnumftPos_right = new bmpPosInfo();
    private Paint pkrnumPaint = new Paint();//时间数字画笔
    //退出按钮
    bmpPosInfo exitPos = new bmpPosInfo();
    //图片资源
    private Bitmap[] pkbmp = new Bitmap[data.PKR_COUNT];//所有卡牌的图
    private Bitmap bkgbmp = null;
    private Bitmap clock_mid_bmp = null;
    private Bitmap clock_left_bmp = null;
    private Bitmap clock_right_bmp = null;
    private Bitmap poker_bk_bmp = null;
    private Bitmap enfloot_bmp = null;
    private Bitmap unfloot_bmp = null;
    private Bitmap enloot_bmp = null;
    private Bitmap unloot_bmp = null;
    private Bitmap enpoke_bmp = null;
    private Bitmap enpoke_disable_bmp = null;
    private Bitmap unpoke_bmp = null;
    private Bitmap unpoke_disable_bmp = null;
    private Bitmap unplay_bmp = null;
    private Bitmap hint_bmp = null;
    private Bitmap hint_disable_bmp = null;
    private Bitmap btnnk_yellow_bmp = null;
    private Bitmap btnnk_gray_bmp = null;
    private Bitmap btnnk_green_bmp = null;
    private Bitmap exit_bmp = null;
    private Bitmap[] userdef_bmp = new Bitmap[3];
    private Bitmap userlo_bmp = null;
    private Bitmap userfm_bmp = null;


    public RunView(Context context) {
        super(context);
        tra = (RunActivity) context;
        getHolder().addCallback(this);//获取放映机//添加回调
        loadBitmap();
        InitData();
    }

    @SuppressLint("ResourceType")
    private void loadBitmap() {
        //加载图片资源
        BitmapFactory.Options opt = new BitmapFactory.Options();
        opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
        opt.inPurgeable = true;
        opt.inInputShareable = true;
        InputStream is = null;
        try {
            {
                is = getResources().openRawResource(R.drawable.b3);
                pkbmp[0] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b4);
                pkbmp[1] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b5);
                pkbmp[2] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b6);
                pkbmp[3] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b7);
                pkbmp[4] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b8);
                pkbmp[5] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b9);
                pkbmp[6] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b10);
                pkbmp[7] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b11);
                pkbmp[8] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b12);
                pkbmp[9] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b13);
                pkbmp[10] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b1);
                pkbmp[11] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.b2);
                pkbmp[12] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r3);
                pkbmp[13] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r4);
                pkbmp[14] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r5);
                pkbmp[15] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r6);
                pkbmp[16] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r7);
                pkbmp[17] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r8);
                pkbmp[18] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r9);
                pkbmp[19] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r10);
                pkbmp[20] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r11);
                pkbmp[21] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r12);
                pkbmp[22] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r13);
                pkbmp[23] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r1);
                pkbmp[24] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.r2);
                pkbmp[25] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m3);
                pkbmp[26] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m4);
                pkbmp[27] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m5);
                pkbmp[28] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m6);
                pkbmp[29] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m7);
                pkbmp[30] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m8);
                pkbmp[31] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m9);
                pkbmp[32] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m10);
                pkbmp[33] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m11);
                pkbmp[34] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m12);
                pkbmp[35] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m13);
                pkbmp[36] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m1);
                pkbmp[37] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.m2);
                pkbmp[38] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f3);
                pkbmp[39] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f4);
                pkbmp[40] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f5);
                pkbmp[41] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f6);
                pkbmp[42] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f7);
                pkbmp[43] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f8);
                pkbmp[44] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f9);
                pkbmp[45] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f10);
                pkbmp[46] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f11);
                pkbmp[47] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f12);
                pkbmp[48] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f13);
                pkbmp[49] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f1);
                pkbmp[50] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.f2);
                pkbmp[51] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.joker1);
                pkbmp[52] = BitmapFactory.decodeStream(is, null, opt);
                is = getResources().openRawResource(R.drawable.joker2);
                pkbmp[53] = BitmapFactory.decodeStream(is, null, opt);
            }
            bkgbmp = BitmapFactory.decodeResource(getResources(), R.drawable.bk);
            is = getResources().openRawResource(R.drawable.colock_mid);
            clock_mid_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.colock_left);
            clock_left_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.colock_right);
            clock_right_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.poker_bk);
            poker_bk_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.enfirstloot);
            enfloot_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.unfirstloot);
            unfloot_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.enloot);
            enloot_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.unloot);
            unloot_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.enpoke);
            enpoke_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.enpoke_disable);
            enpoke_disable_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.unpoke);
            unpoke_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.unpoke_disable);
            unpoke_disable_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.unplay);
            unplay_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.hint);
            hint_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.hint_disable);
            hint_disable_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.btnbk_yellow);
            btnnk_yellow_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.btnbk_gray);
            btnnk_gray_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.btnbk_green);
            btnnk_green_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.exit);
            exit_bmp = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.user_def1);
            userdef_bmp[0] = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.user_def2);
            userdef_bmp[1] = BitmapFactory.decodeStream(is, null, opt);
            is = getResources().openRawResource(R.drawable.user_def3);
            userdef_bmp[2] = BitmapFactory.decodeStream(is, null, opt);
        } catch (Resources.NotFoundException e) {
        }

    }

    private void InitData() {
        //手牌位置
        float width_height_ratio = (float) pkbmp[0].getWidth() / (float) pkbmp[0].getHeight();
        handpokerPos.height = (int) (tra.ScreenY / 3.5);
        handpokerPos.width = (int) (handpokerPos.height * width_height_ratio);
        handpoker_interval = handpokerPos.width / 3;
        handpoker_select_height = handpokerPos.width / 10;
        handpokerPos.top_y = tra.ScreenY - tra.ScreenY / 20 - handpokerPos.height;
        //打出的牌位置 中间
        pokedpokerPos_mid.height = handpokerPos.height / 2;
        pokedpokerPos_mid.width = handpokerPos.width / 2;
        pokedpokerPos_mid.top_y = handpokerPos.top_y - (handpokerPos.height / 9) - pokedpokerPos_mid.height;
        //用户头像位置
        float user_width_height_ratio = (float) userdef_bmp[0].getWidth() / (float) userdef_bmp[0].getHeight();
        userPos_left.height = tra.ScreenY / 6;
        userPos_left.width = (int) (userPos_left.height * user_width_height_ratio);
        userPos_left.left_x = 0;
        userPos_left.top_y = pokedpokerPos_mid.top_y - (tra.ScreenY / 20) - userPos_left.height;
        userPos_right.height = userPos_left.height;
        userPos_right.width = userPos_left.width;
        userPos_right.top_y = userPos_left.top_y;
        userPos_right.left_x = tra.ScreenX - userPos_right.width;
        userPos_mid.height = userPos_left.height;
        userPos_mid.width = userPos_left.width;
        userPos_mid.left_x = userPos_left.left_x;
        userPos_mid.top_y = tra.ScreenY - tra.ScreenY / 25 - userPos_mid.height;
        //其他用户剩余卡牌数量显示位置
        float pkrnum_width_height_ratio = (float) poker_bk_bmp.getWidth() / (float) poker_bk_bmp.getHeight();
        pkrnumbkPos_left.height = userPos_left.height / 2;
        pkrnumbkPos_left.width = (int) (pkrnumbkPos_left.height * pkrnum_width_height_ratio);
        pkrnumbkPos_left.left_x = userPos_left.left_x + userPos_left.width + 1;
        pkrnumbkPos_left.top_y = userPos_left.top_y + userPos_left.height - pkrnumbkPos_left.height;
        pkrnumbkPos_right.height = pkrnumbkPos_left.height;
        pkrnumbkPos_right.width = pkrnumbkPos_left.width;
        pkrnumbkPos_right.left_x = userPos_right.left_x - pkrnumbkPos_right.width - 1;
        pkrnumbkPos_right.top_y = pkrnumbkPos_left.top_y;
        //打出的牌位置 左右
        pokedpokerPos_left.height = pokedpokerPos_mid.height;
        pokedpokerPos_left.width = pokedpokerPos_mid.width;
        pokedpokerPos_left.top_y = pkrnumbkPos_left.top_y + pkrnumbkPos_left.height - pokedpokerPos_left.height;
        pokedpokerPos_left.left_x = pkrnumbkPos_left.left_x + pkrnumbkPos_left.width + tra.ScreenX / 60;
        pokedpokerPos_right.height = pokedpokerPos_left.height;
        pokedpokerPos_right.width = pokedpokerPos_left.width;
        pokedpokerPos_right.top_y = pokedpokerPos_left.top_y;
        pokedpoker_interval = handpoker_interval / 2;
        //地主三张牌位置
        lopokerPos.height = (int) (pokedpokerPos_left.height / 1.2);
        lopokerPos.width = (int) (pokedpokerPos_left.width / 1.2);
        lopokerPos.left_x = (int) (tra.ScreenX / 2 - (lopokerPos.width * 1.5));
        lopokerPos.top_y = tra.ScreenY / 30;
        //按钮背景位置
        float btnbk_width_height_ratio = (float) btnnk_yellow_bmp.getWidth() / (float) btnnk_yellow_bmp.getHeight();
        enlootbtnbkPos.height = tra.ScreenY / 10;
        enlootbtnbkPos.width = (int) (enlootbtnbkPos.height * btnbk_width_height_ratio);
        enlootbtnbkPos.top_y = handpokerPos.top_y - (handpokerPos.height / 9) - enlootbtnbkPos.height;
        unlootbtnbkPos.height = enlootbtnbkPos.height;
        unlootbtnbkPos.width = enlootbtnbkPos.width;
        unlootbtnbkPos.top_y = enlootbtnbkPos.top_y;
        enpokebtnbkPos.height = enlootbtnbkPos.height;
        enpokebtnbkPos.width = enlootbtnbkPos.width;
        enpokebtnbkPos.top_y = enlootbtnbkPos.top_y;
        unpokebtnbkPos.height = enlootbtnbkPos.height;
        unpokebtnbkPos.width = enlootbtnbkPos.width;
        unpokebtnbkPos.top_y = enlootbtnbkPos.top_y;
        hintbtnbkPos.height = enlootbtnbkPos.height;
        hintbtnbkPos.width = enlootbtnbkPos.width;
        hintbtnbkPos.top_y = enlootbtnbkPos.top_y;
        //按钮文字位置
        float btnfont_width_height_ratio = (float) enpoke_bmp.getWidth() / (float) enpoke_bmp.getHeight();
        btnfontPos.height = tra.ScreenY / 16;
        btnfontPos.width = (int) (btnfontPos.height * btnfont_width_height_ratio);
        btnfontPos.top_y = enlootbtnbkPos.top_y + (enlootbtnbkPos.height - btnfontPos.height) / 2;
        btnfontPos.left_x = (enlootbtnbkPos.width - btnfontPos.width) / 2;
        //时间闹钟图位置
        float clock_width_height_ratio = (float) clock_mid_bmp.getWidth() / (float) clock_mid_bmp.getHeight();
        clockPos_left.height = tra.ScreenY / 8;
        clockPos_left.width = (int) (clockPos_left.height * clock_width_height_ratio);
        clockPos_loot_mid.height = clockPos_left.height;
        clockPos_loot_mid.width = clockPos_left.width;
        clockPos_right.height = clockPos_left.height;
        clockPos_right.width = clockPos_left.width;
        clockPos_loot_mid.top_y = enlootbtnbkPos.top_y + enlootbtnbkPos.height - clockPos_loot_mid.height;
        clockPos_poke_mid.height = clockPos_left.height;
        clockPos_poke_mid.width = clockPos_left.width;
        clockPos_poke_mid.top_y = clockPos_loot_mid.top_y;
        clockPos_left.top_y = pokedpokerPos_left.top_y + pokedpokerPos_left.height - clockPos_left.height;
        clockPos_right.top_y = clockPos_left.top_y;
        clockPos_left.left_x = (tra.ScreenX / 2 - clockPos_left.width) / 2;
        clockPos_right.left_x = tra.ScreenX / 4 * 3 - clockPos_right.width / 2;
        clockPos_loot_mid.left_x = (tra.ScreenX - clockPos_loot_mid.width) / 2;
        //按钮背景位置补充 依赖上面数据
        enlootbtnbkPos.left_x = clockPos_loot_mid.left_x + clockPos_loot_mid.width + tra.ScreenX / 20;
        unlootbtnbkPos.left_x = clockPos_loot_mid.left_x - tra.ScreenX / 20 - enlootbtnbkPos.width;
        unpokebtnbkPos.left_x = (tra.ScreenX - enlootbtnbkPos.width * 3 - clockPos_loot_mid.width - (tra.ScreenX * 3) / 20) / 2;
        clockPos_poke_mid.left_x = unpokebtnbkPos.left_x + unpokebtnbkPos.width + tra.ScreenX / 20;
        hintbtnbkPos.left_x = clockPos_poke_mid.left_x + clockPos_poke_mid.width + tra.ScreenX / 20;
        enpokebtnbkPos.left_x = hintbtnbkPos.left_x + enlootbtnbkPos.width + tra.ScreenX / 20;
        //文字结果位置
        resultftPos_left.width = (int) (btnfontPos.width * 1.5);
        resultftPos_left.height = (int) (btnfontPos.height * 1.5);
        resultftPos_left.left_x = pkrnumbkPos_left.left_x + pkrnumbkPos_left.width + tra.ScreenX / 60;
        resultftPos_left.top_y = pokedpokerPos_left.top_y;
        resultftPos_right.width = resultftPos_left.width;
        resultftPos_right.height = resultftPos_left.height;
        resultftPos_right.top_y = resultftPos_left.top_y;
        resultftPos_right.left_x = pkrnumbkPos_right.left_x - tra.ScreenX / 60 - resultftPos_right.width;
        resultftPos_mid.height = resultftPos_left.height;
        resultftPos_mid.width = resultftPos_left.width;
        resultftPos_mid.left_x = (tra.ScreenX - resultftPos_mid.width) / 2;
        resultftPos_mid.top_y = pokedpokerPos_mid.top_y;

        //数据初始化
        for (int i = 0; i < data.PLAYER_NUMBER; i++) {
            handpoker_num[i] = 17;
            lastpoker_num[i] = 0;
            isLoot[i] = 3;
            isLastPoke[i] = 3;
        }
        for (int i = 0; i < data.HANDPOKER_MAXSIZE; i++) {
            handpoker[i] = new data_run().new poker();
            handpoker_isSelect[i] = false;
        }
        for (int i = 0; i < 3; i++) {
            threepoker[i] = new data_run().new poker();
        }
        for (int i = 0; i < data.PLAYER_NUMBER; i++) {
            for (int j = 0; j < data.HANDPOKER_MAXSIZE; j++) {
                lastpoker[i][j] = new data_run().new poker();
            }
        }

        double textSizeRatio = 30.0 / 720.0;
        int textSize = (int) (tra.ScreenY * textSizeRatio + 0.5);
        clockPaint.setTextSize(textSize);
        clockPaint.setColor(Color.BLACK);//设置画笔颜色
        clockPaint.setTypeface(Typeface.SANS_SERIF);
        Paint.FontMetricsInt fontMetrics = clockPaint.getFontMetricsInt();
        int textHeight = fontMetrics.bottom - fontMetrics.top;
        timePos_mid.top_y = clockPos_loot_mid.top_y + (int) (clockPos_loot_mid.height * 1.06 - textHeight);
        timePos_left.top_y = clockPos_left.top_y + (int) (clockPos_left.height * 1.06 - textHeight);
        timePos_right.top_y = clockPos_right.top_y + (int) (clockPos_right.height * 1.06 - textHeight);
        //剩余数量文字位置
        textSize = (int) (tra.ScreenY * textSizeRatio + 0.5);
        pkrnumPaint.setTextSize(textSize);
        pkrnumPaint.setColor(Color.WHITE);//设置画笔颜色
        pkrnumPaint.setTypeface(Typeface.SANS_SERIF);
        fontMetrics = pkrnumPaint.getFontMetricsInt();
        textHeight = fontMetrics.bottom - fontMetrics.top;
        pkrnumftPos_left.top_y = pkrnumbkPos_left.top_y + (int) (pkrnumbkPos_left.height * 1.35 - textHeight);
        pkrnumftPos_left.left_x = pkrnumbkPos_left.left_x + (pkrnumbkPos_left.width - getTextWidth(pkrnumPaint, Integer.toString(17))) / 2;
        pkrnumftPos_right.top_y = pkrnumftPos_left.top_y;
        pkrnumftPos_right.left_x = pkrnumbkPos_right.left_x + (pkrnumbkPos_right.width - getTextWidth(pkrnumPaint, Integer.toString(17))) / 2;

        float exit_width_height_ratio = (float) exit_bmp.getWidth() / (float) exit_bmp.getHeight();
        exitPos.height = tra.ScreenY / 7;
        exitPos.width = (int) (exit_width_height_ratio * clockPos_left.height);
        exitPos.left_x = (tra.ScreenX - clockPos_left.width) / 2;
        exitPos.top_y = tra.ScreenY / 2;
    }

    //surfaceview 创建
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i("surface", "surfaceCreated: ");
    }

    //surfaceview 发生变化
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.i("surface", "surfaceChanged: ");
    }

    //surfaceview 销毁
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i("surface", "surfaceDestroyed: ");
    }

    //绘制的判断
    public void drawRunMain() {
        if (isShowPoker) {
            showFirstRecvPoker();
        } else {
            Canvas canvas = getHolder().lockCanvas();
            if (canvas == null)
                return;
            canvas.drawBitmap(bkgbmp, null, new Rect(0, 0, tra.ScreenX, tra.ScreenY), null);
            if (game_status == data_run.run_status.LOOT) {
                drawRunOnLoot(canvas);
            } else if (game_status == data_run.run_status.POKE) {
                drawRunOnPoke(canvas);
            } else if (game_status == data_run.run_status.END) {
                drawOnEnd(canvas);
            }
            getHolder().unlockCanvasAndPost(canvas);
        }
    }

    //第一次接收到自己卡牌的动画绘制
    public void showFirstRecvPoker() {
        for (int i = 1; i < 17; i++) {
            Canvas canvas = getHolder().lockCanvas();
            if (canvas == null)
                return;
            canvas.drawBitmap(bkgbmp, null, new Rect(0, 0, tra.ScreenX, tra.ScreenY), null);
            int x = (tra.ScreenX - (i - 1) * handpoker_interval - handpokerPos.width) / 2;
            for (int j = 0; j <= i; j++, x += handpoker_interval) {
                handpokerPos.left_x = x;
                handpokerPos.drawBitmap(pkbmp[getPokerIndex(handpoker[j])], canvas);
            }
            //绘制地主牌 没有收到牌时会绘制牌背面
            drawLooterThreePoker(canvas);
            //提交canvas

            try {
                getHolder().unlockCanvasAndPost(canvas);
                Thread.sleep(300);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        isShowPoker = false;
    }

    //判断是否时第一个抢地主的变量
    private boolean isFirstLoot = true;

    //绘制抢地主阶段画面
    public void drawRunOnLoot(Canvas canvas) {
        if (sequence == turn) {//自己回合要绘制按钮
            if (isFirstLoot && sequence == 0) {//第一个抢地主的人 会设置成'叫地主'和'不叫'
                //不叫
                unlootbtnbkPos.drawBitmap(btnnk_yellow_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(unlootbtnbkPos.left_x);
                btnfontPos.drawBitmap(unfloot_bmp, canvas);
                //叫地主
                enlootbtnbkPos.drawBitmap(btnnk_green_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(enlootbtnbkPos.left_x);
                btnfontPos.drawBitmap(enfloot_bmp, canvas);
                isFirstLoot = false;
            } else {//普通绘制  '抢地主'和'不抢'
                //抢地主
                unlootbtnbkPos.drawBitmap(btnnk_yellow_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(unlootbtnbkPos.left_x);
                btnfontPos.drawBitmap(unloot_bmp, canvas);
                //不抢
                enlootbtnbkPos.drawBitmap(btnnk_green_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(enlootbtnbkPos.left_x);
                btnfontPos.drawBitmap(enloot_bmp, canvas);
            }
            //colock
            clockPos_loot_mid.drawBitmap(clock_mid_bmp, canvas);
            canvas.drawText(Integer.toString(remain_sec), timePos_mid.left_x, timePos_mid.top_y, clockPaint);
        } else if (last_seq == turn) {//绘制左边colock
            clockPos_left.drawBitmap(clock_mid_bmp, canvas);
            canvas.drawText(Integer.toString(remain_sec), timePos_left.left_x, timePos_left.top_y, clockPaint);
        } else if (next_seq == turn) {//绘制右边colock
            clockPos_right.drawBitmap(clock_mid_bmp, canvas);
            canvas.drawText(Integer.toString(remain_sec), timePos_right.left_x, timePos_right.top_y, clockPaint);
        }
        //绘制抢地主情况
        drawLootResult(canvas);
        //绘制手牌
        drawHandpoker(canvas);
        //绘制地主牌 没有收到牌时会绘制牌背面
        drawLooterThreePoker(canvas);
        //绘制用户头像和手牌数量
        drawUsersInfo(canvas);
    }

    //绘制出牌阶段画面
    public void drawRunOnPoke(Canvas canvas) {
        if (sequence == turn) {//自己回合要绘制按钮
            if (isFirst) {//先手绘制的图标
                //灰色不出
                unpokebtnbkPos.drawBitmap(btnnk_gray_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(unpokebtnbkPos.left_x);
                btnfontPos.drawBitmap(unpoke_disable_bmp, canvas);
            } else {
                //黄色不出
                unpokebtnbkPos.drawBitmap(btnnk_yellow_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(unpokebtnbkPos.left_x);
                btnfontPos.drawBitmap(unpoke_bmp, canvas);
            }
            //clock
            clockPos_poke_mid.drawBitmap(clock_mid_bmp, canvas);
            canvas.drawText(Integer.toString(remain_sec), timePos_mid.left_x, timePos_mid.top_y, clockPaint);
            //提示
            hintbtnbkPos.drawBitmap(btnnk_yellow_bmp, canvas);
            btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(hintbtnbkPos.left_x);
            btnfontPos.drawBitmap(hint_bmp, canvas);
            //出牌
            handpoker_have_one_select = false;
            for (int i = 0; i < handpoker_num[sequence]; i++) {
                if (handpoker_isSelect[i]) {
                    handpoker_have_one_select = true;
                    break;
                }
            }
            if (handpoker_have_one_select) {
                //正常出牌
                enpokebtnbkPos.drawBitmap(btnnk_green_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(enpokebtnbkPos.left_x);
                btnfontPos.drawBitmap(enpoke_bmp, canvas);
            } else {
                //灰色出牌
                enpokebtnbkPos.drawBitmap(btnnk_gray_bmp, canvas);
                btnfontPos.left_x = getBtnFontPosXByBtnBkPosX(enpokebtnbkPos.left_x);
                btnfontPos.drawBitmap(enpoke_disable_bmp, canvas);
            }


        } else if (last_seq == turn) {//绘制左边colock
            clockPos_left.drawBitmap(clock_mid_bmp, canvas);
            canvas.drawText(Integer.toString(remain_sec), timePos_left.left_x, timePos_left.top_y, clockPaint);
        } else if (next_seq == turn) {//绘制右边colock
            clockPos_right.drawBitmap(clock_mid_bmp, canvas);
            canvas.drawText(Integer.toString(remain_sec), timePos_right.left_x, timePos_right.top_y, clockPaint);
        }
        //绘制手牌
        drawHandpoker(canvas);
        //绘制打出的牌
        drawPokedPoker(canvas);
        //绘制地主牌
        drawLooterThreePoker(canvas);
        //绘制用户头像和手牌数量
        drawUsersInfo(canvas);
    }

    //绘制结束阶段画面
    public void drawOnEnd(Canvas canvas) {
        Paint paint = new Paint();
        paint.setColor(Color.RED);
        paint.setTextSize(40);
        int start_x = (tra.ScreenX - getTextWidth(paint, "游戏结束")) / 2;
        canvas.drawText("游戏结束", start_x, tra.ScreenY / 2 - resultftPos_mid.height, paint);
        exitPos.drawBitmap(exit_bmp, canvas);
    }

    //绘制地主三张牌
    private void drawLooterThreePoker(Canvas canvas) {
        if (isGetThreepoker) {//已经收到地主牌
            int ix = lopokerPos.left_x;
            for (int i = 0; i < 3; i++) {
                lopokerPos.drawBitmap(pkbmp[getPokerIndex(threepoker[i])], canvas);
                lopokerPos.left_x += lopokerPos.width;
            }
            lopokerPos.left_x = ix;
        } else {//没有收到地主牌  绘制卡牌背面
            int ix = lopokerPos.left_x;
            for (int i = 0; i < 3; i++) {
                lopokerPos.drawBitmap(poker_bk_bmp, canvas);
                lopokerPos.left_x += lopokerPos.width;
            }
            lopokerPos.left_x = ix;
        }
    }

    //会出抢地主情况
    private void drawLootResult(Canvas canvas) {
        if (turn != last_seq) {
            //左边
            if (isLoot[last_seq] == 1) {//抢地主
                resultftPos_left.drawBitmap(enloot_bmp, canvas);
            } else if (isLoot[last_seq] == 0) {//不抢地主
                resultftPos_left.drawBitmap(unloot_bmp, canvas);
            }
        }
        if (turn != sequence) {
            //中间
            if (isLoot[sequence] == 1) {//抢地主
                resultftPos_mid.drawBitmap(enloot_bmp, canvas);
            } else if (isLoot[sequence] == 0) {//不抢地主
                resultftPos_mid.drawBitmap(unloot_bmp, canvas);
            }
        }
        if (turn != next_seq) {
            //右边
            if (isLoot[next_seq] == 1) {//抢地主
                resultftPos_right.drawBitmap(enloot_bmp, canvas);
            } else if (isLoot[next_seq] == 0) {//不抢地主
                resultftPos_right.drawBitmap(unloot_bmp, canvas);
            }
        }
    }

    //绘制手牌
    private void drawHandpoker(Canvas canvas) {
        for (int i = 0; i < handpoker_num[sequence]; i++) {
            if (handpoker_isSelect[i]) {//绘制卡牌弹起状态
                handpokerPos.left_x = handpoker_left_x + handpoker_interval * i;
                handpokerPos.top_y -= handpoker_select_height;
                handpokerPos.drawBitmap(pkbmp[getPokerIndex(handpoker[i])], canvas);
                handpokerPos.top_y += handpoker_select_height;
            } else {//绘制卡牌正常状态
                handpokerPos.left_x = handpoker_left_x + handpoker_interval * i;
                handpokerPos.drawBitmap(pkbmp[getPokerIndex(handpoker[i])], canvas);
            }
        }
    }

    //绘制打出的牌
    private void drawPokedPoker(Canvas canvas) {
        if (turn != last_seq) {
            //左边
            if (isLastPoke[last_seq] == 0) {//绘制'不出'图片
                resultftPos_left.drawBitmap(unpoke_bmp, canvas);
            } else if (isLastPoke[last_seq] == 1) {//绘制打出的牌
                int ix = pokedpokerPos_left.left_x;
                for (int i = 0; i < lastpoker_num[last_seq]; i++) {
                    pokedpokerPos_left.drawBitmap(pkbmp[getPokerIndex(lastpoker[last_seq][i])], canvas);
                    pokedpokerPos_left.left_x += pokedpoker_interval;
                }
                pokedpokerPos_left.left_x = ix;
            }
        }
        if (turn != sequence) {
            //中间
            if (isLastPoke[sequence] == 0) {//绘制'不出'图片
                resultftPos_mid.drawBitmap(unpoke_bmp, canvas);
            } else if (isLastPoke[sequence] == 1) {//绘制打出的牌
                int ix = pokedpokerPos_mid.left_x;
                for (int i = 0; i < lastpoker_num[sequence]; i++) {
                    pokedpokerPos_mid.drawBitmap(pkbmp[getPokerIndex(lastpoker[sequence][i])], canvas);
                    pokedpokerPos_mid.left_x += pokedpoker_interval;
                }
                pokedpokerPos_mid.left_x = ix;
            }
        }
        if (turn != next_seq) {
            //右边
            if (isLastPoke[next_seq] == 0) {//绘制'不出'图片
                resultftPos_right.drawBitmap(unpoke_bmp, canvas);
            } else if (isLastPoke[next_seq] == 1) {//绘制打出的牌
                int ix = pokedpokerPos_right.left_x;
                for (int i = 0; i < lastpoker_num[next_seq]; i++) {
                    pokedpokerPos_right.drawBitmap(pkbmp[getPokerIndex(lastpoker[next_seq][i])], canvas);
                    pokedpokerPos_right.left_x += pokedpoker_interval;
                }
                pokedpokerPos_right.left_x = ix;
            }
        }
    }

    //绘制用户头像和手牌数量
    private void drawUsersInfo(Canvas canvas) {
        if (game_status == data_run.run_status.LOOT) {
            //绘制默认头像
            if (sequence == 0) {
                userPos_mid.drawBitmap(userdef_bmp[0], canvas);
                userPos_left.drawBitmap(userdef_bmp[2], canvas);
                userPos_right.drawBitmap(userdef_bmp[1], canvas);
            } else if (last_seq == 0) {
                userPos_mid.drawBitmap(userdef_bmp[1], canvas);
                userPos_left.drawBitmap(userdef_bmp[0], canvas);
                userPos_right.drawBitmap(userdef_bmp[2], canvas);
            } else if (next_seq == 0) {
                userPos_mid.drawBitmap(userdef_bmp[2], canvas);
                userPos_left.drawBitmap(userdef_bmp[1], canvas);
                userPos_right.drawBitmap(userdef_bmp[0], canvas);
            }
            //绘制手牌
            pkrnumbkPos_left.drawBitmap(poker_bk_bmp, canvas);
            canvas.drawText(Integer.toString(handpoker_num[last_seq]), pkrnumftPos_left.left_x, pkrnumftPos_left.top_y, pkrnumPaint);
            pkrnumbkPos_right.drawBitmap(poker_bk_bmp, canvas);
            canvas.drawText(Integer.toString(handpoker_num[next_seq]), pkrnumftPos_right.left_x, pkrnumftPos_right.top_y, pkrnumPaint);
        } else if (game_status == data_run.run_status.POKE) {
            //绘制地主和农民头像


            //绘制默认头像
            if (sequence == 0) {
                userPos_mid.drawBitmap(userdef_bmp[0], canvas);
                userPos_left.drawBitmap(userdef_bmp[2], canvas);
                userPos_right.drawBitmap(userdef_bmp[1], canvas);
            } else if (last_seq == 0) {
                userPos_mid.drawBitmap(userdef_bmp[1], canvas);
                userPos_left.drawBitmap(userdef_bmp[0], canvas);
                userPos_right.drawBitmap(userdef_bmp[2], canvas);
            } else if (next_seq == 0) {
                userPos_mid.drawBitmap(userdef_bmp[2], canvas);
                userPos_left.drawBitmap(userdef_bmp[1], canvas);
                userPos_right.drawBitmap(userdef_bmp[0], canvas);
            }
            //绘制谁是地主文字
            if (looter_sequence == sequence) {
                canvas.drawText("地主", userPos_mid.left_x, userPos_mid.top_y, clockPaint);
            } else if (looter_sequence == last_seq) {
                canvas.drawText("地主", userPos_left.left_x, userPos_left.top_y, clockPaint);
            } else if (looter_sequence == next_seq) {
                canvas.drawText("地主", userPos_right.left_x, userPos_right.top_y, clockPaint);
            }
            //绘制手牌
            pkrnumbkPos_left.drawBitmap(poker_bk_bmp, canvas);
            canvas.drawText(Integer.toString(handpoker_num[last_seq]), pkrnumftPos_left.left_x, pkrnumftPos_left.top_y, pkrnumPaint);
            pkrnumbkPos_right.drawBitmap(poker_bk_bmp, canvas);
            canvas.drawText(Integer.toString(handpoker_num[next_seq]), pkrnumftPos_right.left_x, pkrnumftPos_right.top_y, pkrnumPaint);
            if (looter_sequence == last_seq) {

            } else if (looter_sequence == sequence) {

            } else if (looter_sequence == next_seq) {

            }


        }
    }

    //接收到抢地主数据的调用
    public void recvLoot(int turn, boolean lootOrNot) {
        isLoot[turn] = lootOrNot ? 1 : 0;
        //不需要更新画面  在下次收到跟换回合时会更新
    }

    //游戏回合剩余时间改变的调用
    public void recvTime() {
        //重新测量剩余时间字符长度
        if (turn == sequence) {
            if (game_status == data_run.run_status.LOOT) {
                timePos_mid.left_x = clockPos_loot_mid.left_x + (clockPos_left.width - getTextWidth(clockPaint, Integer.toString(remain_sec))) / 2;
            } else if (game_status == data_run.run_status.POKE) {
                timePos_mid.left_x = clockPos_poke_mid.left_x + (clockPos_left.width - getTextWidth(clockPaint, Integer.toString(remain_sec))) / 2;
            }
        } else if (turn == next_seq) {
            timePos_right.left_x = clockPos_right.left_x + (clockPos_left.width - getTextWidth(clockPaint, Integer.toString(remain_sec))) / 2;
        } else if (turn == last_seq) {
            timePos_left.left_x = clockPos_left.left_x + (clockPos_left.width - getTextWidth(clockPaint, Integer.toString(remain_sec))) / 2;
        }
        //更新画面
        drawRunMain();
    }

    //游戏进度改变的调用 //一般需要改变画面
    public void changeStatus(int run_status) throws IOException {
        game_status = run_status;
        remain_sec = 15;
        drawRunMain();
    }

    //抢地主阶段回合改变的调用
    public void changeTurnOnLoot(int newTurn) {
        turn = newTurn;
        remain_sec = 15;
        drawRunMain();
    }

    //出牌阶段回合改变的调用
    public void changeTurnOnPoke(int newTurn) throws Exception {
        turn = newTurn;
        isFirst = tra.ips.readInt() == 0 ? false : true;
        remain_sec = 15;
        drawRunMain();
        int last_turn = turn - 1 < 0 ? 2 : turn - 1;
        if (handpoker_num[last_turn] == 0) {
            game_status = data_run.run_status.END;
            Thread.sleep(3000);
            drawRunMain();
        }
    }

    //将抢地主结果保存
    public void setLootResult(data_run.looter_data ld) {
        looter_sequence = ld.sequence;
        for (int i = 0; i < 3; i++) {
            threepoker[i] = ld.threepk[i];
        }
        //如果自己是地主 将牌加到自己handpoker 并添加地主的handpoker_num
        if (looter_sequence == sequence) {
            addThreeToHandpoker();
            handpoker_num[looter_sequence] += 3;
            changeHandpokerPosX();
        } else {//如果不是 仅仅添加地主的handpoker_num
            handpoker_num[looter_sequence] += 3;
        }
        //设置收到了地主牌
        isGetThreepoker = true;
        //主动更新画面
        drawRunMain();
    }

    //自己是地主时将地主牌添加到自己手牌
    public void addThreeToHandpoker() {
        int pos_move = 3;
        int handpokerIndex = 16;
        int threepokerIndex = 2;
        while ((threepokerIndex) >= 0 && (handpokerIndex >= 0)) {
            if (handpoker[handpokerIndex].kind > threepoker[threepokerIndex].kind) {//kind越大，排在越前
                handpoker[handpokerIndex + pos_move] = threepoker[threepokerIndex];
                threepokerIndex--;
                pos_move--;
            } else if (handpoker[handpokerIndex].kind == threepoker[threepokerIndex].kind) {
                if (handpoker[handpokerIndex].color > threepoker[threepokerIndex].color) {
                    //颜色越小 排在越前
                    handpoker[handpokerIndex + pos_move] = handpoker[handpokerIndex];
                    handpokerIndex--;
                } else {
                    handpoker[handpokerIndex + pos_move] = threepoker[threepokerIndex];
                    threepokerIndex--;
                    pos_move--;
                }
            } else {//kind越小，排在越后
                handpoker[handpokerIndex + pos_move] = handpoker[handpokerIndex];
                handpokerIndex--;
            }
        }

        while (threepokerIndex >= 0) {
            handpoker[threepokerIndex] = threepoker[threepokerIndex];
            threepokerIndex--;
        }
    }

    //将出的牌加入lastpoker//改变手牌数据//通知更改绘制的 handpoker_pos_x
    public void changeHandpokerData() {
        if (lastpoker_num[sequence] != 0) {
            //改变手牌数据
            int pos = 0;
            for (int i = 0; i < handpoker_num[sequence]; i++) {
                while ((handpoker[i + pos].color == lastpoker[sequence][pos].color) && (handpoker[i + pos].kind == lastpoker[sequence][pos].kind) && (pos < readypokepokers_num)) {
                    pos++;
                }
                if (pos > 0)
                    handpoker[i] = handpoker[i + pos];
            }
            //更改绘制的 handpoker_pos_x
            changeHandpokerPosX();
        }
        //把所有弹起的 牌设置成未弹起状态
        for (int i = 0; i < handpoker_num[sequence]; i++) {
            if (handpoker_isSelect[i] == true)
                handpoker_isSelect[i] = false;
        }

    }

    //当手牌数量改变时更改绘制的 handpoker_pos_x
    public void changeHandpokerPosX() {
        handpoker_left_x = (tra.ScreenX - (handpoker_num[sequence] - 1) * handpoker_interval - handpokerPos.width) / 2;
    }

    //别人出了牌//返回出的卡牌   接受卡牌
    public void changePokedpokerData() throws IOException {
        //接受牌类型和点数
        last_pokers_type = tra.ips.readInt();
        last_pokers_count = tra.ips.readInt();
        //改变出过的牌的数据
        lastpoker_num[turn] = tra.ips.readInt();
        isLastPoke[turn] = lastpoker_num[turn] == 0 ? 0 : 1;
        if (lastpoker_num[turn] != 0) {
            for (int i = 0; i < lastpoker_num[turn]; i++) {
                lastpoker[turn][i].recv(tra.ips);
            }
            //减少手牌数量
            handpoker_num[turn] -= lastpoker_num[turn];
            if (turn == sequence) {
                //改变自己已出牌的pokedpoker_self_pos_x
                pokedpokerPos_mid.left_x = (tra.ScreenX - (lastpoker_num[turn] - 1) * pokedpoker_interval - pokedpokerPos_mid.width) / 2;
                //自己回合接下来就改变自己的手牌数据
                changeHandpokerData();
            } else if (turn == last_seq) {//更改pokedpoker_left_pos_x
                //重新测量剩余卡牌数量文字长度
                pkrnumftPos_left.left_x = pkrnumbkPos_left.left_x + (pkrnumbkPos_left.width - getTextWidth(pkrnumPaint, Integer.toString(handpoker_num[turn]))) / 2;
                //left 不需要修改
                //pokedpokerPos_left.left_x = 0;
            } else if (turn == next_seq) {//或pokedpoker_right_pos_x
                //重新测量剩余卡牌数量文字长度
                pkrnumftPos_right.left_x = pkrnumbkPos_right.left_x + (pkrnumbkPos_right.width - getTextWidth(pkrnumPaint, Integer.toString(handpoker_num[turn]))) / 2;
                //right
                pokedpokerPos_right.left_x = pkrnumbkPos_right.left_x - tra.ScreenX / 40 - (lastpoker_num[turn] - 1) * pokedpoker_interval - pokedpokerPos_right.width;
            } else {
                //错误
            }
        }
        drawRunMain();
    }

    //根据卡牌数据获得卡牌在数组中的下标
    public int getPokerIndex(data_run.poker poker) {
        if (poker.kind <= data_run.pkr_kind.POKER_A)
            return poker.kind + poker.color;
        else if (poker.kind == data_run.pkr_kind.POKER_2)
            return 12 + poker.color;
        else if (poker.kind == data_run.pkr_kind.POKER_L_JOKER)
            return 52;
        else
            return 53;
    }

    //抢地主阶段的触摸事件处理
    private void buttonClickOnLoot(int x, int y) {
        //抢地主 不抢 两个
        //抢地主按钮
        if ((x >= enlootbtnbkPos.left_x) && (x <= enlootbtnbkPos.getRightX()) && (y >= enlootbtnbkPos.top_y) && (y <= enlootbtnbkPos.getBottomY())) {
            tra.ClickLoot(true);
        } else if ((x >= unlootbtnbkPos.left_x) && (x <= unlootbtnbkPos.getRightX()) && (y >= unlootbtnbkPos.top_y) && (y <= unlootbtnbkPos.getBottomY())) {
            tra.ClickLoot(false);
        }
    }

    //出牌阶段的触摸事件处理
    private void buttonClickOnPoke(int x, int y) {
        //不出   提示  出牌  卡牌
        if (sequence == turn) {//自己回合可以触摸按钮
            //不出 只能自己不是先手
            if ((!isFirst) && (x >= unpokebtnbkPos.left_x) && (x <= unpokebtnbkPos.getRightX()) && (y >= unpokebtnbkPos.top_y) && (y <= unpokebtnbkPos.getBottomY())) {
                for (int i = 0; i < handpoker_num[sequence]; i++) {
                    handpoker_isSelect[i] = false;
                }
                readypokepokers_num = 0;
                tra.ClickUnPoke();
            } else if ((x >= hintbtnbkPos.left_x) && (x <= hintbtnbkPos.getRightX()) && (y >= hintbtnbkPos.top_y) && (y <= hintbtnbkPos.getBottomY())) {
                //先不做处理
                if (setHintPoker())
                    drawRunMain();
                else {
                    for (int i = 0; i < handpoker_num[sequence]; i++) {
                        handpoker_isSelect[i] = false;
                    }
                    readypokepokers_num = 0;
                    tra.ClickUnPoke();
                }
            } else if ((x >= enpokebtnbkPos.left_x) && (x <= enpokebtnbkPos.getRightX()) && (y >= enpokebtnbkPos.top_y) && (y <= enpokebtnbkPos.getBottomY())) {
                //判断有多少牌谈起了弹起的牌需要大于0
                int k = 0;
                for (int i = 0; i < handpoker_num[sequence]; i++) {
                    if (handpoker_isSelect[i]) {
                        readypokepokers[k] = i;
                        k++;
                    }
                }
                if (k != 0) {
                    readypokepokers_num = k;
                    tra.ClickPoke(readypokepokers, k);
                } else {//没选出牌给提示
                    readypokepokers_num = 0;
                }
            }
        }

        if ((x >= handpoker_left_x) && (x <= (handpoker_left_x + handpokerPos.width + handpoker_interval * (handpoker_num[sequence] - 1))
                && (y >= (handpokerPos.top_y - handpoker_select_height)) && (y <= handpokerPos.getBottomY()))) {
            for (int i = handpoker_num[sequence] - 1; i >= 0; i--) {
                if (x >= (i * handpoker_interval + handpoker_left_x)) {
                    if (handpoker_isSelect[i]) {//弹起状态卡牌区域
                        if (y <= (handpokerPos.getBottomY() - handpoker_select_height)) {
                            //这样才算选中
                            handpoker_isSelect[i] = handpoker_isSelect[i] ? false : true;
                            //如果是触摸触发更新屏幕  则需要下面添加
                            drawRunMain();
                            break;
                        }
                    } else {//没弹起状态卡牌区域
                        if (y >= handpokerPos.top_y) {
                            handpoker_isSelect[i] = handpoker_isSelect[i] ? false : true;
                            //如果是触摸触发更新屏幕  则需要下面添加
                            drawRunMain();
                            break;
                        }
                    }
                }
            }
        }
    }

    //游戏结束阶段的触摸事件处理
    private void buttonClickOnEnd(int x, int y) {
        if ((x >= exitPos.left_x) && (x <= exitPos.getRightX()) && (y >= exitPos.top_y) && (y <= exitPos.getBottomY())) {
            tra.ClickExit();
        }
    }

    //触摸事件分发
    private void buttonClick(int x, int y) {
        switch (game_status) {
            case data_run.run_status.LOOT:
                if (sequence == turn)
                    buttonClickOnLoot(x, y);
                break;
            case data_run.run_status.POKE:
                buttonClickOnPoke(x, y);
                break;
            case data_run.run_status.END:
                buttonClickOnEnd(x, y);
                break;
        }
    }

    //相应触摸事件
    public void handleTouch(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                break;
            case MotionEvent.ACTION_UP:
                buttonClick((int) event.getX(), (int) event.getY());
                break;
        }
        Log.i("pos", "handleTouch: " + Float.toString(event.getX()) + " " + Float.toString(event.getY()));
    }

    //自动出牌提示
    private boolean setHintPoker() {//找出最佳选择 和一个副选
        if (sequence == turn) {
            for (int i = 0; i < handpoker_num[sequence]; i++) {
                handpoker_isSelect[i] = false;
            }
            if (isFirst) {//第一个出牌 随便出
                if (!onFurwhtwo(-1)) {
                    lastpoker_num[last_seq] = 15;
                    while (lastpoker_num[last_seq] >= 10 && !onPlanetwo(-1)) {
                        lastpoker_num[last_seq] -= 5;
                    }
                    if (lastpoker_num[last_seq] < 10) {
                        lastpoker_num[last_seq] = 16;
                        while (lastpoker_num[last_seq] >= 8 && !onPlaneone(-1)) {
                            lastpoker_num[last_seq] -= 4;
                        }
                    }
                    if (lastpoker_num[last_seq] < 8) {
                        lastpoker_num[last_seq] = 15;
                        while (lastpoker_num[last_seq] >= 6 && !onPlanezer(-1)) {
                            lastpoker_num[last_seq] -= 3;
                        }
                    }
                    if (lastpoker_num[last_seq] < 6) {
                        lastpoker_num[last_seq] = 16;
                        while (lastpoker_num[last_seq] >= 6 && !onDoublecn(-1)) {
                            lastpoker_num[last_seq] -= 2;
                        }
                    }
                    if (lastpoker_num[last_seq] < 6) {
                        lastpoker_num[last_seq] = 16;
                        while (lastpoker_num[last_seq] >= 5 && !onContinue(-1)) {
                            lastpoker_num[last_seq] -= 1;
                        }
                    }
                    if (lastpoker_num[last_seq] < 5) {
                        if (!onThrwhtwo(-1)) {
                            if (!onThrwhone(-1)) {
                                if (!onThrwhzre(-1)) {
                                    if (!onTwosame(-1)) {
                                        if (!onFoursame(-1)) {
                                            if (!onSingle(-1)) {
                                                return false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return true;
            } else {
                switch (last_pokers_type) {
                    case data_run.pkr_type.SINGLE:
                        return onSingle(last_pokers_count);
                    case data_run.pkr_type.TWOSAME:
                        return onTwosame(last_pokers_count);
                    case data_run.pkr_type.THRWHZRE:
                        return onThrwhzre(last_pokers_count);
                    case data_run.pkr_type.THRWHONE:
                        return onThrwhone(last_pokers_count);
                    case data_run.pkr_type.THRWHTWO:
                        return onThrwhtwo(last_pokers_count);
                    case data_run.pkr_type.CONTINUE:
                        return onContinue(last_pokers_count);
                    case data_run.pkr_type.DOUBLECN:
                        return onDoublecn(last_pokers_count);
                    case data_run.pkr_type.PLANEZRE:
                        return onPlanezer(last_pokers_count);
                    case data_run.pkr_type.PLANEONE:
                        return onPlaneone(last_pokers_count);
                    case data_run.pkr_type.PLANETWO:
                        return onPlanetwo(last_pokers_count);
                    case data_run.pkr_type.FURWHTWO:
                        return onFurwhtwo(last_pokers_count);
                    case data_run.pkr_type.FOURSAME:
                        return onFoursame(last_pokers_count);
                    case data_run.pkr_type.TWOJOKER:
                        return false;
                }
            }
        }

        return false;
    }

    //自动出牌提示用到的函数 功能是获得1-4张相同的牌
    private int[] getPokerPos(int strat_index, int end_index, int same_num, int last_pokers_count) {
        int[] ints = getPokerPos(strat_index, end_index, same_num, last_pokers_count, true);
        if (ints[0] == -1)
            return getPokerPos(strat_index, end_index, same_num, last_pokers_count, false);
        return ints;
    }

    //自动出牌提示用到的函数 功能是获得1-4张相同的牌
    //获取1张/2张/3张/4张相同牌的起始和结束位置 闭区间
    //返回值 大小为2的int数组  参数1:起始位置(包含),参数2:结束位置(包含)参数3:相同数量牌的张数,参数4:牌的点数,参数5:是否是严格模式
    //严格模式:不能再4张相同的里面选3张相同 不能再3张相同选2张相同。。。以此类推;非严格模式则可以;
    //return:如果找不到 返回int[-1,-1];找到ints[0]必然不是-1;
    private int[] getPokerPos(int strat_index, int end_index, int same_num, int last_pokers_count, boolean isStrict) {
        if ((same_num > (end_index - strat_index + 1)) || strat_index > end_index)
            return new int[]{-1, -1};

        if (same_num == 1 && end_index == 0)
            return new int[]{0, 0};

        int p = end_index - 1, q = end_index;

        if (same_num >= 2 && same_num <= 4) {
            if (isStrict) {
                while (p >= 0) {
                    if (handpoker[q].kind > last_pokers_count && handpoker[p].kind == handpoker[q].kind) {
                        if ((q - p) == (same_num - 1)) {
                            if (((p - 1 >= 0) && (handpoker[p].kind != handpoker[p - 1].kind)) || (p - 1 < 0)) {
                                return new int[]{p, q};
                            }
                        }
                        --p;
                    } else {
                        q = p;
                        --p;
                    }
                }
            } else {
                while (p >= 0) {
                    if (handpoker[q].kind > last_pokers_count && handpoker[p].kind == handpoker[q].kind) {
                        if ((q - p) == (same_num - 1)) {
                            return new int[]{p, q};
                        }
                        --p;
                    } else {
                        q = p;
                        --p;
                    }
                }
            }

        } else if (same_num == 1) {
            if (isStrict) {
                while (q >= 0) {
                    if ((q - 1 < 0 && handpoker[q].kind > last_pokers_count) || (handpoker[q].kind > last_pokers_count && handpoker[q - 1].kind != handpoker[q].kind)) {
                        return new int[]{q, -1};
                    } else if (q - 1 >= 0 && handpoker[q - 1].kind == handpoker[q].kind) {
                        while (q - 1 >= 0 && handpoker[q - 1].kind == handpoker[q].kind) {
                            --q;
                        }
                        --q;
                        if (q < 0)
                            return new int[]{-1, -1};
                    } else {
                        --q;
                    }
                }
            } else {
                while (q >= 0) {
                    if (handpoker[q].kind > last_pokers_count) {
                        return new int[]{q, -1};
                    } else {
                        --q;
                    }
                }
            }
        }
        return new int[]{-1, -1};
    }

    //自动出牌提示用到的函数 功能是提示单张牌
    private boolean onSingle(int last_pokers_count) {
        if (handpoker_num[sequence] == 1) {
            if (handpoker[0].kind > last_pokers_count) {
                handpoker_isSelect[0] = true;
                return true;
            } else
                return false;
        }
        int[] ints = getPokerPos(0, handpoker_num[sequence] - 1, 1, last_pokers_count);
        if (ints[0] == -1) {
            return onFoursame(-1);
        }
        handpoker_isSelect[ints[0]] = true;
        return true;
    }

    //自动出牌提示用到的函数 功能是提示两张相同牌
    private boolean onTwosame(int last_pokers_count) {
        if (handpoker_num[sequence] < 2) {
            return false;
        }
        int[] ints = getPokerPos(0, handpoker_num[sequence] - 1, 2, last_pokers_count);
        if (ints[0] == -1) {
            return onFoursame(-1);
        }
        for (int i = ints[0]; i <= ints[1]; i++) {
            handpoker_isSelect[i] = true;
        }
        return true;
    }

    //自动出牌提示用到的函数 功能是提示三张相同牌
    private boolean onThrwhzre(int last_pokers_count) {
        if (handpoker_num[sequence] < 3) {
            return false;
        }
        int[] ints = getPokerPos(0, handpoker_num[sequence] - 1, 3, last_pokers_count);
        if (ints[0] == -1) {
            return onFoursame(-1);
        }
        for (int i = ints[0]; i <= ints[1]; i++) {
            handpoker_isSelect[i] = true;
        }
        return true;
    }

    //自动出牌提示用到的函数 功能是提示三带一
    private boolean onThrwhone(int last_pokers_count) {
        if (handpoker_num[sequence] < 4) {
            return false;
        }
        int[] ints = getPokerPos(0, handpoker_num[sequence] - 1, 3, last_pokers_count);
        if (ints[0] == -1)
            return onFoursame(-1);
        int[] ints2 = getPokerPos(ints[1] + 1, handpoker_num[sequence] - 1, 1, -1, true);
        if (ints2[0] == -1) {
            ints2 = getPokerPos(0, ints[0] - 1, 1, -1, true);
            if (ints2[0] == -1) {
                ints2 = getPokerPos(ints[1] + 1, handpoker_num[sequence] - 1, 1, -1, false);
                if (ints2[0] == -1) {
                    ints2 = getPokerPos(0, ints[0] - 1, 1, -1, false);
                    if (ints2[0] == -1) {
                        return onFoursame(-1);
                    }
                }
            }
        }
        for (int i = ints[0]; i <= ints[1]; i++) {
            handpoker_isSelect[i] = true;
        }
        handpoker_isSelect[ints2[0]] = true;
        return true;
    }

    //自动出牌提示用到的函数 功能是提示三带二
    private boolean onThrwhtwo(int last_pokers_count) {
        if (handpoker_num[sequence] < 5) {
            return onFoursame(-1);
        }
        int[] ints = getPokerPos(0, handpoker_num[sequence] - 1, 3, last_pokers_count);
        if (ints[0] == -1)
            return onFoursame(-1);
        else {
            int[] ints2 = getPokerPos(ints[1] + 1, handpoker_num[sequence] - 1, 2, -1, true);
            if (ints2[0] == -1) {
                ints2 = getPokerPos(0, ints[0] - 1, 2, -1, true);
                if (ints2[0] == -1) {
                    ints2 = getPokerPos(ints[1] + 1, handpoker_num[sequence] - 1, 2, -1, false);
                    if (ints2[0] == -1) {
                        ints2 = getPokerPos(0, ints[0] - 1, 2, -1, false);
                        if (ints2[0] == -1) {
                            return onFoursame(-1);
                        }
                    }
                }
            }
            for (int i = ints[0]; i <= ints[1]; i++) {
                handpoker_isSelect[i] = true;
            }
            for (int i = ints2[0]; i <= ints2[1]; i++) {
                handpoker_isSelect[i] = true;
            }
            return true;
        }
    }

    //三带对提示
    private boolean onThrwhtwo(){
        int[] poker_pos = getPokerPos(0, handpoker_num[sequence] - 1, 3, last_pokers_count, true);

        if(poker_pos[0] == -1)
            return onFoursame(-1);
        else{
            //poker_pos_out中[0,1]存放比找到的poker_pos小的对的位置
            //poker_pos_out中[2,3]存放比找到的poker_pos大的对的位置
            //poker_pos_out中[4,5]存放比找到的poker_pos小的三张牌取两张的的位置
            //poker_pos_out中[6,7]存放比找到的poker_pos大的三张牌取两张的的位置
            int poker_pos_out[] ={-1,-1,-1,-1,-1,-1,-1,-1};
            int temp[] = {-1, -1};
            temp = getPokerPos(poker_pos[1] + 1, handpoker_num[sequence]-1, 2, -1, true);
            if(temp[0] != -1){
                poker_pos_out[0] = temp[0];
                poker_pos_out[1] = temp[1];
            }
            temp = getPokerPos(0, poker_pos[0] - 1, 2, -1, true);
            if(temp[0] != -1){
                poker_pos_out[2] = temp[0];
                poker_pos_out[3] = temp[1];
            }
            temp = getPokerPos(poker_pos[1] + 1, handpoker_num[sequence]-1, 2, -1, false);
            if(temp[0] != -1){
                poker_pos_out[4] = temp[0];
                poker_pos_out[5] = temp[1];
            }
            temp = getPokerPos(0, poker_pos[0] - 1, 2, -1, false);
            if(temp[0] != -1){
                poker_pos_out[6] = temp[0];
                poker_pos_out[7] = temp[1];
            }
            int i = 0;
            while(i < 8 && poker_pos_out[i ++] == -1){
                break;
            }
            if(i < 7){
                for (int z = poker_pos[0]; z <= poker_pos[1]; z++) {
                    handpoker_isSelect[z] = true;
                }
                handpoker_isSelect[poker_pos_out[i]] = true;
                handpoker_isSelect[poker_pos_out[i + 1]] = true;
                return true;
            }else{
                return false;
            }
        }

    }
    //自动出牌提示用到的函数 功能是提示顺子
    private boolean onContinue(int last_pokers_count) {
        int last_poker_num = lastpoker_num[last_seq] == 0 ? lastpoker_num[next_seq] : lastpoker_num[last_seq];
        if (handpoker_num[sequence] < last_poker_num)
            return onFoursame(-1);
        int[] poss = new int[last_poker_num];
        int rm = last_poker_num - 1;
        int i = handpoker_num[sequence] - 2, h = handpoker_num[sequence] - 1;
        for (; i > 0; --i) {
            if (handpoker[h].kind == (handpoker[i].kind - 1) && handpoker[h].kind > last_pokers_count) {
                poss[rm] = h;
                --rm;
                h = i;
                if (rm == 0) {//找到满足条件
                    poss[rm] = h;
                    for (int ps : poss) {
                        handpoker_isSelect[ps] = true;
                    }
                    return true;
                }
            } else if (handpoker[h].kind == handpoker[i].kind && handpoker[h].kind > last_pokers_count) {
                //必须什么都不做
            } else {
                h = i;
                rm = last_poker_num - 1;
            }
        }

        return onFoursame(-1);
    }

    //自动出牌提示用到的函数 功能是提示连对
    private boolean onDoublecn(int last_pokers_count) {
        int last_poker_num = lastpoker_num[last_seq] == 0 ? lastpoker_num[next_seq] : lastpoker_num[last_seq];
        if (handpoker_num[sequence] < last_poker_num)
            return onFoursame(-1);
        int[] poss = new int[last_poker_num / 2];
        int rm = last_poker_num / 2 - 1;
        boolean preTwoSame = false;
        int i = handpoker_num[sequence] - 2, h = handpoker_num[sequence] - 1;
        for (; i > 0; --i) {
            if (preTwoSame && (handpoker[h].kind == (handpoker[i].kind - 1)) && handpoker[h].kind > last_pokers_count) {
                preTwoSame = false;
                if (handpoker[i].kind == handpoker[i - 1].kind) {
                    poss[rm] = h;
                    --rm;
                    h = i;
                    if (rm == 0) {//找到满足条件
                        poss[rm] = h;
                        for (int ps : poss) {
                            handpoker_isSelect[ps] = true;
                            handpoker_isSelect[ps - 1] = true;
                        }
                        return true;
                    }
                } else {
                    --i;
                    h = i;
                    rm = last_poker_num / 2 - 1;
                }
            } else if (handpoker[h].kind == handpoker[i].kind && handpoker[h].kind > last_pokers_count) {
                preTwoSame = true;
            } else {
                h = i;
                rm = last_poker_num / 2 - 1;
            }
        }

        return onFoursame(-1);

    }

    //自动出牌飞机提示用到的函数 选出可以管住上一家的飞机的位置
    private int[] getPlanePos(int iprePokerNum) {
        int out[] = {-1, -1};
        if (handpoker_num[sequence] < iprePokerNum)
            return out;

        int num_poker_san = 0;
        if (iprePokerNum == 8 || iprePokerNum == 10)
            num_poker_san = 6;
        else if (iprePokerNum == 12 || iprePokerNum == 15)
            num_poker_san = 9;
        else if (iprePokerNum == 16 || iprePokerNum == 20)
            num_poker_san = 12;

        int i = 0, j = 0, count = 0, searching_count = 0;
        for (i = handpoker_num[sequence] - 1; i > 0; --i) {
            boolean judge_last_poker = false;
            for (j = i; j >= 0; --j) {
                if (count == 0) {
                    if (j - 2 >= 0 && handpoker[j].kind > last_pokers_count && handpoker[j - 1].kind == handpoker[j].kind && handpoker[j].kind == handpoker[j - 2].kind) {
                        count += 3;
                        j -= 2;
                        judge_last_poker = true;
                    } else {
                        break;
                    }
                } else {
                    if (handpoker[j].kind - handpoker[j + 1].kind == 1 && j - 2 >= 0 && handpoker[j].kind > last_pokers_count && handpoker[j - 1].kind == handpoker[j].kind && handpoker[j].kind == handpoker[j - 2].kind) {
                        count += 3;
                        j -= 2;
                        judge_last_poker = true;
                    } else {
                        break;
                    }
                }
                //下面这个if用来允许备选的三张牌是从炸弹中选出
                if (j - 1 >= 0 && judge_last_poker) {
                    if (handpoker[j].kind == handpoker[j - 1].kind)
                        j--;
                }
                if (count == num_poker_san) {
                    out[0] = j;
                    out[1] = i;
                    return out;
                }
            }
        }
        return out;
    }

    //自动出牌提示用到的函数 功能是提示飞机不带
    private boolean onPlanezer(int last_pokers_count) {
        int iprePokerNum = lastpoker_num[last_seq] == 0 ? lastpoker_num[next_seq] : lastpoker_num[last_seq];
        int[] planePos = getPlanePos(iprePokerNum);
        if (planePos[0] == -1)
            return onFoursame(-1);
        else {
            if (handpoker[planePos[1]].kind == 13)
                return onFoursame(-1);
            else {
                for (int z = planePos[0]; z <= planePos[1]; z++) {
                    handpoker_isSelect[z] = true;
                }
                return true;
            }
        }
    }

    //自动出牌提示用到的函数 功能是提示飞机带单
    private boolean onPlaneone(int last_pokers_count) {
        int iprePokerNum = lastpoker_num[last_seq] == 0 ? lastpoker_num[next_seq] : lastpoker_num[last_seq];
        int[] planePos = getPlanePos(iprePokerNum);
        if (planePos[0] == -1) {
            return onFoursame(-1);
        } else {
            if (handpoker[planePos[1]].kind == 13)
                return onFoursame(-1);
            else {
                int poker_pos_out[] = {-1, -1, -1, -1, -1, -1, -1, -1};
                int temp[] = {-1, -1};
                temp = getPokerPos(planePos[1] + 1, handpoker_num[sequence] - 1, 1, -1, true);
                poker_pos_out[0] = temp[0];
                if (temp[0] != -1) {//找小于poker_pos的第二个单牌
                    temp = getPokerPos(planePos[1] + 1, temp[0] - 1, 1, -1, true);
                    poker_pos_out[1] = temp[0];
                }
                temp = getPokerPos(0, planePos[0] - 1, 1, -1, true);
                poker_pos_out[2] = temp[0];
                if (temp[0] != -1) {
                    temp = getPokerPos(0, temp[0] - 1, 1, -1, true);
                    poker_pos_out[3] = temp[0];
                }
                temp = getPokerPos(planePos[1] + 1, handpoker_num[sequence] - 1, 2, -1, false);
                poker_pos_out[4] = temp[0];
                poker_pos_out[5] = temp[1];
                temp = getPokerPos(0, planePos[0] - 1, 2, -1, false);
                poker_pos_out[6] = temp[0];
                poker_pos_out[7] = temp[1];
                int count = 0;
                for (int i = 0; i < 8; i++) {
                    if (poker_pos_out[i] != -1)
                        count++;
                    if (count == iprePokerNum / 4)
                        break;
                }
                if (count == iprePokerNum / 4) {
                    for (int z = planePos[0]; z <= planePos[1]; z++) {
                        handpoker_isSelect[z] = true;
                    }
                    for (int z = 0; z < 8; z++) {
                        if (count == 0)
                            break;
                        int pos = poker_pos_out[z];
                        if (poker_pos_out[z] != -1) {
                            handpoker_isSelect[pos] = true;
                            count--;
                        }
                    }
                    return true;
                }
            }
        }
        return false;
    }

    //自动出牌提示用到的函数 功能是提示飞机带对
    private boolean onPlanetwo(int last_pokers_count) {
        int iprePokerNum = lastpoker_num[last_seq] == 0 ? lastpoker_num[next_seq] : lastpoker_num[last_seq];
        int[] planePos = getPlanePos(iprePokerNum);
        if (planePos[0] == -1) {
            return onFoursame(-1);
        } else {
            if (handpoker[planePos[1]].kind == 13) {
                return onFoursame(-1);
            } else {
                //poker_pos_out中[0,1]存放比找到的poker_pos小的对的位置
                //poker_pos_out中[2,3]存放比找到的poker_pos大的对的位置
                //poker_pos_out中[4,5]存放比找到的poker_pos小的三张牌取两张的的位置
                //poker_pos_out中[6,7]存放比找到的poker_pos大的三张牌取两张的的位置
                int poker_pos_out[] = {-1, -1, -1, -1, -1, -1, -1, -1};
                int temp[] = {-1, -1};
                int count = 0;//统计找出的对的数目
                temp = getPokerPos(planePos[1] + 1, handpoker_num[sequence] - 1, 2, -1, true);
                if (temp[0] != -1) {
                    poker_pos_out[0] = temp[0];
                    poker_pos_out[1] = temp[1];
                    count++;
                }
                temp = getPokerPos(0, planePos[0] - 1, 2, -1, true);
                if (temp[0] != -1) {
                    poker_pos_out[2] = temp[0];
                    poker_pos_out[3] = temp[1];
                    count++;
                }
                temp = getPokerPos(planePos[1] + 1, handpoker_num[sequence] - 1, 2, -1, false);
                if (temp[0] != -1) {
                    poker_pos_out[4] = temp[0];
                    poker_pos_out[5] = temp[1];
                    count++;
                }
                temp = getPokerPos(0, planePos[0] - 1, 2, -1, false);
                if (temp[0] != -1) {
                    poker_pos_out[6] = temp[0];
                    poker_pos_out[7] = temp[1];
                    count++;
                }
                if (count >= iprePokerNum / 5) {
                    for (int z = planePos[0]; z <= planePos[1]; z++) {
                        handpoker_isSelect[z] = true;
                    }
                    int num = iprePokerNum / 5;//需要几个对
                    for (int z = 0; z < 8; z++) {
                        if (num == 0)
                            break;
                        int pos = poker_pos_out[z];
                        if (poker_pos_out[z] != -1) {
                            handpoker_isSelect[pos] = true;
                            handpoker_isSelect[poker_pos_out[++z]] = true;
                            num--;
                        }
                    }
                    return true;
                }
            }
        }
        return false;
    }

    //自动出牌提示用到的函数 功能是提示四代二
    private boolean onFurwhtwo(int last_pokers_count) {
        int[] poker_pos = getPokerPos(0, handpoker_num[sequence] - 1, 4, last_pokers_count, true);
        if (poker_pos[0] == -1)
            return onFoursame(-1);
        else {
            //poker_pos_out中[0,1]存放两张比找到的poker_pos小的对的位置
            //poker_pos_out中[2,3]存放两张比找到的poker_pos大的对的位置
            //poker_pos_out中[4,5]存放比找到的poker_pos小的对的位置
            //poker_pos_out中[6,7]存放比找到的poker_pos大的对的位置

            int poker_pos_out[] = {-1, -1, -1, -1, -1, -1, -1, -1};
            int temp[] = {-1, -1};
            temp = getPokerPos(poker_pos[1] + 1, handpoker_num[sequence] - 1, 1, -1, true);
            poker_pos_out[0] = temp[0];
            if (temp[0] != -1) {//找小于poker_pos的第二个单牌
                temp = getPokerPos(poker_pos[1] + 1, temp[0] - 1, 1, -1, true);
                poker_pos_out[1] = temp[0];
            }
            temp = getPokerPos(0, poker_pos[0] - 1, 1, -1, true);
            poker_pos_out[2] = temp[0];
            if (temp[0] != -1) {
                temp = getPokerPos(0, temp[0] - 1, 1, -1, true);
                poker_pos_out[3] = temp[0];
            }
            temp = getPokerPos(poker_pos[1] + 1, handpoker_num[sequence] - 1, 2, -1, false);
            poker_pos_out[4] = temp[0];
            poker_pos_out[5] = temp[1];
            temp = getPokerPos(0, poker_pos[0] - 1, 2, -1, false);
            poker_pos_out[6] = temp[0];
            poker_pos_out[7] = temp[1];
            int count = 0;
            int first_pos = -1, second_pos = -1;
            for (int i = 0; i < 8; i++) {
                if (poker_pos_out[i] != -1)
                    count++;
                if (count == 2)
                    break;
            }

            if (count == 2) {
                for (int z = poker_pos[0]; z <= poker_pos[1]; z++) {
                    handpoker_isSelect[z] = true;
                }
                for (int z = 0; z < 8; z++) {
                    if (count == 0)
                        break;
                    int pos = poker_pos_out[z];
                    if (poker_pos_out[z] != -1) {
                        handpoker_isSelect[pos] = true;
                        count--;
                    }
                }
                return true;
            } else {
                return false;
            }
        }
    }

    //自动出牌提示用到的函数 功能是炸弹和王炸
    private boolean onFoursame(int last_pokers_count) {
        if (handpoker_num[sequence] < 4)
            return false;
        int[] ints = getPokerPos(0, handpoker_num[sequence] - 1, 4, last_pokers_count);
        if (ints[0] == -1) {
            if (handpoker[0].kind == data_run.pkr_kind.POKER_B_JOKER && handpoker[1].kind == data_run.pkr_kind.POKER_L_JOKER) {
                handpoker_isSelect[0] = true;
                handpoker_isSelect[1] = true;
                return true;
            }
        } else {
            for (int i = ints[0]; i <= ints[1]; i++) {
                handpoker_isSelect[i] = true;
            }
            return true;
        }
        return false;
    }

    //获取字符穿宽度
    public int getTextWidth(Paint paint, String str) {
        int iRet = 0;
        if (str != null && str.length() > 0) {
            int len = str.length();
            float[] widths = new float[len];
            paint.getTextWidths(str, widths);
            for (int j = 0; j < len; j++) {
                iRet += (int) Math.ceil(widths[j]);
            }
        }
        return iRet;
    }

    //根据按钮背景的x位置获取按钮字体的x位置
    public int getBtnFontPosXByBtnBkPosX(int btnbkPosX) {
        return (btnbkPosX + (enlootbtnbkPos.width - btnfontPos.width) / 2);
    }

    //游戏进度
    public int game_status = data_run.run_status.BEGIN;
    //游戏当前回合
    public int turn = -1;
    public boolean isFirst = false;
    //游戏回合剩余时间
    public int remain_sec = 15;
    //玩家自身是sequence  上一家是last_seq 下一家是next_seq
    public int sequence = -1;
    public int last_seq = -1;
    public int next_seq = -1;
    //玩家剩余手牌数量
    public int[] handpoker_num = new int[data.PLAYER_NUMBER];
    //出过的牌类型和点数
    int last_pokers_type = -1;
    int last_pokers_count = 0;
    //handpoker自己手牌数据，handpoker_isSelect是否被选择，handpoker_have_one_select是否有一张被选择；
    public data_run.poker[] handpoker = new data_run.poker[data.HANDPOKER_MAXSIZE];
    private boolean[] handpoker_isSelect = new boolean[data.HANDPOKER_MAXSIZE];
    private boolean handpoker_have_one_select = false;
    //threepoker地主三张牌数据，isGetThreepoker是否获得这三种牌
    public data_run.poker[] threepoker = new data_run.poker[3];
    private boolean isGetThreepoker = false;
    //地主的sequence
    public int looter_sequence = -1;
    //有没有叫地主  0不抢 1抢 3没有操作 初始化为3
    public int[] isLoot = new int[data.PLAYER_NUMBER];
    //要出的牌的下标
    public int[] readypokepokers = new int[data.HANDPOKER_MAXSIZE];
    public int readypokepokers_num = 0;
    //lastpoker上回合出过的牌数据，lastpoker_num牌数量
    public data_run.poker[][] lastpoker = new data_run.poker[data.PLAYER_NUMBER][data.HANDPOKER_MAXSIZE];
    public int[] lastpoker_num = new int[data.PLAYER_NUMBER];
    //上回合有没有出牌  0没出 1出了 3没有操作 初始化为3
    public int[] isLastPoke = new int[data.PLAYER_NUMBER];
}