package com.example.myapplication;

import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TimePicker;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class editschedule extends AppCompatActivity {
    DBHelper helper;
    SQLiteDatabase db;
    int ID;
    Button editbutton;
    Button deletebutton;
    EditText title;
    EditText todo;
    TimePicker startT;
    TimePicker endT;
    String Title;
    String TOdo;
    String date;
    int s_hour;
    int s_minute;
    int e_hour;
    int e_minute;
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.editschedule);
        Intent intent=getIntent();
        ID=intent.getIntExtra("ID",0);
        editbutton=(Button) findViewById(R.id.edit);
        deletebutton=(Button) findViewById(R.id.deletebutton);
        title=(EditText)findViewById(R.id.Titletext);
        todo=(EditText)findViewById(R.id.Todotext);
        startT=(TimePicker)findViewById(R.id.Start_timepicker);
        endT=(TimePicker)findViewById(R.id.End_timepicker);
        helper=new DBHelper(this);
        try{
            db=helper.getWritableDatabase();
        }catch(SQLiteException ex){
            db=helper.getReadableDatabase();
        }
        Cursor cursor;
        cursor=db.rawQuery("SELECT TITLE,TODO,HOUR,MINUTE,E_HOUR,E_MINUTE,DATE FROM schedule WHERE _id='"+ID+"';",null);
        while (cursor.moveToNext()){
            Title=cursor.getString(0);
            TOdo=cursor.getString(1);
            s_hour=cursor.getInt(2);
            s_minute=cursor.getInt(3);
            e_hour=cursor.getInt(4);
            e_minute=cursor.getInt(5);
            date=cursor.getString(6);
        }
        title.setText(Title);
        todo.setText(TOdo);
        startT.setHour(s_hour);
        startT.setMinute(s_minute);
        endT.setHour(e_hour);
        endT.setMinute(e_minute);
        deletebutton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                db.execSQL("DELETE FROM schedule WHERE _id='"+ID+"';");
                cancelNotification(ID);
                Log.v("test",ID+" D");
                Toast.makeText(getApplicationContext(),"일정삭제",Toast.LENGTH_SHORT).show();
                Intent intent =new Intent(editschedule.this, showall.class);
                intent.putExtra("date", date);
                startActivity(intent);
            }
        });
        editbutton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                db.execSQL("UPDATE schedule SET TITLE='"+title.getText().toString()+"',TODO='"+todo.getText().toString()+"',HOUR='"+startT.getHour()+"',MINUTE='"+startT.getMinute()+"',E_HOUR='"+endT.getHour()+"',E_MINUTE='"+endT.getMinute()+"' WHERE _id='"+ID+"';");
                Toast.makeText(getApplicationContext(),"일정수정",Toast.LENGTH_SHORT).show();
                Intent intent =new Intent(editschedule.this,showall.class);
                intent.putExtra("date",date);
                startActivity(intent);
            }
        });
    }
    private void cancelNotification(int notificationId) {
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancel(notificationId);

    }
}
