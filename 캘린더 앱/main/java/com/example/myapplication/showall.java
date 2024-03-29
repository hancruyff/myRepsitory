package com.example.myapplication;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

public class showall extends AppCompatActivity {
    ListView list;
    DBHelper helper;
    SQLiteDatabase db;
    String date;
    String date1;
    String Title;
    String TODO;
    int S_hour;
    int S_minute;
    int E_hour;
    int E_minute;
    int id;
    Button addbutton;
    Button back;

    ArrayList<schedule> schedules=new ArrayList<schedule>();
    Button money;
    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.showschedule);
        Intent intent=getIntent();
        date= intent.getStringExtra("date");

        super.onCreate(savedInstanceState);
        setContentView(R.layout.showschedule);

        helper=new DBHelper(this);
        try{
            db=helper.getWritableDatabase();
        }catch(SQLiteException ex){
            db=helper.getReadableDatabase();
        }


        Cursor cursor1;

        cursor1=db.rawQuery("SELECT TITLE,TODO,HOUR,MINUTE,E_HOUR,E_MINUTE,_id FROM schedule WHERE DATE='"+date+"';",null);


        while (cursor1.moveToNext()) {
            // 데이터를 읽어오는 작업

            Title = cursor1.getString(0);
            TODO = cursor1.getString(1);
            S_hour = cursor1.getInt(2);
            S_minute = cursor1.getInt(3);
            E_hour = cursor1.getInt(4);
            E_minute = cursor1.getInt(5);
            id=cursor1.getInt(6);
            schedules.add(new schedule(Title, TODO, S_hour, S_minute, E_hour, E_minute,id,date));

        }


        CustomList adapter=new CustomList(this,schedules);
        list=(ListView)findViewById(R.id.list);
        list.setAdapter(adapter);
        addbutton=(Button)findViewById(R.id.addschedule);
        addbutton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent(showall.this,addschedule.class);
                intent.putExtra("date",date);
                startActivity(intent);
            }
        });
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                Intent intent2 = new Intent(getApplicationContext(), editschedule.class);
                intent2.putExtra("ID", schedules.get(i).getId());
                startActivity(intent2);
            }
        });
        back=(Button)findViewById(R.id.back);
        back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent3=new Intent(showall.this,MainActivity.class);
                startActivity(intent3);
            }
        });
        money=(Button)findViewById(R.id.money);
        money.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent4=new Intent(showall.this,outmoney.class);
                intent4.putExtra("date",date);
                startActivity(intent4);
            }
        });
    }

    public class CustomList extends ArrayAdapter {
        public CustomList(Context context, ArrayList schedules) {
            super(context, 0, schedules);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            // Check if an existing view is being reused, otherwise inflate the view
            if (convertView == null) {
                convertView = LayoutInflater.from(getContext()).inflate(R.layout.showlist, parent, false);

            }

            // Get the data item for this position
            schedule S = (schedule) getItem(position);

            // Lookup view for data population
            TextView title = (TextView) convertView.findViewById(R.id.Ttext);
            TextView todo = (TextView) convertView.findViewById(R.id.TDtext);
            TextView time=(TextView) convertView.findViewById(R.id.timetext);
            // Populate the data into the template view using the data object
            title.setText(S.getTitle());
            todo.setText(S.getTODO());
            time.setText(S.getS_hour()+"시 "+S.S_minute+"분 - "+S.getE_hour()+"시 "+S.E_minute+"분");

            // Return the completed view to render on screen
            return convertView;
        }
    }
}
