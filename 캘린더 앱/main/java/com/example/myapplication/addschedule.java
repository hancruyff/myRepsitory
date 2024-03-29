package com.example.myapplication;

import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TimePicker;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class addschedule extends AppCompatActivity {
    DBHelper helper;
    SQLiteDatabase db;
    String date;

    int year;
    int month;
    int day;
    int DATE;
    int s_hour;
    int s_minute;
    int e_hour;
    int e_minute;

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.addschedule);
        TimePicker startTime=findViewById(R.id.start_timepicker);
        TimePicker endTime=findViewById(R.id.end_timepicker);
        EditText title=findViewById(R.id.titletext);
        EditText todo=findViewById(R.id.todotext);
        Button button=findViewById(R.id.addbutton);
        Intent intent=getIntent();
        date= intent.getStringExtra("date");
        button.setText(date+"에 일정 추가");
        /*Log.v("test","1");
        String[] str=date.split("-");
        Log.v("test","2");
        button.setText(date+"에 일정 추가");
        year=Integer.parseInt(str[0]);
        Log.v("test","3");
        month=Integer.parseInt(str[1]);
        Log.v("test","4");
        day=Integer.parseInt(str[2]);
        Log.v("test","5");*/
        helper=new DBHelper(this);
        try{
            db=helper.getWritableDatabase();
        }catch(SQLiteException ex){
            db=helper.getReadableDatabase();
        }
        button.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                String title1=title.getText().toString();
                String todo1=todo.getText().toString();
                s_hour=startTime.getHour();
                s_minute=startTime.getMinute();
                e_hour=endTime.getHour();
                e_minute=endTime.getMinute();
                db.execSQL("INSERT INTO schedule VALUES (null,'"+date+"','"+s_hour+"','"+s_minute+"','"+e_hour+"','"+e_minute+"','"+title1+"','"+todo1+"');");
                Toast.makeText(getApplicationContext(),"일정추가성공",Toast.LENGTH_SHORT).show();
                Intent intent=new Intent(addschedule.this,MainActivity.class);
                intent.putExtra("date",date);
                intent.putExtra("status",1);
                startActivity(intent);
            }
        });
    }
}
