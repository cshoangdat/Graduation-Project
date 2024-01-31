#include "appTask.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include "freertos/timers.h"

#include "../NVS/NVSDriver.h"
#include "../INCLUDE/common.h"
#include "../INCLUDE/config.h"
#include "../NEXTION/nextion.h"
#include "../IR/IRSend.h"

#define TAG "AppTask"

static portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;

void appSetUp(void){
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(RELAY_1_PIN, 1);
    gpio_set_level(RELAY_2_PIN, 1);
    gpio_set_level(RELAY_3_PIN, 1);
    gpio_set_level(RELAY_4_PIN, 1);
    OTA.isUpdate = false;
    SetUp.isNextion = false;
    device.isAirConOn = 0;
    device.isAirOn = 0;
    device.isDoorOn = 0;
    device.isFanOn = 0;
    device.isLedOn = 0;
    deviceMode.autoMode = 1;

    // ws2812_init();
    // led[0].red=0;
    // led[0].blue=0;
    // led[0].green=0;

    // led[1].red=0;
    // led[1].blue=0;
    // led[1].green=0;

    // led[2].red=0;
    // led[2].blue=0;
    // led[2].green=0;

    // led[3].red=0;
    // led[3].blue=0;
    // led[3].green=0;

    // led[4].red=0;
    // led[4].blue=0;
    // led[4].green=0;
    // ws2812_update(led);
    // ledState.isLed1On = 0;
    // ledState.isLed2On = 0;
    // ledState.isLed3On = 0;
    // ledState.isLed4On = 0;
    // ledState.isLed5On = 0;

    esp_err_t err = NVSDriverInit();
    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS Flash Init Failed");
    }
    err = esp_netif_init();
    if(err != ESP_OK){
        ESP_LOGE(TAG, "ESP NetIF Init Failed");
    }
    err = esp_event_loop_create_default();
    if(err != ESP_OK){
        ESP_LOGE(TAG, "ESP Create Event Failed");
    }

    SetUp.isWifiConnected = false;
    if(NVSDriverReadString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_SSID, &WifiData.WifiName) != ESP_OK){
        WifiData.WifiName = (char* )DEFAULT_SSID;
    }
    if(NVSDriverReadString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_PASS, &WifiData.WifiPass) != ESP_OK){
        WifiData.WifiPass = (char* )DEFAULT_PASS;
    }
    if(NVSDriverReadString(NVS_NAMESPACE_OTA, NVS_KEY_IP_URL, &OTA.ip) != ESP_OK){
        OTA.ip = (char* )DEFAULT_OTA_IP;
    }
    initNextion();
    if(SetUp.isNextion == true){
        // sendData("page 0\xFF\xFF\xFF");
        char* data_out = (char*)malloc(TX_BUF_SIZE);
        sprintf(data_out, "wifi.wifi_id.txt=\"%s\"\xFF\xFF\xFF",WifiData.WifiName);
        sendData(data_out);  
        sprintf(data_out, "wifi.wifi_pass.txt=\"%s\"\xFF\xFF\xFF",WifiData.WifiPass);
        sendData(data_out);
        sprintf(data_out, "firmware.ip.txt=\"%s\"\xFF\xFF\xFF",OTA.ip);
        sendData(data_out);
        sendData("wifi_wait.j0.val=0\xFF\xFF\xFF");
        sendData("firmware_wait.j0.val=0\xFF\xFF\xFF");
        sprintf(data_out, "firmware.fw_ver.txt=\"%d.%d.%d\"\xFF\xFF\xFF",FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_BUILD);
        sendData(data_out);
        sendData("center.mode.val=0\xFF\xFF\xFF");
        sendData("center.mode_state.txt=\"AUTO\"\xFF\xFF\xFF");
        sendData("tsw minus,0\xFF\xFF\xFF");
        sendData("tsw plus,0\xFF\xFF\xFF");
        sendData("tsw send,0\xFF\xFF\xFF");
        sendData("tsw cool,0\xFF\xFF\xFF");
        sendData("tsw auto,0\xFF\xFF\xFF");
        sendData("tsw bulb,0\xFF\xFF\xFF");
        sendData("tsw fan,0\xFF\xFF\xFF");
        sendData("tsw air,0\xFF\xFF\xFF");
        sendData("tsw door,0\xFF\xFF\xFF");
        sendData("tsw air_con,0\xFF\xFF\xFF");
        sendData("page 0\xFF\xFF\xFF");
    }
    rmt_tx_init();
}