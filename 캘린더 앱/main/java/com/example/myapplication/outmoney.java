package com.example.myapplication;

import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class outmoney extends AppCompatActivity {
    DBHelper helper;
    SQLiteDatabase db;
    EditText moneytext;
    Button moneybutton;
    TextView history;
    String date;
    String mh;
    int money;
    int m;
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.outmoney);
        mh="";
        Intent intent=getIntent();
        date=intent.getStringExtra("date");
        moneytext=(EditText) findViewById(R.id.edittext1);
        moneybutton=(Button) findViewById(R.id.moneybutton);
        history=(TextView)findViewById(R.id.history);
        helper=new DBHelper(this);
        try{
            db=helper.getWritableDatabase();
        }catch(SQLiteException ex){
            db=helper.getReadableDatabase();
        }
        Cursor cursor1;
        cursor1=db.rawQuery("SELECT MONEY FROM outlay WHERE DATE='"+date+"';",null);
        while (cursor1.moveToNext()){
            money=money+Integer.parseInt(cursor1.getString(0));
            mh=mh+date+cursor1.getString(0)+"\n";
        }
        mh=mh+"총 지출 "+money+"원";
        history.setText(mh);
        moneybutton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                money=Integer.parseInt(moneytext.getText().toString());

                db.execSQL("INSERT INTO outlay VALUES (null,'"+date+"','"+moneytext.getText().toString()+"');");


                Intent intent1=new Intent(outmoney.this,showall.class);
                intent1.putExtra("date",date);
                startActivity(intent1);
            }
        });

    }
}
