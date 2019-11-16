package com.example.ddzclient.data;

import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class CppOutputStream extends FilterOutputStream {

    public CppOutputStream(OutputStream out) {
        super(out);
    }

    public final void write(byte b[]) throws IOException {
        out.write(b, 0, b.length);
    }

    public final void write(byte b[], int off, int len) throws IOException {
        out.write(b, off, len);
    }

    public final void writeFully(byte b[]) throws IOException {
        writeFully(b, 0, b.length);
    }

    public final void writeFully(byte b[], int off, int len) throws IOException {
       out.write(b,off,len);
    }


    public final void writeByte(byte b) throws IOException {
        out.write(b);
    }


    public final void writeChar(char ch) throws IOException {
        int ch1 = (byte)(ch % (1<<8)); //先发
        int ch2 = (byte)(ch >> 8);//后发
        out.write(ch1);
        out.write(ch2);
    }

    public final void writeString(char[] chs)throws IOException{
        for(int i=0;i<chs.length;i++){
            writeChar(chs[i]);
        }
    }

    public final void writeBool(boolean b)throws IOException {
        out.write(b==true?1:0);
    }

    public final void writeInt(int i) throws IOException {
        int ch1 = (byte)(i % (1<<8)); //先发
        int ch2 = (byte)((i>>8)%(1<<8));
        int ch3 = (byte)((i>>16)%(1<<8));
        int ch4 = (byte)(i>>24);
        out.write(ch1);
        out.write(ch2);
        out.write(ch3);
        out.write(ch4);
    }

    private byte writeBuffer[] = new byte[8];

    public final void readLong(long l) throws IOException {
        int ch1 = (byte)(l % (1<<8)); //先发
        int ch2 = (byte)((l>>8)%(1<<8));
        int ch3 = (byte)((l>>16)%(1<<8));
        int ch4 = (byte)((l>>24)%(1<<8));
        int ch5 = (byte)((l>>32)%(1<<8));
        int ch6 = (byte)((l>>40)%(1<<8));
        int ch7 = (byte)((l>>48)%(1<<8));
        int ch8 = (byte)(l>>56);
        out.write(ch1);
        out.write(ch2);
        out.write(ch3);
        out.write(ch4);
        out.write(ch5);
        out.write(ch6);
        out.write(ch7);
        out.write(ch8);
    }

    public final void writeFloat(float f) throws IOException {
        writeInt(Float.floatToIntBits(f));
    }



}

