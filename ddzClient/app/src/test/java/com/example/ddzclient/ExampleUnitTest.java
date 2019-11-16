package com.example.ddzclient;

import org.junit.Test;

import java.io.InputStream;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() {
        assertEquals(4, 2 + 2);
    }
    /*
     * 常规登录测试
     * */
    @Test
    public void testUsualLogin() {

        try {
            //创建登录类对象
            LoginActivity loginActivity = new LoginActivity();

            //账号密码都为空
            loginActivity.usualLogin("", "");

            //账号为空，密码不为空的情况
            //loginActivity.usualLogin("", "1");

            //账号不为空，密码为空的情况
            //loginActivity.usualLogin("1", "");

            //账号密码都不为空且账号不正确，但密码正确的情况
            //loginActivity.usualLogin("2", "1");

            //账号密码都不为空且账号正确，但密码不正确的情况
            //loginActivity.usualLogin("1", "2");

            //账号密码都不为空且都正确的情况
            //loginActivity.usualLogin("1", "1");

            String testResult = loginActivity.testResult;
            if (testResult != null) {
                System.out.println("测试通过！测试结果为：" + testResult);
            }
        } catch (Exception e) {
            System.out.println("测试不通过！");
        }

    }

    /*
     * 人脸登录测试
     * */
    @Test
    public void testFaceLogin() {

        try {
            //创建人脸登录类对象
            MainActivity mainActivity = new MainActivity();

            //未上传照片
            InputStream inputStream1 = null;
            mainActivity.testFaceLogin(inputStream1);

            //上传的照片中有人脸且已绑定账号
            /*InputStream inputStream2 = new FileInputStream("C:\\Users\\ASUS\\Desktop\\images\\1.jpg");
            mainActivity.testFaceLogin(inputStream2);*/

            //上传的照片中有人脸但未绑定账号
            /*InputStream inputStream3 = new FileInputStream("C:\\Users\\ASUS\\Desktop\\images\\2.jpg");
            mainActivity.testFaceLogin(inputStream3);*/

            //上传的照片中没有人脸
            /*InputStream inputStream4 = new FileInputStream("C:\\Users\\ASUS\\Desktop\\images\\3.jpg");
            mainActivity.testFaceLogin(inputStream4);*/

            String testResult = mainActivity.testResult;
            if (testResult != null) {
                System.out.println("测试通过！测试结果为：" + testResult);
            } else {
                System.out.println("测试出错！");
            }
        } catch (Exception e) {
            System.out.println("测试出现异常");
        }

    }


}