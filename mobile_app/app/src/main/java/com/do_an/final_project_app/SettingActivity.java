package com.do_an.final_project_app;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

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
import com.google.android.material.button.MaterialButton;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.UUID;

public class SettingActivity extends AppCompatActivity {
    AmplifyCognito amplifyCognito;
    DrawerLayout drawerLayout;
    ImageView menu;
    LinearLayout home, settings, control, about, logout, camera, storage;
    String []items = {"Panasonic"};
    AutoCompleteTextView autoCompleteTxt;
    ArrayAdapter<String> adapterItems;
    EditText camIp, camPass, wifiName, wifiPass, otaIp;
    MaterialButton btnCameraSet, btnWiFiSet, btnUpdate;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

        amplifyCognito = new AmplifyCognito(getApplicationContext());

        drawerLayout = findViewById(R.id.drawer_layout);
        menu = findViewById(R.id.menu);
        home = findViewById(R.id.home);
        settings = findViewById(R.id.setting);
        control = findViewById(R.id.control);
        about = findViewById(R.id.about);
        logout = findViewById(R.id.logout);
        camera = findViewById(R.id.camera);
        storage = findViewById(R.id.storage);

        autoCompleteTxt = findViewById(R.id.autoCompleteTxt);

        camIp = findViewById(R.id.camIp);
        camPass = findViewById(R.id.camPassword);
        wifiName = findViewById(R.id.wifiName);
        wifiPass = findViewById(R.id.wifiPassword);
        btnCameraSet = findViewById(R.id.btnCamSet);
        btnWiFiSet = findViewById(R.id.btnWifiSet);
        btnUpdate = findViewById(R.id.btnUpdate);
        otaIp = findViewById(R.id.otaIp);

        initAWS();

        adapterItems = new ArrayAdapter<String>(this,R.layout.list_item, items);
        autoCompleteTxt.setAdapter(adapterItems);
        autoCompleteTxt.setText("Panasonic");
        autoCompleteTxt.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                String item = adapterView.getItemAtPosition(i).toString();
                Toast.makeText(getApplicationContext(),"AC Model: " + item, Toast.LENGTH_SHORT).show();
            }
        });

        SharedPreferences prefs = getSharedPreferences("cameraURL", MODE_PRIVATE);
        String getCamIp = prefs.getString("camIp", "");
        String getCamPass = prefs.getString("camPass", "");
        if(getCamIp != null && getCamPass != null){
            camIp.setText(getCamIp);
            camPass.setText(getCamPass);
        }

        SharedPreferences wifi_prefs = getSharedPreferences("wifiInfo", MODE_PRIVATE);
        String getWifiName = prefs.getString("wifiName", "");
        String getWifiPass = prefs.getString("wifiPass", "");
        if(getWifiName != null && getWifiPass != null){
            wifiName.setText(getWifiName);
            wifiPass.setText(getWifiPass);
        }

        btnCameraSet.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String ip = camIp.getText().toString();
                String pass = camPass.getText().toString();
                String address = "rtsp://admin:"+pass+"@"+ip+":554/onvif1";
                SharedPreferences.Editor editor = getSharedPreferences("cameraURL", MODE_PRIVATE).edit();
                editor.putString("URl", address);
                editor.putString("camIp", ip);
                editor.putString("camPass", pass);
                editor.apply();
                Toast.makeText(getApplicationContext(),"Camera URL: " + address, Toast.LENGTH_SHORT).show();
            }
        });

        btnWiFiSet.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String name = wifiName.getText().toString();
                String pass = wifiPass.getText().toString();
                publish(pubData(name, pass), topicPub);
                SharedPreferences.Editor editor = getSharedPreferences("wifiInfo", MODE_PRIVATE).edit();
                editor.putString("wifiName", pass);
                editor.putString("wifiPass", pass);
                editor.apply();
                Toast.makeText(getApplicationContext(),"Set WiFi Done", Toast.LENGTH_SHORT).show();
            }
        });

        btnUpdate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    JSONObject mainObject = new JSONObject();
                    JSONObject mobileObject = new JSONObject();
                    mobileObject.put("update", 1);
                    mobileObject.put("otaIp", otaIp.getText().toString());
                    mainObject.put("mobile", mobileObject);
                    String jsonString = mainObject.toString();
                    publish(jsonString, topicPub);
                    Toast.makeText(getApplicationContext(),"Update Firmware...", Toast.LENGTH_SHORT).show();
                } catch (JSONException e) {
                    e.printStackTrace();
                }
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
                redirectActivity(SettingActivity.this, MainActivity.class);
            }
        });

        camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(SettingActivity.this, CameraActivity.class);
            }
        });

        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                recreate();
            }
        });

        control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(SettingActivity.this, ControlActivity.class);
            }
        });

        about.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(SettingActivity.this, AboutActivity.class);
            }
        });

        storage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(SettingActivity.this, StorageActivity.class);
            }
        });

        logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(SettingActivity.this, "LogOut", Toast.LENGTH_SHORT).show();
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

    private String pubData(String name, String pass){
        String data = null;
        try {
            JSONObject mainObject = new JSONObject();
            JSONObject mobileObject = new JSONObject();
            mobileObject.put("setWifi", 1);
            mobileObject.put("wifiName", name);
            mobileObject.put("wifiPass", pass);
            mainObject.put("mobile", mobileObject);
            String jsonString = mainObject.toString();
            data = jsonString;

        } catch (JSONException e) {
            e.printStackTrace();
        }

        return data;
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

    public void initAWS(){
        initialTheAWSCognitoCredentials(this);
        initialMOTTClient();
    }
}