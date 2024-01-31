package com.do_an.final_project_app;


import static com.amazonaws.mobile.auth.core.internal.util.ThreadUtils.runOnUiThread;

import android.content.Context;
import android.util.Log;

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

import java.io.UnsupportedEncodingException;
import java.util.UUID;

import androidx.lifecycle.ViewModel;

public class DataViewModel extends ViewModel{
    Data d;
    public static final Regions MY_REGION = Regions.AP_SOUTHEAST_2;
    AWSIotMqttManager mqttManager;

    CognitoCachingCredentialsProvider credentialsProvider;
    AWSIotClient mIotAndroidClient;
    String clientId;

    private String topicSub = "esp32";
    private String topicPub = "mobile";

    private static final int KEEP_ALIVE_TIME = 10;

    public void initialTheAWSCognitoCredentials(Context context) {
        credentialsProvider =
                new CognitoCachingCredentialsProvider(
                        context, // context
                        "ap-southeast-2:714c780b-9798-4543-b923-4b4b6ab8924c",
                        MY_REGION);
    }

    public void initialMOTTClient() {
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
                            policyAttachRequest.setPolicyName("air_monitoring_mobile");
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

    public void connect() {
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
    public void publish(String msg, String topic) {
        try {
            mqttManager.publishString(msg, topic, AWSIotMqttQos.QOS0);
        } catch (Exception e) {
            Log.e("AWS_publish", "Publish error.", e);
        }
    }

    public void subscribe(String topic) {
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
                                                    JSONObject dev0 = root.getJSONObject("dev0");
                                                    double temp0 = dev0.getDouble("temperature");
                                                    double humid0 = dev0.getDouble("humidity");
                                                    double press0 = dev0.getDouble("pressure");
                                                    int co20 = dev0.getInt("eCO2");
                                                    int tvoc0 = dev0.getInt("TVOC");
                                                    setData(0, temp0,humid0, press0, co20, tvoc0);
                                                    getData();
//                                                    Log.d("AWS_subscribe", "temp0" + String.valueOf(getData().temp));
//                                                    Log.d("AWS_subscribe", "humid0" + String.valueOf(getData().humid));
//                                                    Log.d("AWS_subscribe", "press0" + String.valueOf(press0));
//                                                    Log.d("AWS_subscribe", "co20" + String.valueOf(co20));
//                                                    Log.d("AWS_subscribe", "tvoc0" + String.valueOf(tvoc0));
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
    public void setData(int devId, double temp, double humid, double press, int co2, int tvoc){
        d = new Data(devId, temp, humid, press, co2, tvoc);
//        Log.d("AWS_debug", "devId" + String.valueOf(d.devId));
//        Log.d("AWS_debug", "temp0" + String.valueOf(d.temp));
//        Log.d("AWS_debug", "humid0" + String.valueOf(d.humid));
//        Log.d("AWS_debug", "press0" + String.valueOf(d.press));
//        Log.d("AWS_debug", "co20" + String.valueOf(d.co2));
//        Log.d("AWS_debug", "tvoc0" + String.valueOf(d.tvoc));
    }

    public Data getData(){
        return this.d;
    }
}
