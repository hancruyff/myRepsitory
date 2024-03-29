package com.example.myapplication;

import java.io.Serializable;

public class schedule implements Serializable {
    String Title;
    String TODO;
    int S_hour;
    int S_minute;
    int E_hour;
    int E_minute;
    int id;
    String date;
    public schedule(String T,String t,int sh,int sm,int eh,int em,int id,String D){
        this.Title=T;
        this.TODO=t;
        this.S_hour=sh;
        this.S_minute=sm;
        this.E_hour=eh;
        this.E_minute=em;
        this.id=id;
        this.date=D;
    }
    public String getTitle(){
        return Title;
    }
    public String getTODO(){
        return TODO;
    }
    public int getS_hour(){
        return S_hour;
    }
    public int getS_minute(){
        return S_minute;
    }
    public int getE_hour(){
        return E_hour;
    }
    public int getE_minute(){
        return E_minute;
    }
    public int getId(){
        return id;
    }
    public String getDate(){return date;}
}
