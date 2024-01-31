package com.do_an.final_project_app;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.Toast;
import android.widget.ToggleButton;

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
import com.ekn.gruzer.gaugelibrary.FullGauge;
import com.google.android.material.slider.RangeSlider;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.UUID;

public class ControlActivity extends AppCompatActivity {
    AmplifyCognito amplifyCognito;
    DrawerLayout drawerLayout;
    ImageView menu;
    LinearLayout home, settings, control, about, logout, camera, storage;
    Switch mode;
    FullGauge gauge;
    ToggleButton autoAC, coolAC, ledSwitch, fanSwitch, airSwitch, doorSwitch, airConSwitch;
    com.ekn.gruzer.gaugelibrary.Range range_blue;
    RangeSlider slider;

    int temp, isAutoMode, isAutoAC, isCoolAC, isLedOn, isFanOn, isAirOn, isAirConOn, isDoorOpen;
    int getACTemp, getMode, getLedState, getFanState, getDoorState, getAirState, getACState, getACAutoMode, getACCoolMode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

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

        mode = findViewById(R.id.mode);
        gauge = findViewById(R.id.fullGauge);
        autoAC = findViewById(R.id.autoAC);
        coolAC = findViewById(R.id.coolAC);
        ledSwitch = findViewById(R.id.led_switch);
        fanSwitch = findViewById(R.id.fan_switch);
        airSwitch = findViewById(R.id.air_switch);
        doorSwitch = findViewById(R.id.door_switch);
        airConSwitch = findViewById(R.id.airCon_switch);
        slider = findViewById(R.id.temperatureRangeSlider);

        range_blue = new com.ekn.gruzer.gaugelibrary.Range();
        range_blue.setColor(Color.CYAN);

        gauge.setMinValue(16);
        gauge.setMaxValue(30);
        gauge.addRange(range_blue);
        gauge.setValue(16);

        mode.setChecked(true);
        coolAC.setChecked(true);
        autoAC.setChecked(false);
        ledSwitch.setChecked(false);
        fanSwitch.setChecked(false);
        airSwitch.setChecked(false);
        airConSwitch.setChecked(false);
        doorSwitch.setChecked(false);

        isAutoMode = 1;
        isCoolAC = 1;
        isAutoAC = 0;
        isLedOn = 0;
        isFanOn = 0;
        isAirOn = 0;
        isAirConOn = 0;
        isDoorOpen = 0;

        coolAC.setEnabled(false);
        autoAC.setEnabled(false);
        ledSwitch.setEnabled(false);
        fanSwitch.setEnabled(false);
        airSwitch.setEnabled(false);
        airConSwitch.setEnabled(false);
        doorSwitch.setEnabled(false);
        slider.setEnabled(false);

        getData();

        slider.addOnChangeListener(new RangeSlider.OnChangeListener() {
            @Override
            public void onValueChange(@NonNull RangeSlider slider, float value, boolean fromUser) {
                gauge.setValue(value);
                temp = Math.round(value);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        publish(pubData(), topicPub);
                    }
                }, 2000);
            }
        });

        mode.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(mode.isChecked()){
//                    Toast.makeText(ControlActivity.this, "AUTO MODE", Toast.LENGTH_SHORT).show();
                    isAutoMode = 1;
                    disableAll();
                }
                else{
//                    Toast.makeText(ControlActivity.this, "MANUAL MODE", Toast.LENGTH_SHORT).show();
                    isAutoMode = 0;
                    enableAll();
                }
                publish(pubData(), topicPub);
            }
        });

        autoAC.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(autoAC.isChecked()){
                    isAutoAC = 1;
                    isCoolAC = 0;
                    coolAC.setChecked(false);
                }
                else{
                    isAutoAC = 0;
                    isCoolAC = 1;
                    coolAC.setChecked(true);
                }
                publish(pubData(), topicPub);
            }
        });

        ledSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(ledSwitch.isChecked()){
                    isLedOn = 1;
                }
                else{
                    isLedOn = 0;
                }
                publish(pubData(), topicPub);
            }
        });

        fanSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(fanSwitch.isChecked()){
                    isFanOn = 1;
                }
                else{
                    isFanOn = 0;
                }
                publish(pubData(), topicPub);
            }
        });

        airSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(airSwitch.isChecked()){
                    isAirOn = 1;
                }
                else{
                    isAirOn = 0;
                }
                publish(pubData(), topicPub);
            }
        });

        doorSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(doorSwitch.isChecked()){
                    isDoorOpen = 1;
                }
                else{
                    isDoorOpen = 0;
                }
                publish(pubData(), topicPub);
            }
        });

        airConSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(airConSwitch.isChecked()){
                    isAirConOn = 1;
                    slider.setEnabled(true);
                    coolAC.setEnabled(true);
                    autoAC.setEnabled(true);
                }
                else{
                    isAirConOn = 0;
                    slider.setEnabled(false);
                    coolAC.setEnabled(false);
                    autoAC.setEnabled(false);
                }
                publish(pubData(), topicPub);
            }
        });

        coolAC.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(coolAC.isChecked()){
                    isAutoAC = 0;
                    isCoolAC = 1;
                    autoAC.setChecked(false);
                }
                else{
                    isAutoAC = 1;
                    isCoolAC = 0;
                    autoAC.setChecked(true);
                }
                publish(pubData(), topicPub);
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
                redirectActivity(ControlActivity.this, MainActivity.class);
            }
        });

        camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(ControlActivity.this, CameraActivity.class);
            }
        });

        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(ControlActivity.this, SettingActivity.class);
            }
        });

        control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                recreate();
            }
        });

        about.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(ControlActivity.this, AboutActivity.class);
            }
        });

        storage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(ControlActivity.this, StorageActivity.class);
            }
        });

        logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(ControlActivity.this, "Log Out", Toast.LENGTH_SHORT).show();
                amplifyCognito.signOut();
                amplifyCognito.loadLogin();
            }
        });
    }

    private void enableAll(){
        ledSwitch.setEnabled(true);
        fanSwitch.setEnabled(true);
        airSwitch.setEnabled(true);
        airConSwitch.setEnabled(true);
        doorSwitch.setEnabled(true);
        if(airConSwitch.isChecked()){
            slider.setEnabled(true);
            coolAC.setEnabled(true);
            autoAC.setEnabled(true);
        }
    }

    private void disableAll(){
        coolAC.setEnabled(false);
        autoAC.setEnabled(false);
        ledSwitch.setEnabled(false);
        fanSwitch.setEnabled(false);
        airSwitch.setEnabled(false);
        airConSwitch.setEnabled(false);
        doorSwitch.setEnabled(false);
        slider.setEnabled(false);
    }

    private void AWS_getState(){
        if(getMode == 1){
            isAutoMode = 1;
            mode.setChecked(true);
            disableAll();
        }
        else{
            isAutoMode = 0;
            mode.setChecked(false);
            enableAll();
        }

        if(getLedState == 1){
            ledSwitch.setChecked(true);
            isLedOn = 1;
        }
        else{
            ledSwitch.setChecked(false);
            isLedOn = 0;
        }

        if(getFanState == 1){
            fanSwitch.setChecked(true);
            isFanOn = 1;
        }
        else{
            fanSwitch.setChecked(false);
            isFanOn = 0;
        }

        if(getAirState == 1){
            airSwitch.setChecked(true);
            isAirOn = 1;
        }
        else{
            airSwitch.setChecked(false);
            isAirOn = 0;
        }

        if(getDoorState == 1){
            doorSwitch.setChecked(true);
            isDoorOpen = 1;
        }
        else{
            doorSwitch.setChecked(false);
            isDoorOpen = 0;
        }

        if(getACState == 1){
            airConSwitch.setChecked(true);
            isAirConOn = 1;
        }
        else{
            airConSwitch.setChecked(false);
            isAirConOn = 0;
        }

        if(getACTemp != temp && getACTemp != 0){
            slider.setValues((float)getACTemp);
            gauge.setValue((float)getACTemp);
        }

        if(getACAutoMode == 1 || getACCoolMode == 0){
            autoAC.setChecked(true);
            isAutoAC = 1;
            isCoolAC = 0;
            coolAC.setChecked(false);
        }
        else if (getACAutoMode == 0 || getACCoolMode == 1){
            autoAC.setChecked(false);
            isAutoAC = 0;
            isCoolAC = 1;
            coolAC.setChecked(true);
        }
    }

    private String pubData(){
        String data = null;
        try {
            JSONObject mainObject = new JSONObject();
            JSONObject mobileObject = new JSONObject();
            mobileObject.put("mode", isAutoMode);
            mobileObject.put("led", isLedOn);
            mobileObject.put("fan", isFanOn);
            mobileObject.put("door", isDoorOpen);
            mobileObject.put("air", isAirOn);
            mobileObject.put("AC_State", isAirConOn);
            mobileObject.put("AC_Temp", temp);
            mobileObject.put("AC_AutoMode", isAutoAC);
            mobileObject.put("AC_CoolMode", isCoolAC);
            mainObject.put("mobile", mobileObject);
            String jsonString = mainObject.toString();
            data = jsonString;

        } catch (JSONException e) {
            e.printStackTrace();
        }

        return data;
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
                                                subscribe(topicEsp32);
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
                                                    JSONObject dev = root.getJSONObject("device");
                                                    if(!dev.isNull("mode"))
                                                        getMode = dev.getInt("mode");
                                                    else{
                                                        getMode = isAutoMode;
                                                    }
                                                    if(!dev.isNull("led"))
                                                        getLedState = dev.getInt("led");
                                                    else{
                                                        getLedState = isLedOn;
                                                    }
                                                    if(!dev.isNull("fan"))
                                                        getFanState = dev.getInt("fan");
                                                    else{
                                                        getFanState = isFanOn;
                                                    }
                                                    if(!dev.isNull("door"))
                                                        getDoorState = dev.getInt("door");
                                                    else{
                                                        getDoorState = isDoorOpen;
                                                    }
                                                    if(!dev.isNull("air"))
                                                        getAirState = dev.getInt("air");
                                                    else{
                                                        getAirState = isAirOn;
                                                    }
                                                    if(!dev.isNull("AC_State"))
                                                        getACState = dev.getInt("AC_State");
                                                    else{
                                                        getACState = isAirConOn;
                                                    }
                                                    if(!dev.isNull("AC_Temp"))
                                                        getACTemp = dev.getInt("AC_Temp");
                                                    else{
                                                        getACTemp = temp;
                                                    }
                                                    if(!dev.isNull("AC_AutoMode"))
                                                        getACAutoMode = dev.getInt("AC_AutoMode");
                                                    else{
                                                        getACAutoMode = isAutoAC;
                                                    }
                                                    if(!dev.isNull("AC_CoolMode"))
                                                        getACCoolMode = dev.getInt("AC_CoolMode");
                                                    else{
                                                        getACCoolMode = isCoolAC;
                                                    }
                                                    AWS_getState();
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