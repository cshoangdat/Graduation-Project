package com.do_an.final_project_app;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
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

import org.json.JSONException;
import org.json.JSONObject;
import org.videolan.libvlc.LibVLC;
import org.videolan.libvlc.Media;

import java.io.UnsupportedEncodingException;
import java.util.UUID;

public class CameraActivity extends AppCompatActivity {

    AmplifyCognito amplifyCognito;
    DrawerLayout drawerLayout;
    ImageView menu;
    LinearLayout home, settings, control, about, logout, camera, storage;
    LibVLC libVLC;
    org.videolan.libvlc.media.VideoView videoView;
    int num, male, female, young, teen, worker, old;
    int F_G1, F_G2, F_G3, F_G4, M_G1, M_G2, M_G3, M_G4;
    String time;
    TextView num_txt, male_txt, female_txt, young_txt, teen_txt, worker_txt, old_txt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

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

        num_txt = findViewById(R.id.num);
        male_txt = findViewById(R.id.male);
        female_txt = findViewById(R.id.female);
        young_txt = findViewById(R.id.young);
        teen_txt = findViewById(R.id.teen);
        worker_txt = findViewById(R.id.worker);
        old_txt = findViewById(R.id.old);

        SharedPreferences prefs = getSharedPreferences("cameraURL", MODE_PRIVATE);
        String url = prefs.getString("URl", "");

        menu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openDrawer(drawerLayout);
            }
        });

        home.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(CameraActivity.this, MainActivity.class);
            }
        });

        control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(CameraActivity.this, ControlActivity.class);
            }
        });

        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(CameraActivity.this, SettingActivity.class);
            }
        });

        camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                recreate();
            }
        });

        about.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(CameraActivity.this, AboutActivity.class);
            }
        });

        storage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                redirectActivity(CameraActivity.this, StorageActivity.class);
            }
        });

        logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(CameraActivity.this, "LogOut", Toast.LENGTH_SHORT).show();
                amplifyCognito.signOut();
                amplifyCognito.loadLogin();
            }
        });

        videoView = findViewById(R.id.videoView);

        libVLC = new LibVLC(this);
        //default rtspUrl: rtsp://admin:dat123456@192.168.1.2:554/onvif1
        if(url == null){
            url = getString(R.string.rtspUrl);
        }
        Media media = new Media(libVLC, Uri.parse(url));
        media.addOption("--aout=opensles");
        media.addOption("--audio-time-stretch");
        media.addOption("-vvv"); // verbosity
        org.videolan.libvlc.MediaPlayer mediaPlayer = new org.videolan.libvlc.MediaPlayer(libVLC);
        mediaPlayer.setMedia(media);
        mediaPlayer.getVLCVout().setVideoSurface(videoView.getHolder().getSurface(), videoView.getHolder());
        mediaPlayer.getVLCVout().setWindowSize(videoView.getWidth(), videoView.getHeight());
        mediaPlayer.getVLCVout().attachViews();
        mediaPlayer.play();

        getData();
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

    private String topicCamera = "camera";
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
                                                subscribe(topicCamera);
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
                                                    JSONObject cam = root.getJSONObject("camera");
                                                    time = cam.getString("timestamp");
                                                    F_G1 = cam.getInt("F_G1");
                                                    F_G2 = cam.getInt("F_G2");
                                                    F_G3 = cam.getInt("F_G3");
                                                    F_G4 = cam.getInt("F_G4");
                                                    M_G1 = cam.getInt("M_G1");
                                                    M_G2 = cam.getInt("M_G2");
                                                    M_G3 = cam.getInt("M_G3");
                                                    M_G4 = cam.getInt("M_G4");

                                                    male = M_G1 + M_G2 + M_G3 + M_G4;
                                                    female = F_G1 + F_G2 + F_G3 + F_G4;
                                                    young = F_G1 + M_G1;
                                                    teen = F_G2 + M_G2;
                                                    worker = F_G3 + M_G3;
                                                    old = F_G4 + M_G4;
                                                    num = M_G1 + M_G2 + M_G3 + M_G4 + F_G1 + F_G2 + F_G3 + F_G4;

                                                    num_txt.setText(String.valueOf(num));
                                                    male_txt.setText(String.valueOf(male));
                                                    female_txt.setText(String.valueOf(female));
                                                    young_txt.setText(String.valueOf(young));
                                                    teen_txt.setText(String.valueOf(teen));
                                                    worker_txt.setText(String.valueOf(worker));
                                                    old_txt.setText(String.valueOf(old));

                                                    MyDatabaseHelper myDb = new MyDatabaseHelper(CameraActivity.this);
                                                    myDb.addData(time, num, male, female, young, teen, worker, old);

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
        subscribe(topicCamera);
    }
}