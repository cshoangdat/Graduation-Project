package com.do_an.final_project_app;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import java.util.ArrayList;

public class StorageActivity extends AppCompatActivity {
    AmplifyCognito amplifyCognito;
    DrawerLayout drawerLayout;
    ImageView menu;
    LinearLayout home, settings, control, about, logout, camera, storage;

    RecyclerView recyclerView;
    MyDatabaseHelper myDB;
    ArrayList<String> id, time, people, male, female, g1, g2, g3, g4;
    CustomAdapter customAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_storage);

        amplifyCognito = new AmplifyCognito(getApplicationContext());

        drawerLayout = findViewById(R.id.drawer_layout);
        menu = findViewById(R.id.menu);
        home = findViewById(R.id.home);
        settings = findViewById(R.id.setting);
        control = findViewById(R.id.control);
        about = findViewById(R.id.about);
        camera = findViewById(R.id.camera);
        logout = findViewById(R.id.logout);
        storage = findViewById(R.id.storage);

        menu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openDrawer(drawerLayout);
            }
        });

        home.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(StorageActivity.this, MainActivity.class);
            }
        });

        camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(StorageActivity.this, CameraActivity.class);
            }
        });

        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(StorageActivity.this, SettingActivity.class);
            }
        });

        control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(StorageActivity.this, ControlActivity.class);
            }
        });

        about.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(StorageActivity.this, AboutActivity.class);
            }
        });

        storage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                recreate();
            }
        });

        logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(StorageActivity.this, "LogOut", Toast.LENGTH_SHORT).show();
                amplifyCognito.signOut();
                amplifyCognito.loadLogin();
            }
        });

        recyclerView = findViewById(R.id.recycleViewer);

        myDB = new MyDatabaseHelper(StorageActivity.this);
        id = new ArrayList<>();
        time = new ArrayList<>();
        people = new ArrayList<>();
        male = new ArrayList<>();
        female = new ArrayList<>();
        g1 = new ArrayList<>();
        g2 = new ArrayList<>();
        g3 = new ArrayList<>();
        g4 = new ArrayList<>();

        storeDataInArrays();
        customAdapter = new CustomAdapter(StorageActivity.this, time, people, male, female, g1, g2, g3, g4);
        recyclerView.setAdapter(customAdapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(StorageActivity.this));
    }

    public static void openDrawer(DrawerLayout drawerLayout){
        drawerLayout.openDrawer(GravityCompat.START);
    }

    public static void closeDrawer(DrawerLayout drawerLayout){
        if(drawerLayout.isDrawerOpen(GravityCompat.START)){
            drawerLayout.closeDrawer(GravityCompat.START);
        }
    }

    public static void redirectActivity(Activity activity, Class secondActivity){
        Intent intent = new Intent(activity, secondActivity);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        activity.startActivity(intent);
        activity.finish();
    }

    @Override
    protected void onPause() {
        super.onPause();
        closeDrawer(drawerLayout);
    }

    void storeDataInArrays(){
        Cursor cursor = myDB.readAllData();
        if(cursor.getCount() == 0){
            Log.d("Storage", "No data storage");
        }
        else{
            while (cursor.moveToNext()){
                id.add(cursor.getString(0));
                time.add(cursor.getString(1));
                people.add(cursor.getString(2));
                male.add(cursor.getString(3));
                female.add(cursor.getString(4));
                g1.add(cursor.getString(5));
                g2.add(cursor.getString(6));
                g3.add(cursor.getString(7));
                g4.add(cursor.getString(8));
            }
        }
    }
}