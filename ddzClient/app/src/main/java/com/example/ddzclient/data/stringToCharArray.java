package com.example.ddzclient.data;

public class stringToCharArray {
    public static void stringToCharAry(char[] chars,String string){
        int i=0;
        while(i<string.length())
        {
            chars[i] = string.toCharArray()[i];
            i++;
        }
        chars[i]='\0';
    }

    public static String charArrayToString(char[] chars){
        int i=0;
        String string = new String();
        while(chars[i]!='\0'){
            string+=chars[i];
            i++;
        }
        return string;
    }
}
