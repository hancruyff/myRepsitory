package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteOpenHelper;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.github.sundeepk.compactcalendarview.CompactCalendarView;
import com.github.sundeepk.compactcalendarview.domain.Event;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Locale;
class DBHelper extends SQLiteOpenHelper {
    private static final String DATABASE_NAME="myschedule.db";
    private static final int DATABASE_VERSION=11;

    public DBHelper(Context context){
        super(context,DATABASE_NAME,null,DATABASE_VERSION);
    }
    @Override
    public void onCreate(SQLiteDatabase db) {
        //db.execSQL("CREATE TABLE schedule (_id INTEGER PRIMARY KEY"+" AUTOINCREMENT, YEAR INTEGER, MONTH INTEGER,DAY INTEGER,HOUR INTEGER, MINUTE INTEGER,TITLE TEXT,TODO TEXT);");
        db.execSQL("CREATE TABLE schedule (_id INTEGER PRIMARY KEY"+" AUTOINCREMENT, DATE TEXT,HOUR INTEGER,MINUTE INTEGER,E_HOUR INTEGER,E_MINUTE INTEGER,TITLE TEXT,TODO TEXT)");
        db.execSQL("CREATE TABLE outlay (_id INTEGER PRIMARY KEY "+" AUTOINCREMENT, DATE TEXT "+",MONEY TEXT)");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int i, int i1) {
        db.execSQL("DROP TABLE IF EXISTS schedule");
        db.execSQL("DROP TABLE IF EXISTS outlay");
        onCreate(db);
    }
}

public class MainActivity extends AppCompatActivity {
    final String TAG = "calendar test";
    private SimpleDateFormat dateFormatForDisplaying = new SimpleDateFormat("yyyy-MM-dd", Locale.KOREA);
    private SimpleDateFormat dateFormatForMonth = new SimpleDateFormat("yyyy년 MM월", Locale.KOREA);
    private SimpleDateFormat dateFormatForMonth2 = new SimpleDateFormat("yyyy-MM", Locale.KOREA);
    private static final String CHANNEL_ID = "my_channel_id";
    private static final int NOTIFICATION_ID = 1;
    String date1;
    String title;
    String todo;
    DBHelper helper;
    SQLiteDatabase db;
    int month;
    int year;
    int day;
    int c=0;
    int c1=0;
    String str;
    String date2;
    Event[] e={};
    String date3;
    String today;
    ArrayList<schedule> schedules=new ArrayList<>();





    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        CompactCalendarView compactCalendarView = (CompactCalendarView) findViewById(R.id.compactcalendar_view);

        TextView textView_month = (TextView) findViewById(R.id.textView_month);


        helper=new DBHelper(this);
        try{
            db=helper.getWritableDatabase();
        }catch(SQLiteException ex){
            db=helper.getReadableDatabase();
        }
        compactCalendarView.removeAllEvents();
        textView_month.setText(dateFormatForMonth.format(compactCalendarView.getFirstDayOfCurrentMonth()));
        str=dateFormatForMonth2.format(compactCalendarView.getFirstDayOfCurrentMonth());
        compactCalendarView.setFirstDayOfWeek(Calendar.SUNDAY);

        Cursor cursor;
        /*cursor=db.rawQuery("SELECT DATE,HOUR,TITLE FROM schedule WHERE DATE LIKE'"+str+"'%;",null);
        while(cursor.moveToNext()){
            c++;
        }
        c--;*/
        //cursor=db.rawQuery("SELECT DATE,HOUR,TITLE FROM schedule WHERE DATE LIKE '%" + str + "%';",null);
        cursor=db.rawQuery("SELECT DATE,HOUR,TITLE,MINUTE,TODO,_id FROM schedule;",null);
        while(cursor.moveToNext()){

            int i=0;
            Date trans_date1 = null;
            Date date = compactCalendarView.getFirstDayOfCurrentMonth();
            Log.v("test","1");
            String yyymm = dateFormatForMonth2.format(date);
            Log.v("test","2");
            String d;
            String T;
            date3=cursor.getString(0);
            T=cursor.getString(2);
            try {
                trans_date1 = dateFormatForDisplaying.parse(date3);
                Log.v("test","7");
            } catch (ParseException e) {
                e.printStackTrace();
                Log.v("test","8");
            }
            schedule S=new schedule(T,cursor.getString(4),cursor.getInt(1),cursor.getInt(3),0,0,cursor.getInt(5), cursor.getString(0));
            schedules.add(S);

            setAlarm(this,S);
            long time1 = trans_date1.getTime();
            /*e[i]=new Event(Color.GREEN, time1, T);
            compactCalendarView.addEvent(e[i]);
            if(i>=c)
                break;
            i++;*/
            Event e=new Event(Color.GREEN,time1,T);
            compactCalendarView.addEvent(e);

        }

        // 이벤트 관련 코드
        compactCalendarView.setListener(new CompactCalendarView.CompactCalendarViewListener() {
            @Override
            public void onDayClick(Date dateClicked) {
                SimpleDateFormat transFormat = new SimpleDateFormat("yyyy-MM-dd");
                date1 = transFormat.format(dateClicked);
                Intent intent=new Intent(MainActivity.this,showall.class);
                intent.putExtra("date",date1);
                startActivity(intent);

            }

            @Override
            public void onMonthScroll(Date firstDayOfNewMonth) {
                textView_month.setText(dateFormatForMonth.format(firstDayOfNewMonth));



            }
        });

    }
    /*private void setAlarm(schedule schedule) {
        AlarmManager alarmManager = (AlarmManager) getSystemService(Context.ALARM_SERVICE);
        Intent intent = new Intent(this, AlarmReceiver.class);
        intent.putExtra("title", schedule.getTitle());
        intent.putExtra("todo", schedule.getTODO());
        intent.putExtra("id",schedule.getId());
        PendingIntent pendingIntent = PendingIntent.getBroadcast(this, schedule.getId(), intent, PendingIntent.FLAG_UPDATE_CURRENT);

        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.HOUR_OF_DAY, schedule.getS_hour());
        calendar.set(Calendar.MINUTE, schedule.getS_minute());
        calendar.set(Calendar.SECOND, 0);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            alarmManager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), pendingIntent);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            alarmManager.setExact(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), pendingIntent);
        } else {
            alarmManager.set(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), pendingIntent);
        }
    }*/
    private void setAlarm(Context context, schedule schedule) {
        // schedule 객체로부터 알림에 필요한 정보 추출
        int id = schedule.getId();
        String title = schedule.getTitle();
        String date = schedule.getDate();
        int hour = schedule.getS_hour();
        int minute = schedule.getS_minute();

        // 날짜와 시간 정보를 처리하여 timeInMillis 값 계산
        Calendar calendar = Calendar.getInstance();
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm", Locale.KOREA);
        try {
            Date scheduleDateTime = dateFormat.parse(date + " " + hour + ":" + minute);
            calendar.setTime(scheduleDateTime);
        } catch (ParseException e) {
            e.printStackTrace();
        }
        long timeInMillis = calendar.getTimeInMillis();

        if (timeInMillis <= System.currentTimeMillis()) {
            // 이미 지난 시간인 경우 알림을 설정하지 않음
            return;
        }

        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        Intent intent = new Intent(context, AlarmReceiver.class);
        intent.putExtra("title", schedule.getTitle());
        intent.putExtra("todo", schedule.getTODO());
        intent.putExtra("id", id);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, id, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            alarmManager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, timeInMillis, pendingIntent);
            Log.v("test",id+" A1");
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            alarmManager.setExact(AlarmManager.RTC_WAKEUP, timeInMillis, pendingIntent);
            Log.v("test",id+" A2");
        } else {
            alarmManager.set(AlarmManager.RTC_WAKEUP, timeInMillis, pendingIntent);
            Log.v("test",id+" A3");
        }
    }

}
