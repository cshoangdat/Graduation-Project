package com.do_an.final_project_app;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.Nullable;

class MyDatabaseHelper extends SQLiteOpenHelper {

    private Context context;
    private static final String DATABASE_NAME = "MyFinalApp.db";
    private static final int DATABASE_VERSION = 1;

    private static final String TABLE_NAME = "my_control_system";
    private static final String COLUMN_ID = "_id";
    private static final String COLUMN_TIME = "time";
    private static final String COLUMN_PEOPLE = "people";
    private static final String COLUMN_MALE = "male";
    private static final String COLUMN_FEMALE = "female";
    private static final String COLUMN_G1 = "G1";
    private static final String COLUMN_G2 = "G2";
    private static final String COLUMN_G3 = "G3";
    private static final String COLUMN_G4 = "G4";

    public MyDatabaseHelper(@Nullable Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
        this.context = context;
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String query =
                "CREATE TABLE " + TABLE_NAME +
                        " (" + COLUMN_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                        COLUMN_TIME + " TEXT, " +
                        COLUMN_PEOPLE + " INTEGER, " +
                        COLUMN_MALE + " INTEGER, " +
                        COLUMN_FEMALE + " INTEGER, " +
                        COLUMN_G1 + " INTEGER, " +
                        COLUMN_G2 + " INTEGER, " +
                        COLUMN_G3 + " INTEGER, " +
                        COLUMN_G4 + " INTEGER);";
        db.execSQL(query);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int i, int i1) {
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
        onCreate(db);
    }

    public void addData(String time, int people, int male, int female, int g1, int g2, int g3, int g4){
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put(COLUMN_TIME, time);
        cv.put(COLUMN_PEOPLE, people);
        cv.put(COLUMN_MALE, male);
        cv.put(COLUMN_FEMALE, female);
        cv.put(COLUMN_G1, g1);
        cv.put(COLUMN_G2, g2);
        cv.put(COLUMN_G3, g3);
        cv.put(COLUMN_G4, g4);

        long result = db.insert(TABLE_NAME, null, cv);
        if(result == -1){
//            Toast.makeText(context, "Failed", Toast.LENGTH_SHORT).show();
            Log.d("DB", "Status: " + "DB Stored Failed");
        }
        else{
//            Toast.makeText(context, "Added Success", Toast.LENGTH_SHORT).show();
            Log.d("DB", "Status: " + "DB Stored Success");
        }
    }

    Cursor readAllData(){
        String query = "SELECT * FROM " + TABLE_NAME;
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = null;
        if(db != null){
            cursor = db.rawQuery(query, null);
        }
        return cursor;
    }
}
