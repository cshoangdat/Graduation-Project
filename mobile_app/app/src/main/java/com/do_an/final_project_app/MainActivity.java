package com.do_an.final_project_app;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;

import android.app.Activity;
import android.content.Intent;
import android.media.Image;
import android.os.Bundle;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import  static com.amazonaws.mobile.auth.core.internal.util.ThreadUtils.runOnUiThread;

import androidx.appcompat.app.AppCompatActivity;
//import androidx.lifecycle.ViewModelProvider;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.amazonaws.auth.CognitoCachingCredentialsProvider;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttClientStatusCallback;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttLastWillAndTestament;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttManager;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttNewMessageCallback;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttQos;
import com.amazonaws.regions.Region;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.iot.AWSIotClient;
import com.amazonaws.services.iot.model.AttachPolicyRequest;
import com.amazonaws.services.iot.model.CreateKeysAndCertificateRequest;
import com.amazonaws.services.iot.model.CreateKeysAndCertificateResult;
import com.ekn.gruzer.gaugelibrary.ArcGauge;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import com.google.android.material.navigation.NavigationView;

public class MainActivity extends AppCompatActivity{
    AmplifyCognito amplifyCognito;
    DrawerLayout drawerLayout;
    ImageView menu;
    LinearLayout home, settings, control, about, logout, camera, storage;

    DataViewModel viewModel;
    TextView temp_txt, humid_txt, aqi_txt, co2_txt, tvoc_txt, typeDisplay;
    ArcGauge gauge;
    ImageView temp_btn, humid_btn, aqi_btn, co2_btn, tvoc_btn;
    com.ekn.gruzer.gaugelibrary.Range range_green, range_yellow, range_red, range_blue;
    LineChart chart;
    double temp, humid;
    int co2, tvoc, aqi;
    Boolean isTemp = false, isHumid = false, isAqi = false, isCo2 = false, isTvoc = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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

        temp_txt = findViewById(R.id.Temp);
        humid_txt = findViewById(R.id.Humid);
        aqi_txt = findViewById(R.id.aqi);
        co2_txt = findViewById(R.id.Co2);
        tvoc_txt = findViewById(R.id.TVOC);
        gauge = findViewById(R.id.arcGauge);
        temp_btn = findViewById(R.id.temp_btn);
        humid_btn = findViewById(R.id.humid_btn);
        aqi_btn = findViewById(R.id.aqi_btn);
        co2_btn = findViewById(R.id.co2_btn);
        tvoc_btn = findViewById(R.id.tvoc_btn);
        chart = findViewById(R.id.arcChart);
        typeDisplay = findViewById(R.id.typeDisplay);

        typeDisplay.setText("TEMPERATURE");

        range_green = new com.ekn.gruzer.gaugelibrary.Range();
        range_yellow = new com.ekn.gruzer.gaugelibrary.Range();
        range_red = new com.ekn.gruzer.gaugelibrary.Range();
        range_blue = new com.ekn.gruzer.gaugelibrary.Range();

        range_blue.setColor(Color.CYAN);
        range_green.setColor(Color.GREEN);
        range_yellow.setColor(Color.YELLOW);
        range_red.setColor(Color.RED);

        getData();

        range_blue.setFrom(0); range_blue.setTo(15);
        range_green.setFrom(15); range_green.setTo(25);
        range_yellow.setFrom(25); range_yellow.setTo(35);
        range_red.setFrom(35); range_red.setTo(45);

        gauge.setMinValue(0);
        gauge.setMaxValue(45);
        gauge.setValue(0);
        gauge.addRange(range_blue);
        gauge.addRange(range_red);
        gauge.addRange(range_yellow);
        gauge.addRange(range_green);

        List<Entry> entries = new ArrayList<>();
        LineDataSet lineDataSet = new LineDataSet(entries, "Label");
        lineDataSet.setColor(Color.RED);
        lineDataSet.setLineWidth(2f);
        lineDataSet.setDrawCircles(false);
        lineDataSet.setValueTextSize(0f);

        LineData lineData = new LineData(lineDataSet);

        chart.setData(lineData);
        chart.getDescription().setEnabled(false);
        chart.setTouchEnabled(true);
        chart.setDragEnabled(true);
        chart.setScaleEnabled(true);
        chart.setPinchZoom(false);
        chart.getLegend().setEnabled(false);

        XAxis xAxis = chart.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
        xAxis.setTextSize(12f);
        xAxis.setTextColor(Color.BLACK);
        xAxis.setDrawAxisLine(true);
        xAxis.setDrawGridLines(false);

        YAxis yAxisLeft = chart.getAxisLeft();
        yAxisLeft.setTextSize(12f);
        yAxisLeft.setTextColor(Color.BLACK);
        yAxisLeft.setDrawAxisLine(true);
        yAxisLeft.setDrawGridLines(true);
        yAxisLeft.setGridColor(Color.LTGRAY);

        YAxis yAxisRight = chart.getAxisRight();
        yAxisRight.setEnabled(false);

        new Thread(new Runnable() {
            @Override
            public void run() {
                isTemp = true;
                isHumid = false;
                isAqi = false;
                isCo2 = false;
                isTvoc = false;
                long startTime = System.currentTimeMillis();
                while (isTemp) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            long currentTime = System.currentTimeMillis();
                            long elapsedTime = currentTime - startTime;

                            gauge.setValue((double) Math.round(temp * 10000) / 10000);

                            Entry newEntry = new Entry(elapsedTime, (float)temp);

                            // Thêm entry mới vào đối tượng LineData
                            lineData.addEntry(newEntry, 0);

                            // Cập nhật biểu đồ
                            chart.notifyDataSetChanged();
                            chart.invalidate();
                        }
                    });

                    try {
                        Thread.sleep(1000); // Độ trễ 1 giây
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();

        temp_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                typeDisplay.setText("TEMPERATURE");
                isTemp = true;
                isHumid = false;
                isAqi = false;
                isCo2 = false;
                isTvoc = false;

                range_blue.setColor(Color.CYAN);
                range_green.setColor(Color.GREEN);
                range_yellow.setColor(Color.YELLOW);
                range_red.setColor(Color.RED);

                range_blue.setFrom(0); range_blue.setTo(15);
                range_green.setFrom(16); range_green.setTo(25);
                range_yellow.setFrom(26); range_yellow.setTo(35);
                range_red.setFrom(36); range_red.setTo(45);

                gauge.setMinValue(0);
                gauge.setMaxValue(45);
                gauge.addRange(range_blue);
                gauge.addRange(range_red);
                gauge.addRange(range_yellow);
                gauge.addRange(range_green);

                lineData.clearValues();

                List<Entry> entries = new ArrayList<>();
                LineDataSet lineDataSet = new LineDataSet(entries, "Label");
                lineDataSet.setColor(Color.RED);
                lineDataSet.setLineWidth(2f);
                lineDataSet.setDrawCircles(false);
                lineDataSet.setValueTextSize(0f);

                LineData lineData = new LineData(lineDataSet);

                chart.setData(lineData);
                chart.getDescription().setEnabled(false);
                chart.setTouchEnabled(true);
                chart.setDragEnabled(true);
                chart.setScaleEnabled(true);
                chart.setPinchZoom(false);
                chart.getLegend().setEnabled(false);

                XAxis xAxis = chart.getXAxis();
                xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
                xAxis.setTextSize(12f);
                xAxis.setTextColor(Color.BLACK);
                xAxis.setDrawAxisLine(true);
                xAxis.setDrawGridLines(false);

                YAxis yAxisLeft = chart.getAxisLeft();
                yAxisLeft.setTextSize(12f);
                yAxisLeft.setTextColor(Color.BLACK);
                yAxisLeft.setDrawAxisLine(true);
                yAxisLeft.setDrawGridLines(true);
                yAxisLeft.setGridColor(Color.LTGRAY);

                YAxis yAxisRight = chart.getAxisRight();
                yAxisRight.setEnabled(false);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        isTemp = true;
                        isHumid = false;
                        isAqi = false;
                        isCo2 = false;
                        isTvoc = false;
                        long startTime = System.currentTimeMillis();
                        while (isTemp) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    long currentTime = System.currentTimeMillis();
                                    long elapsedTime = currentTime - startTime;

                                    gauge.setValue((double) Math.round(temp * 10000) / 10000);

                                    Entry newEntry = new Entry(elapsedTime, (float)temp);

                                    // Thêm entry mới vào đối tượng LineData
                                    lineData.addEntry(newEntry, 0);

                                    // Cập nhật biểu đồ
                                    chart.notifyDataSetChanged();
                                    chart.invalidate();
                                }
                            });

                            try {
                                Thread.sleep(1000); // Độ trễ 1 giây
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
            }
        });

        humid_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                typeDisplay.setText("HUMIDITY");

                isTemp = false;
                isHumid = true;
                isAqi = false;
                isCo2 = false;
                isTvoc = false;

                range_blue.setColor(Color.CYAN);
                range_green.setColor(Color.GREEN);
                range_yellow.setColor(Color.YELLOW);
                range_red.setColor(Color.RED);

                range_blue.setFrom(0); range_blue.setTo(25);
                range_green.setFrom(26); range_green.setTo(50);
                range_yellow.setFrom(51); range_yellow.setTo(75);
                range_red.setFrom(76); range_red.setTo(100);

                gauge.setMinValue(0);
                gauge.setMaxValue(100);
                gauge.addRange(range_blue);
                gauge.addRange(range_red);
                gauge.addRange(range_yellow);
                gauge.addRange(range_green);

                lineData.clearValues();

                List<Entry> entries = new ArrayList<>();
                LineDataSet lineDataSet = new LineDataSet(entries, "Label");
                lineDataSet.setColor(Color.RED);
                lineDataSet.setLineWidth(2f);
                lineDataSet.setDrawCircles(false);
                lineDataSet.setValueTextSize(0f);

                LineData lineData = new LineData(lineDataSet);

                chart.setData(lineData);
                chart.getDescription().setEnabled(false);
                chart.setTouchEnabled(true);
                chart.setDragEnabled(true);
                chart.setScaleEnabled(true);
                chart.setPinchZoom(false);
                chart.getLegend().setEnabled(false);

                XAxis xAxis = chart.getXAxis();
                xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
                xAxis.setTextSize(12f);
                xAxis.setTextColor(Color.BLACK);
                xAxis.setDrawAxisLine(true);
                xAxis.setDrawGridLines(false);

                YAxis yAxisLeft = chart.getAxisLeft();
                yAxisLeft.setTextSize(12f);
                yAxisLeft.setTextColor(Color.BLACK);
                yAxisLeft.setDrawAxisLine(true);
                yAxisLeft.setDrawGridLines(true);
                yAxisLeft.setGridColor(Color.LTGRAY);

                YAxis yAxisRight = chart.getAxisRight();
                yAxisRight.setEnabled(false);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        isTemp = false;
                        isHumid = true;
                        isAqi = false;
                        isCo2 = false;
                        isTvoc = false;
                        long startTime = System.currentTimeMillis();
                        while (isHumid) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    long currentTime = System.currentTimeMillis();
                                    long elapsedTime = currentTime - startTime;

                                    gauge.setValue((double) Math.round(humid * 10000) / 10000);

                                    Entry newEntry = new Entry(elapsedTime, (float)humid);

                                    // Thêm entry mới vào đối tượng LineData
                                    lineData.addEntry(newEntry, 0);

                                    // Cập nhật biểu đồ
                                    chart.notifyDataSetChanged();
                                    chart.invalidate();
                                }
                            });

                            try {
                                Thread.sleep(1000); // Độ trễ 1 giây
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
            }
        });

        aqi_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                typeDisplay.setText("AQI");
                isTemp = false;
                isHumid = false;
                isAqi = true;
                isCo2 = false;
                isTvoc = false;

                range_blue.setColor(Color.CYAN);
                range_green.setColor(Color.GREEN);
                range_yellow.setColor(Color.YELLOW);
                range_red.setColor(Color.RED);

                range_blue.setFrom(1); range_blue.setTo(1);
                range_green.setFrom(2); range_green.setTo(3);
                range_red.setFrom(4); range_yellow.setTo(5);

                gauge.setMinValue(1);
                gauge.setMaxValue(5);
                gauge.addRange(range_blue);
                gauge.addRange(range_red);
                gauge.addRange(range_green);

                lineData.clearValues();

                List<Entry> entries = new ArrayList<>();
                LineDataSet lineDataSet = new LineDataSet(entries, "Label");
                lineDataSet.setColor(Color.RED);
                lineDataSet.setLineWidth(2f);
                lineDataSet.setDrawCircles(false);
                lineDataSet.setValueTextSize(0f);

                LineData lineData = new LineData(lineDataSet);

                chart.setData(lineData);
                chart.getDescription().setEnabled(false);
                chart.setTouchEnabled(true);
                chart.setDragEnabled(true);
                chart.setScaleEnabled(true);
                chart.setPinchZoom(false);
                chart.getLegend().setEnabled(false);

                XAxis xAxis = chart.getXAxis();
                xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
                xAxis.setTextSize(12f);
                xAxis.setTextColor(Color.BLACK);
                xAxis.setDrawAxisLine(true);
                xAxis.setDrawGridLines(false);

                YAxis yAxisLeft = chart.getAxisLeft();
                yAxisLeft.setTextSize(12f);
                yAxisLeft.setTextColor(Color.BLACK);
                yAxisLeft.setDrawAxisLine(true);
                yAxisLeft.setDrawGridLines(true);
                yAxisLeft.setGridColor(Color.LTGRAY);

                YAxis yAxisRight = chart.getAxisRight();
                yAxisRight.setEnabled(false);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        isTemp = false;
                        isHumid = false;
                        isAqi = true;
                        isCo2 = false;
                        isTvoc = false;
                        long startTime = System.currentTimeMillis();
                        while (isAqi) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    long currentTime = System.currentTimeMillis();
                                    long elapsedTime = currentTime - startTime;

                                    gauge.setValue((double) Math.round(aqi * 10000) / 10000);

                                    Entry newEntry = new Entry(elapsedTime, (float)aqi);

                                    // Thêm entry mới vào đối tượng LineData
                                    lineData.addEntry(newEntry, 0);

                                    // Cập nhật biểu đồ
                                    chart.notifyDataSetChanged();
                                    chart.invalidate();
                                }
                            });

                            try {
                                Thread.sleep(1000); // Độ trễ 1 giây
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
            }
        });

        co2_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                typeDisplay.setText("eCO2");
                isTemp = false;
                isHumid = false;
                isAqi = false;
                isCo2 = true;
                isTvoc = false;

                range_blue.setColor(Color.CYAN);
                range_green.setColor(Color.GREEN);
                range_yellow.setColor(Color.YELLOW);
                range_red.setColor(Color.RED);

                range_blue.setFrom(250); range_blue.setTo(300);
                range_green.setFrom(300); range_green.setTo(400);
                range_yellow.setFrom(400); range_yellow.setTo(500);
                range_red.setFrom(500); range_red.setTo(600);

                gauge.setMinValue(250);
                gauge.setMaxValue(600);
                gauge.addRange(range_blue);
                gauge.addRange(range_red);
                gauge.addRange(range_yellow);
                gauge.addRange(range_green);

                lineData.clearValues();

                List<Entry> entries = new ArrayList<>();
                LineDataSet lineDataSet = new LineDataSet(entries, "Label");
                lineDataSet.setColor(Color.RED);
                lineDataSet.setLineWidth(2f);
                lineDataSet.setDrawCircles(false);
                lineDataSet.setValueTextSize(0f);

                LineData lineData = new LineData(lineDataSet);

                chart.setData(lineData);
                chart.getDescription().setEnabled(false);
                chart.setTouchEnabled(true);
                chart.setDragEnabled(true);
                chart.setScaleEnabled(true);
                chart.setPinchZoom(false);
                chart.getLegend().setEnabled(false);

                XAxis xAxis = chart.getXAxis();
                xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
                xAxis.setTextSize(12f);
                xAxis.setTextColor(Color.BLACK);
                xAxis.setDrawAxisLine(true);
                xAxis.setDrawGridLines(false);

                YAxis yAxisLeft = chart.getAxisLeft();
                yAxisLeft.setTextSize(12f);
                yAxisLeft.setTextColor(Color.BLACK);
                yAxisLeft.setDrawAxisLine(true);
                yAxisLeft.setDrawGridLines(true);
                yAxisLeft.setGridColor(Color.LTGRAY);

                YAxis yAxisRight = chart.getAxisRight();
                yAxisRight.setEnabled(false);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        isTemp = false;
                        isHumid = false;
                        isAqi = false;
                        isCo2 = true;
                        isTvoc = false;
                        long startTime = System.currentTimeMillis();
                        while (isCo2) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    long currentTime = System.currentTimeMillis();
                                    long elapsedTime = currentTime - startTime;

                                    gauge.setValue(co2);

                                    Entry newEntry = new Entry(elapsedTime, co2);

                                    // Thêm entry mới vào đối tượng LineData
                                    lineData.addEntry(newEntry, 0);

                                    // Cập nhật biểu đồ
                                    chart.notifyDataSetChanged();
                                    chart.invalidate();
                                }
                            });

                            try {
                                Thread.sleep(1000); // Độ trễ 1 giây
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
            }
        });

        tvoc_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                typeDisplay.setText("TVOC");
                isTemp = false;
                isHumid = false;
                isAqi = false;
                isCo2 = false;
                isTvoc = true;

                range_blue.setColor(Color.CYAN);
                range_green.setColor(Color.GREEN);
                range_yellow.setColor(Color.YELLOW);
                range_red.setColor(Color.RED);

                range_blue.setFrom(0); range_blue.setTo(200);
                range_green.setFrom(200); range_green.setTo(500);
                range_yellow.setFrom(500); range_yellow.setTo(1000);
                range_red.setFrom(1000); range_red.setTo(1200);

                gauge.setMinValue(0);
                gauge.setMaxValue(1200);
                gauge.addRange(range_blue);
                gauge.addRange(range_red);
                gauge.addRange(range_yellow);
                gauge.addRange(range_green);

                lineData.clearValues();

                List<Entry> entries = new ArrayList<>();
                LineDataSet lineDataSet = new LineDataSet(entries, "Label");
                lineDataSet.setColor(Color.RED);
                lineDataSet.setLineWidth(2f);
                lineDataSet.setDrawCircles(false);
                lineDataSet.setValueTextSize(0f);

                LineData lineData = new LineData(lineDataSet);

                chart.setData(lineData);
                chart.getDescription().setEnabled(false);
                chart.setTouchEnabled(true);
                chart.setDragEnabled(true);
                chart.setScaleEnabled(true);
                chart.setPinchZoom(false);
                chart.getLegend().setEnabled(false);

                XAxis xAxis = chart.getXAxis();
                xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
                xAxis.setTextSize(12f);
                xAxis.setTextColor(Color.BLACK);
                xAxis.setDrawAxisLine(true);
                xAxis.setDrawGridLines(false);

                YAxis yAxisLeft = chart.getAxisLeft();
                yAxisLeft.setTextSize(12f);
                yAxisLeft.setTextColor(Color.BLACK);
                yAxisLeft.setDrawAxisLine(true);
                yAxisLeft.setDrawGridLines(true);
                yAxisLeft.setGridColor(Color.LTGRAY);

                YAxis yAxisRight = chart.getAxisRight();
                yAxisRight.setEnabled(false);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        isTemp = false;
                        isHumid = false;
                        isAqi = false;
                        isCo2 = false;
                        isTvoc = true;
                        long startTime = System.currentTimeMillis();
                        while (isTvoc) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    long currentTime = System.currentTimeMillis();
                                    long elapsedTime = currentTime - startTime;

                                    gauge.setValue(tvoc);

                                    Entry newEntry = new Entry(elapsedTime, tvoc);

                                    // Thêm entry mới vào đối tượng LineData
                                    lineData.addEntry(newEntry, 0);

                                    // Cập nhật biểu đồ
                                    chart.notifyDataSetChanged();
                                    chart.invalidate();
                                }
                            });

                            try {
                                Thread.sleep(1000); // Độ trễ 1 giây
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
            }
        });

        menu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openDrawer(drawerLayout);
            }
        });

        home.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                recreate();
            }
        });

        camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(MainActivity.this, CameraActivity.class);
            }
        });

        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(MainActivity.this, SettingActivity.class);
            }
        });

        control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(MainActivity.this, ControlActivity.class);
            }
        });

        about.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(MainActivity.this, AboutActivity.class);
            }
        });

        storage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(MainActivity.this, StorageActivity.class);
            }
        });

        logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(MainActivity.this, "LogOut", Toast.LENGTH_SHORT).show();
                amplifyCognito.signOut();
                amplifyCognito.loadLogin();
            }
        });
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

    //AWS

    public static final Regions MY_REGION = Regions.AP_SOUTHEAST_2;
    AWSIotMqttManager mqttManager;

    CognitoCachingCredentialsProvider credentialsProvider;
    AWSIotClient mIotAndroidClient;
    String clientId;

    private String topicEsp32 = "esp32";
    private String topicPub = "mobile";

    private static final int KEEP_ALIVE_TIME = 10;

    private void initialTheAWSCognitoCredentials(Context context) {
        credentialsProvider =
                new CognitoCachingCredentialsProvider(
                        context, // context
                        "ap-southeast-2:a45d7a17-2614-46ba-a95f-243d6951192d",
                        MY_REGION);
    }

    private void initialMOTTClient() {
        clientId = UUID.randomUUID().toString();
        Region region = Region.getRegion(MY_REGION);
        mqttManager = new AWSIotMqttManager(clientId, "a3suuuxay09k3c-ats.iot.ap-southeast-2.amazonaws.com");

        // Set keep a live to 10 seconds.  Will recognize disconnects more quickly but will also
        // send
        // MQTT pings every 10 seconds.
        mqttManager.setKeepAlive(KEEP_ALIVE_TIME);

        // AWS IoT will publish this message to alert other clients.
        AWSIotMqttLastWillAndTestament lwt =
                new AWSIotMqttLastWillAndTestament(
                        "try", "Android client lost connection", AWSIotMqttQos.QOS0);
        mqttManager.setMqttLastWillAndTestament(lwt);
        mIotAndroidClient = new AWSIotClient(credentialsProvider);
        mIotAndroidClient.setRegion(region);
        new Thread(
                new Runnable() {
                    @Override
                    public void run() {
                        try {
                            CreateKeysAndCertificateRequest
                                    createKeysAndCertificateRequest =
                                    new CreateKeysAndCertificateRequest();
                            createKeysAndCertificateRequest.setSetAsActive(true);
                            final CreateKeysAndCertificateResult
                                    createKeysAndCertificateResult;
                            AttachPolicyRequest policyAttachRequest =
                                    new AttachPolicyRequest();
                            policyAttachRequest.setPolicyName("controlSystem");
                            runOnUiThread(
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                        }
                                    });
                        } catch (Exception e) {
                            Log.e(
                                    "AWS",
                                    "Exception occurred when generating new private key and certificate.",
                                    e);
                        }
                    }
                })
                .start();

        connect();
    }

    private void connect() {
        Log.d("AWS_connect", "clientId = " + clientId);
        try {
            mqttManager.connect(
                    credentialsProvider,
                    new AWSIotMqttClientStatusCallback() {
                        @Override
                        public void onStatusChanged(
                                final AWSIotMqttClientStatus status, final Throwable throwable) {
                            Log.d("AWS_connect", "Status = " + String.valueOf(status));

                            runOnUiThread(
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                            if (status == AWSIotMqttClientStatus.Connecting) {
                                                Log.d("AWS_connect", "Status: " + "Connecting...");
                                            } else if (status == AWSIotMqttClientStatus.Connected) {
                                                Log.d("AWS_connect", "Status: " + "Connected");
                                                subscribe("esp32");
                                            } else if (status
                                                    == AWSIotMqttClientStatus.Reconnecting) {
                                                if (throwable != null) {
                                                    Log.e("AWS_connect", "Connection error.", throwable);
                                                }
                                                Log.d("AWS_connect", "Status: " + "Reconnecting");
                                            } else if (status
                                                    == AWSIotMqttClientStatus.ConnectionLost) {
                                                if (throwable != null) {
                                                    Log.e("AWS_connect", "Connection error.", throwable);
                                                }
                                                Log.d("AWS_connect", "Status: " + "Disconnected");
                                            } else {
                                                Log.d("AWS_connect", "Status: " + "Disconnected");
                                            }
                                        }
                                    });
                        }
                    });
        } catch (final Exception e) {
            Log.e("AWS_connect", "Connection error.", e);
        }
    }
    private void publish(String msg, String topic) {
        try {
            mqttManager.publishString(msg, topic, AWSIotMqttQos.QOS0);
        } catch (Exception e) {
            Log.e("AWS_publish", "Publish error.", e);
        }
    }

    private void subscribe(String topic) {
        Log.d("AWS_subscribe", "topic = " + topic);
        try {
            mqttManager.subscribeToTopic(
                    topic,
                    AWSIotMqttQos.QOS0,
                    new AWSIotMqttNewMessageCallback() {
                        @Override
                        public void onMessageArrived(final String topic, final byte[] data) {
                            runOnUiThread(
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                            try {
                                                String message = new String(data, "UTF-8");
                                                Log.d("AWS_subscribe", "Message arrived:");
                                                Log.d("AWS_subscribe", "   Topic: " + topic);
                                                Log.e("AWS_Message", " Message: " + message);
                                                try {
                                                    JSONObject root = new JSONObject(message);
                                                    JSONObject dev0 = root.getJSONObject("sensor");
                                                    temp = dev0.getDouble("temperature");
                                                    humid = dev0.getDouble("humidity");
                                                    aqi = dev0.getInt("aqi");
                                                    co2 = dev0.getInt("eCO2");
                                                    tvoc = dev0.getInt("TVOC");

                                                    temp_txt.setText(String.valueOf((double)Math.round(temp*10000)/10000) + " °C");
                                                    humid_txt.setText(String.valueOf((double)Math.round(humid*10000)/10000) + " %");
                                                    aqi_txt.setText(String.valueOf((double)Math.round(aqi*10000)/10000) + " Pa");
                                                    co2_txt.setText(String.valueOf(co2) + " ppm");
                                                    tvoc_txt.setText(String.valueOf(tvoc) + " ppb");

                                                } catch (JSONException e) {
                                                    // subscribe(topicPub);
                                                    e.printStackTrace();
                                                }
                                            } catch (UnsupportedEncodingException e) {
                                                Log.e("AWS", "Message encoding error.", e);
                                            }
                                        }
                                    });
                        }
                    });
        } catch (Exception e) {
            Log.e("AWS", "Subscription error.", e);
        }
    }

    public void getData(){
        initialTheAWSCognitoCredentials(this);
        initialMOTTClient();
//        connect();
        subscribe(topicEsp32);
    }
}