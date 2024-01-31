#include "aws.h"
#include "esp_log.h"
#include <stdint.h>
#include <stddef.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "mqtt_client.h"
#include "esp_tls.h"
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"

#include "INCLUDE/common.h"
#include "INCLUDE/config.h"
#include "../NVS/NVSDriver.h"

static const char* TAG                     =                 "AWS";
static const char* TOPIC_TRANSMIT          =                 "esp32";
static const char* TOPIC_CAMERA_RECIEVE    =                 "camera";
static const char* TOPIC_MOBILE_RECIEVE    =                 "mobile";
TaskHandle_t AWSTaskSendHandle;
TaskHandle_t AWSTaskRecvHandle;

esp_mqtt_client_handle_t client;
esp_mqtt_event_handle_t event;

char* receiveData;

static portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;

static void MQTT_Handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, TOPIC_TRANSMIT, 0);
        esp_mqtt_client_subscribe(client, TOPIC_CAMERA_RECIEVE, 0);
        esp_mqtt_client_subscribe(client, TOPIC_MOBILE_RECIEVE, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        int len = event->data_len;
        receiveData = event->data;
        receiveData[event->data_len] = '\0';
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
            strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void MQTT_Start(const char* URI)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = URI,
        .port = 8883,
        .cert_pem = (const char*) cert_start,
        .client_cert_pem = (const char*) certificate_start,
        .client_key_pem = (const char*) private_start,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, MQTT_Handler, NULL);
    esp_mqtt_client_start(client);
}

static void MQTT_Stop(void){
    esp_mqtt_client_disconnect(client);
    vTaskDelay(500/portTICK_PERIOD_MS);
    esp_mqtt_client_stop(client);
    vTaskDelay(500/portTICK_PERIOD_MS);
    esp_mqtt_client_destroy(client);
}

static bool isCameraDataUpdate(char* oldCameraData,char* newCameraData){
    static bool result;
    if(strcmp(oldCameraData, newCameraData) == 0) result = 0;
    else result = 1;
    return result;
}

static void AWSTaskSend(void *arg){
    cJSON* pub_data, *sensor, *deviceState, *deviceInfo;
    static char* publicData = NULL;
    while(1){
        {
            pub_data = cJSON_CreateObject();
            cJSON_AddItemToObject(pub_data, "sensor", sensor = cJSON_CreateObject());
            cJSON_AddNumberToObject(sensor, "temperature", SensorData.temperature);
            cJSON_AddNumberToObject(sensor, "humidity", SensorData.humid);
            cJSON_AddNumberToObject(sensor, "aqi", SensorData.aqi);
            cJSON_AddNumberToObject(sensor, "eCO2", SensorData.eCo2);
            cJSON_AddNumberToObject(sensor, "TVOC", SensorData.tvoc);
            cJSON_AddItemToObject(pub_data, "device", deviceState = cJSON_CreateObject());
            if(deviceMode.autoMode != mobileData.isAutoMode)
                cJSON_AddNumberToObject(deviceState, "mode", deviceMode.autoMode);
            if(device.isLedOn != mobileData.isLedOn)
                cJSON_AddNumberToObject(deviceState, "led", device.isLedOn);
            if(device.isFanOn != mobileData.isFanOn)
                cJSON_AddNumberToObject(deviceState, "fan", device.isFanOn);
            if(device.isDoorOn != mobileData.isDoorOn)
                cJSON_AddNumberToObject(deviceState, "door", device.isDoorOn);
            if(device.isAirOn != mobileData.isAirOn)
                cJSON_AddNumberToObject(deviceState, "air", device.isAirOn);
            if(device.isAirConOn != mobileData.isAirConOn)
                cJSON_AddNumberToObject(deviceState, "AC_State", device.isAirConOn);
            if(device.airConSetting.acTemp != mobileData.ACSetting.acTemp)
                cJSON_AddNumberToObject(deviceState, "AC_Temp", device.airConSetting.acTemp);
            if(device.airConSetting.isAutoMode != mobileData.ACSetting.isAutoMode)
                cJSON_AddNumberToObject(deviceState, "AC_AutoMode", device.airConSetting.isAutoMode);
            if(device.airConSetting.isCoolMode != mobileData.ACSetting.isCoolMode)
                cJSON_AddNumberToObject(deviceState, "AC_CoolMode", device.airConSetting.isCoolMode);
            cJSON_AddItemToObject(pub_data, "info", deviceInfo = cJSON_CreateObject());
            cJSON_AddNumberToObject(deviceInfo, "ver_major", FW_VERSION_MAJOR);
            cJSON_AddNumberToObject(deviceInfo, "ver_minor", FW_VERSION_MINOR);
            cJSON_AddNumberToObject(deviceInfo, "ver_build", FW_VERSION_BUILD);            
            publicData = cJSON_Print(pub_data);
            int msg_id = esp_mqtt_client_publish(client, TOPIC_TRANSMIT, publicData, 0, 0, 0);
            ESP_LOGD(TAG,"sent publish Data successful, msg_id=%d", msg_id);
            ESP_LOGI(TAG,"publicData:%s", publicData);
            cJSON_free(publicData);
            cJSON_Delete(pub_data);
        }
        if(OTA.isUpdate == true){
            AWS_Stop();
        }
        vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("remaining memory of task_transmit_to_AWS : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }  
}

static void AWSTaskRecv(void *arg){
    cJSON* subData;
    while(1){
        if(strlen(&receiveData) != 0){
            subData = cJSON_Parse(receiveData);
            if(cJSON_GetObjectItem(subData,"camera")){
                cJSON *camera = cJSON_GetObjectItem(subData,"camera");
                if(cJSON_GetObjectItem(camera, "M_G1")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.M_G1 = cJSON_GetObjectItem(camera, "M_G1")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "M_G2")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.M_G2 = cJSON_GetObjectItem(camera, "M_G2")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "M_G3")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.M_G3 = cJSON_GetObjectItem(camera, "M_G3")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "M_G4")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.M_G4 = cJSON_GetObjectItem(camera, "M_G4")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "F_G1")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.F_G1 = cJSON_GetObjectItem(camera, "F_G1")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "F_G2")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.F_G2 = cJSON_GetObjectItem(camera, "F_G2")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "F_G3")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.F_G3 = cJSON_GetObjectItem(camera, "F_G3")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(camera, "F_G4")){
                    portENTER_CRITICAL(&param_lock);
                    CameraData.F_G4 = cJSON_GetObjectItem(camera, "F_G4")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                CameraData.num = CameraData.F_G1 + CameraData.F_G2 + CameraData.F_G3 + CameraData.F_G4 + CameraData.M_G1 + CameraData.M_G2 + CameraData.M_G3 + CameraData.M_G4;
                CameraData.femaleNum = CameraData.F_G1 + CameraData.F_G2 + CameraData.F_G3 + CameraData.F_G4;
                CameraData.maleNum = CameraData.M_G1 + CameraData.M_G2 + CameraData.M_G3 + CameraData.M_G4;
                CameraData.G1Num = CameraData.F_G1 + CameraData.M_G1;
                CameraData.G2Num = CameraData.F_G2 + CameraData.M_G2;
                CameraData.G3Num = CameraData.F_G3 + CameraData.M_G3;
                CameraData.G4Num = CameraData.F_G4 + CameraData.M_G4;
            }
            if(cJSON_GetObjectItem(subData,"mobile")){
                cJSON *mobile = cJSON_GetObjectItem(subData,"mobile");
                deviceMode.isNextionContact = 0;
                if(cJSON_GetObjectItem(mobile, "mode")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isAutoMode = cJSON_GetObjectItem(mobile, "mode")->valueint;
                    deviceMode.autoMode = mobileData.isAutoMode;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "led")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isLedOn = cJSON_GetObjectItem(mobile, "led")->valueint;
                    device.isLedOn = mobileData.isLedOn;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "fan")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isFanOn = cJSON_GetObjectItem(mobile, "fan")->valueint;
                    device.isFanOn = mobileData.isFanOn;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "air")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isAirOn = cJSON_GetObjectItem(mobile, "air")->valueint;
                    device.isAirOn = mobileData.isAirOn;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "door")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isDoorOn = cJSON_GetObjectItem(mobile, "door")->valueint;
                    device.isDoorOn = mobileData.isDoorOn;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "AC_State")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isAirConOn = cJSON_GetObjectItem(mobile, "AC_State")->valueint;
                    device.isAirConOn = mobileData.isAirConOn;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "AC_Temp")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.ACSetting.acTemp = cJSON_GetObjectItem(mobile, "AC_Temp")->valueint;
                    device.airConSetting.acTemp = mobileData.ACSetting.acTemp;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "AC_AutoMode")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.ACSetting.isAutoMode = cJSON_GetObjectItem(mobile, "AC_AutoMode")->valueint;
                    device.airConSetting.isAutoMode = mobileData.ACSetting.isAutoMode;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "AC_CoolMode")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.ACSetting.isCoolMode = cJSON_GetObjectItem(mobile, "AC_CoolMode")->valueint;
                    device.airConSetting.isCoolMode = mobileData.ACSetting.isCoolMode;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "setWifi")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.wifiSetting.isWifiSet = cJSON_GetObjectItem(mobile, "setWifi")->valueint;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "wifiName")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.wifiSetting.WifiName = cJSON_GetObjectItem(mobile, "wifiName")->valuestring;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "wifiPass")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.wifiSetting.WifiPass = cJSON_GetObjectItem(mobile, "wifiPass")->valuestring;
                    portEXIT_CRITICAL(&param_lock);
                }
                if(cJSON_GetObjectItem(mobile, "otaIp")){
                    portENTER_CRITICAL(&param_lock);
                    OTA.ip = cJSON_GetObjectItem(mobile, "otaIp")->valuestring;
                    portEXIT_CRITICAL(&param_lock);
                    NVSDriverWriteString(NVS_NAMESPACE_OTA, NVS_KEY_IP_URL, OTA.ip);
                }               
                if(cJSON_GetObjectItem(mobile, "update")){
                    portENTER_CRITICAL(&param_lock);
                    mobileData.isUpdate = cJSON_GetObjectItem(mobile, "update")->valueint;
                    // OTA.isUpdate = mobileData.isUpdate;
                    portEXIT_CRITICAL(&param_lock);
                }
            }
            receiveData = NULL;
        }
        if(OTA.isUpdate == true){
            AWS_Stop();
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
        // printf("remaining memory of task_transmit_to_AWS : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }  
}


void AWS_Run(void){
    MQTT_Start(AWS_URI);
    xTaskCreate(&AWSTaskSend, "AWSTaskSend", 1024*4, NULL, 3,  AWSTaskSendHandle);
    xTaskCreate(&AWSTaskRecv, "AWSTaskRecv", 1024*6, NULL, 4,  AWSTaskRecvHandle);
}

void AWS_Stop(void){
    vTaskDelete(AWSTaskSendHandle);
    vTaskDelete(AWSTaskRecvHandle);
}