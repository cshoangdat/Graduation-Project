#include "driver/gpio.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "nextion.h"
#include "INCLUDE/common.h"
#include "INCLUDE/config.h"
#include "string.h"
#include "esp_log.h"
#include "cJSON.h"
#include "../NVS/NVSDriver.h"
#include "../OTA/ota.h"
#include "../AWS/aws.h"
#include "../IR/IRSend.h"
#include "../IR/panasonic.h"

static const char *TX_TAG               =                      "Nextion_TxTask";
static const char *RX_TAG               =                      "Nextion_RxTask";

TaskHandle_t TxTaskHandle;
TaskHandle_t RxTaskHandle;
TaskHandle_t deviceControlHandle;

static void DelOneChar(char* char_data, const char erase_char)
{
    uint16_t data = strlen(char_data);            
    for(uint16_t i=0; i<data; i++)        
    if(char_data[i] == erase_char) {                       
        for(uint16_t j=i--; j<data;j++)   
            char_data[j]=char_data[j+1];                
        data--;                           
    }
}

int sendData(const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_PORT_NUM, data, len);
    ESP_LOGD(TX_TAG, "Wrote %d bytes", txBytes);
    return txBytes;
}

void initNextion() {
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TXD_PIN, UART_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    SetUp.isNextion = true;
}

static void NextionTxTask(void *arg)
{
    char* dataOut = (char*)malloc(TX_BUF_SIZE);
    while (1) {
        if(page.center == 1){
            sprintf(dataOut, "center.temp.txt=\"%.2f\"\xFF\xFF\xFF",SensorData.temperature);
            sendData(dataOut);
            sprintf(dataOut, "center.humid.txt=\"%.2f %%\"\xFF\xFF\xFF",SensorData.humid);
            sendData(dataOut);
            sprintf(dataOut, "center.aqi.txt=\"%d Pa\"\xFF\xFF\xFF",SensorData.aqi);
            sendData(dataOut);
            sprintf(dataOut, "center.eco2.txt=\"%d ppm\"\xFF\xFF\xFF",SensorData.eCo2);
            sendData(dataOut);
            sprintf(dataOut, "center.tvoc.txt=\"%d ppb\"\xFF\xFF\xFF",SensorData.tvoc);
            sendData(dataOut);
            sprintf(dataOut, "center.people.txt=\"%d\"\xFF\xFF\xFF",CameraData.num);
            sendData(dataOut);
            sprintf(dataOut, "center.female.txt=\"%d\"\xFF\xFF\xFF",CameraData.femaleNum);
            sendData(dataOut);
            sprintf(dataOut, "center.male.txt=\"%d\"\xFF\xFF\xFF",CameraData.maleNum);
            sendData(dataOut);
            sprintf(dataOut, "center.g1.txt=\"%d\"\xFF\xFF\xFF",CameraData.G1Num);
            sendData(dataOut);
            sprintf(dataOut, "center.g2.txt=\"%d\"\xFF\xFF\xFF",CameraData.G2Num);
            sendData(dataOut);
            sprintf(dataOut, "center.g3.txt=\"%d\"\xFF\xFF\xFF",CameraData.G3Num);
            sendData(dataOut);
            sprintf(dataOut, "center.g4.txt=\"%d\"\xFF\xFF\xFF",CameraData.G4Num);
            sendData(dataOut);
        }
        if(OTA.isUpdate == true){
            vTaskDelete(TxTaskHandle);
        }
        // printf("remaining memory of NextionTxTask : %d byte",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

static void NextionRxTask(void *arg)
{
  ESP_LOGD(RX_TAG,"NEXTION RX TASK");
  uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
  char *dstream = malloc(RX_BUF_SIZE+1);
  while (1) {
    ESP_LOGD(RX_TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    memset(dstream,0,sizeof(malloc(RX_BUF_SIZE+1)));
    const int rxBytes = uart_read_bytes(UART_PORT_NUM, data, RX_BUF_SIZE, 100/portTICK_RATE_MS);
    if (rxBytes > 0) {
      data[rxBytes] = '\0';
      snprintf(dstream, RX_BUF_SIZE+1, "%s", data);
      ESP_LOGD(RX_TAG, "rxBytes:%d", rxBytes);
    }
    ESP_LOGD(RX_TAG, "data recv %s", dstream);

    cJSON *sub;
    sub=cJSON_Parse(dstream);

    cJSON *body =cJSON_GetObjectItem(sub,"body");
    char *value_body;
    //char *value_type_cmd;
    if(body){
        value_body=cJSON_GetObjectItem(sub,"body")->valuestring;
        ESP_LOGW(RX_TAG, "body is %s",value_body);
    }else{
        value_body="";
    }
    cJSON *command =cJSON_GetObjectItem(sub,"command");
    if(command){
        char *value_type_cmd =cJSON_GetObjectItem(sub,"command")->valuestring;
        ESP_LOGW(RX_TAG, "command is %s",value_type_cmd);
        deviceMode.isNextionContact = 1;
        if(strcmp(value_type_cmd, "center") == 0){
            page.center = 1;
        }
        else if(strcmp(value_type_cmd, "IP") == 0){
            ESP_LOGW(RX_TAG, "Save IP OTA = %s", value_body);
            NVSDriverWriteString(NVS_NAMESPACE_OTA, NVS_KEY_IP_URL, value_body);
        }
        else if(strcmp(value_type_cmd, "update") == 0){
            if(NVSDriverReadString(NVS_NAMESPACE_OTA, NVS_KEY_IP_URL, &OTA.ip) != ESP_OK){
                OTA.ip = (char* )DEFAULT_OTA_IP;
            }
            char* url = (char*)malloc(40);
            sprintf(url,"%s%s%s","http://",OTA.ip,":8000/firmware.bin");
            ESP_LOGW(RX_TAG, "URL: %s", url);
            OTA.isUpdate = true;
            ESP_LOGW(RX_TAG, "UPDATE");
            OTARun(url);
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            for(int i = 1; i<= 100; i++){
                sprintf(data_out, "firmware_wait.j0.val=%d\xFF\xFF\xFF",i);
                sendData(data_out);
                vTaskDelay(400/portTICK_PERIOD_MS);
            }
            vTaskDelete(RxTaskHandle);
        }
        else if(strcmp(value_type_cmd, "WN") == 0){
            ESP_LOGW(RX_TAG, "Save Wifi SSID = %s", value_body);
            NVSDriverWriteString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_SSID, value_body);
        }
        else if(strcmp(value_type_cmd, "WP") == 0){
            ESP_LOGW(RX_TAG, "Save Wifi PASS = %s", value_body);
            NVSDriverWriteString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_PASS, value_body);           
        }
        else if(strcmp(value_type_cmd, "reset") == 0){
            ESP_LOGW(RX_TAG, "RESET");
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            for(int i = 1; i<= 100; i++){
                sprintf(data_out, "wifi_wait.j0.val=%d\xFF\xFF\xFF",i);
                sendData(data_out);
                vTaskDelay(10/portTICK_PERIOD_MS);
            }
            esp_restart();
        }
        else if(strcmp(value_type_cmd, "wifi") == 0 || strcmp(value_type_cmd, "firmware") == 0){
            page.center = 0;
        }
        if(page.center == 1){
            if(strcmp(value_type_cmd, "manual") == 0){
                deviceMode.autoMode = 0;
                sendData("center.mode.val=1\xFF\xFF\xFF");
                sendData("center.mode_state.txt=\"MANUAL\"\xFF\xFF\xFF");
                sendData("tsw bulb,1\xFF\xFF\xFF");
                sendData("tsw fan,1\xFF\xFF\xFF");
                sendData("tsw air,1\xFF\xFF\xFF");
                sendData("tsw door,1\xFF\xFF\xFF");
                sendData("tsw air_con,1\xFF\xFF\xFF");
            }
            else if(strcmp(value_type_cmd, "auto") == 0){
                deviceMode.autoMode = 1;
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
            }
            if(deviceMode.autoMode == 0){
                if(strcmp(value_type_cmd, "ledOn") == 0){
                    // led[0].red=255;
                    // led[0].blue=0;
                    // led[0].green=0;
                    // ws2812_update(led);
                    device.isLedOn = 1;
                    gpio_set_level(RELAY_1_PIN, 0);
                }
                else if(strcmp(value_type_cmd, "ledOff") == 0){
                    // led[0].red=0;
                    // led[0].blue=0;
                    // led[0].green=0;
                    // ws2812_update(led);
                    device.isLedOn = 0;
                    gpio_set_level(RELAY_1_PIN, 1);
                }
                else if(strcmp(value_type_cmd, "fanOn") == 0){
                    // led[1].red=255;
                    // led[1].blue=0;
                    // led[1].green=0;
                    // ws2812_update(led);                    
                    device.isFanOn = 1;
                    gpio_set_level(RELAY_2_PIN, 0);
                }
                else if(strcmp(value_type_cmd, "fanOff") == 0){
                    // led[1].red=0;
                    // led[1].blue=0;
                    // led[1].green=0;
                    // ws2812_update(led);   
                    device.isFanOn = 0;
                    gpio_set_level(RELAY_2_PIN, 1);
                }
                else if(strcmp(value_type_cmd, "airOn") == 0){
                    // led[2].red=255;
                    // led[2].blue=0;
                    // led[2].green=0;
                    // ws2812_update(led); 
                    device.isAirOn = 1;
                    gpio_set_level(RELAY_3_PIN, 0);
                }
                else if(strcmp(value_type_cmd, "airOff") == 0){
                    // led[2].red=0;
                    // led[2].blue=0;
                    // led[2].green=0;
                    // ws2812_update(led); 
                    device.isAirOn = 0;
                    gpio_set_level(RELAY_3_PIN, 1);
                }
                else if(strcmp(value_type_cmd, "doorOn") == 0){
                    // led[3].red=255;
                    // led[3].blue=0;
                    // led[3].green=0;
                    // ws2812_update(led); 
                    device.isDoorOn = 1;
                    gpio_set_level(RELAY_4_PIN, 0);
                }
                else if(strcmp(value_type_cmd, "doorOff") == 0){
                    // led[3].red=0;
                    // led[3].blue=0;
                    // led[3].green=0;
                    // ws2812_update(led); 
                    device.isDoorOn = 0;
                    gpio_set_level(RELAY_4_PIN, 1);
                }
                else if(strcmp(value_type_cmd, "airConOn") == 0){
                    // led[4].red=255;
                    // led[4].blue=0;
                    // led[4].green=0;
                    // ws2812_update(led); 
                    device.isAirConOn = 1;
                    device.airConSetting.isCoolMode = 1;
                    rmt_tx_trans(panasonic_powerOn, sizeof(panasonic_powerOn));
                    device.airConSetting.acState = AC_ON;
                    sendData("center.ac_num.val=20\xFF\xFF\xFF");
                }
                else if(strcmp(value_type_cmd, "airConOff") == 0){
                    // led[4].red=0;
                    // led[4].blue=0;
                    // led[4].green=0;
                    // ws2812_update(led); 
                    device.isAirConOn = 0;
                    device.airConSetting.isCoolMode = 0;
                    rmt_tx_trans(panasonic_powerOff, sizeof(panasonic_powerOff));
                    device.airConSetting.acState = AC_OFF;
                }
                if(device.isAirConOn == 1){
                    if(strcmp(value_type_cmd, "coolOn") == 0){
                        device.airConSetting.isCoolMode = 1;
                        device.airConSetting.isAutoMode = 0;
                    }
                    else if(strcmp(value_type_cmd, "autoOn") == 0){
                        device.airConSetting.isCoolMode = 0;
                        device.airConSetting.isAutoMode = 1;
                        rmt_tx_trans(panasonic_auto, sizeof(panasonic_auto));
                        device.airConSetting.acState = AC_AUTO;                      
                    }
                    if(device.airConSetting.isCoolMode == 1){
                        if(strcmp(value_type_cmd, "16") == 0){
                            device.airConSetting.acTemp = 16;
                            sendData("center.ac_num.val=16\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_16, sizeof(panasonic_cool_16));
                            device.airConSetting.acState = AC_COOL_16; 
                            //sendAC                     
                        } 
                        else if(strcmp(value_type_cmd, "17") == 0){
                            device.airConSetting.acTemp = 17;
                            sendData("center.ac_num.val=17\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            //sendAC
                            rmt_tx_trans(panasonic_cool_17, sizeof(panasonic_cool_17)); 
                            device.airConSetting.acState = AC_COOL_17;                       
                        }  
                        else if(strcmp(value_type_cmd, "18") == 0){
                            sendData("center.ac_num.val=18\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            device.airConSetting.acTemp = 18;   
                            //sendAC
                            rmt_tx_trans(panasonic_cool_18, sizeof(panasonic_cool_18));   
                            device.airConSetting.acState = AC_COOL_18;                       
                        }  
                        else if(strcmp(value_type_cmd, "19") == 0){
                            sendData("center.ac_num.val=19\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            device.airConSetting.acTemp = 19;
                            rmt_tx_trans(panasonic_cool_19, sizeof(panasonic_cool_19)); 
                            //sendAC
                            device.airConSetting.acState = AC_COOL_19;                       
                        }
                        else if(strcmp(value_type_cmd, "20") == 0){
                            device.airConSetting.acTemp = 20;
                            sendData("center.ac_num.val=20\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");                            
                            rmt_tx_trans(panasonic_cool_20, sizeof(panasonic_cool_20));   
                            //sendAC                 
                            device.airConSetting.acState = AC_COOL_20;      
                        } 
                        else if(strcmp(value_type_cmd, "21") == 0){
                            device.airConSetting.acTemp = 21;
                            sendData("center.ac_num.val=21\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_21, sizeof(panasonic_cool_21));  
                            //sendAC                      
                            device.airConSetting.acState = AC_COOL_21;
                        }  
                        else if(strcmp(value_type_cmd, "22") == 0){
                            device.airConSetting.acTemp = 22;
                            sendData("center.ac_num.val=22\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_22, sizeof(panasonic_cool_22));   
                            //sendAC 
                            device.airConSetting.acState = AC_COOL_22;                    
                        }  
                        else if(strcmp(value_type_cmd, "23") == 0){
                            device.airConSetting.acTemp = 23;
                            sendData("center.ac_num.val=23\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_23, sizeof(panasonic_cool_23));   
                            //sendAC
                            device.airConSetting.acState = AC_COOL_23;                     
                        }  
                        else if(strcmp(value_type_cmd, "24") == 0){
                            device.airConSetting.acTemp = 24;
                            sendData("center.ac_num.val=24\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_24, sizeof(panasonic_cool_24));  
                            //sendAC
                            device.airConSetting.acState = AC_COOL_24;                      
                        } 
                        else if(strcmp(value_type_cmd, "25") == 0){
                            device.airConSetting.acTemp = 25;
                            sendData("center.ac_num.val=25\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_25, sizeof(panasonic_cool_25));      
                            //sendAC   
                            device.airConSetting.acState = AC_COOL_25;                 
                        }  
                        else if(strcmp(value_type_cmd, "26") == 0){
                            device.airConSetting.acTemp = 26;
                            sendData("center.ac_num.val=26\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_26, sizeof(panasonic_cool_26));       
                            //sendAC
                            device.airConSetting.acState = AC_COOL_26;                 
                        }  
                        else if(strcmp(value_type_cmd, "27") == 0){
                            device.airConSetting.acTemp = 27;
                            sendData("center.ac_num.val=27\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_27, sizeof(panasonic_cool_27));      
                            //sendAC
                            device.airConSetting.acState = AC_COOL_27;                  
                        }
                        else if(strcmp(value_type_cmd, "28") == 0){
                            device.airConSetting.acTemp = 28;
                            sendData("center.ac_num.val=28\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_28, sizeof(panasonic_cool_28));  
                            //sendAC
                            device.airConSetting.acState = AC_COOL_28;                      
                        } 
                        else if(strcmp(value_type_cmd, "29") == 0){
                            device.airConSetting.acTemp = 29;
                            sendData("center.ac_num.val=29\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_29, sizeof(panasonic_cool_29));     
                            //sendAC
                            device.airConSetting.acState = AC_COOL_29;                    
                        }  
                        else if(strcmp(value_type_cmd, "30") == 0){
                            device.airConSetting.acTemp = 30;
                            sendData("center.ac_num.val=30\xFF\xFF\xFF");
                            sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                            rmt_tx_trans(panasonic_cool_30, sizeof(panasonic_cool_30));             
                            //sendAC  
                            device.airConSetting.acState = AC_COOL_30;          
                        }                                      
                    }
                }           
            }
        }
    }
    // printf("remaining memory of NextionRxTask : %d byte",uxTaskGetStackHighWaterMark(NULL));
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
  free(data);
  free(dstream);
}

static void updateLed(void){
    if(device.isLedOn == 1 && ledState.isLed1On == 0){
        led[0].red=255;
        led[0].blue=0;
        led[0].green=0;
        ledState.isLed1On = 1;
    }
    else if (device.isLedOn == 0 && ledState.isLed1On == 1){
        led[0].red=0;
        led[0].blue=0;
        led[0].green=0;
        ledState.isLed1On = 0;
    }
    if(device.isFanOn == 1 && ledState.isLed2On == 0){
        led[1].red=255;
        led[1].blue=0;
        led[1].green=0;
        ledState.isLed2On = 1;
    }
    else if(device.isFanOn == 0 && ledState.isLed2On == 1){
        led[1].red=0;
        led[1].blue=0;
        led[1].green=0;
        ledState.isLed2On = 0;
    }
    if(device.isAirOn == 1 && ledState.isLed3On == 0){
        led[2].red=255;
        led[2].blue=0;
        led[2].green=0;
        ledState.isLed3On = 1;
    }
    else if(device.isAirOn == 0 && ledState.isLed3On == 1){
        led[2].red=0;
        led[2].blue=0;
        led[2].green=0;
        ledState.isLed3On = 0;
    } 
    if(device.isDoorOn == 1 && ledState.isLed4On == 0){
        led[3].red=255;
        led[3].blue=0;
        led[3].green=0;
        ledState.isLed4On = 1;
    }
    else if(device.isDoorOn == 0 && ledState.isLed4On == 1){
        led[3].red=0;
        led[3].blue=0;
        led[3].green=0;
        ledState.isLed4On = 0;
    } 
    if(device.isAirConOn == 1 && ledState.isLed5On == 0){
        led[4].red=255;
        led[4].blue=0;
        led[4].green=0;
        ledState.isLed5On = 1;
    }
    else if(device.isAirConOn == 0 && ledState.isLed5On == 1){
        led[4].red=0;
        led[4].blue=0;
        led[4].green=0;
        ledState.isLed5On = 0;
    } 
    ws2812_update(led);
}

void appControlDeviceTask(void *arg){
    while(1){
        if(deviceMode.autoMode == 1 || mobileData.isAutoMode == 1){
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
            if(CameraData.num == 0){
                if(device.isLedOn == 1){
                    // led[0].red=0;
                    // led[0].blue=0;
                    // led[0].green=0;
                    // ws2812_update(led);
                    sendData("center.bulb.val=0\xFF\xFF\xFF");
                    sendData("center.bulb_state.txt=\"OFF\"\xFF\xFF\xFF");
                    sendData("center.bulb_state.pco=2771\xFF\xFF\xFF");
                    device.isLedOn = 0;
                    gpio_set_level(RELAY_1_PIN, 1);
                }
                if(device.isFanOn == 1){
                    // led[1].red=0;
                    // led[1].blue=0;
                    // led[1].green=0;
                    // ws2812_update(led);
                    sendData("center.fan.val=0\xFF\xFF\xFF");
                    sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                    sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                    device.isFanOn = 0;
                    gpio_set_level(RELAY_2_PIN, 1);
                }
                if(device.isAirOn == 1){
                    // led[2].red=0;
                    // led[2].blue=0;
                    // led[2].green=0;
                    // ws2812_update(led);
                    sendData("center.air.val=0\xFF\xFF\xFF");
                    sendData("center.air_state.txt=\"OFF\"\xFF\xFF\xFF");
                    sendData("center.air_state.pco=2771\xFF\xFF\xFF");
                    device.isAirOn = 0;
                    gpio_set_level(RELAY_3_PIN, 1);
                }
                if(device.isDoorOn == 1){
                    // led[3].red=0;
                    // led[3].blue=0;
                    // led[3].green=0;
                    // ws2812_update(led);
                    sendData("center.door.val=0\xFF\xFF\xFF");
                    sendData("center.door_state.txt=\"CLOSE\"\xFF\xFF\xFF");
                    sendData("center.door_state.pco=2771\xFF\xFF\xFF");
                    device.isDoorOn = 0;
                    gpio_set_level(RELAY_4_PIN, 1);
                }
                sendData("center.air_con.val=0\xFF\xFF\xFF");
                sendData("center.air_con_state.txt=\"OFF\"\xFF\xFF\xFF");
                sendData("center.air_con_state.pco=2771\xFF\xFF\xFF");
                sendData("center.cool.val=0\xFF\xFF\xFF");
                sendData("center.auto.val=0\xFF\xFF\xFF");
                sendData("tsw minus,0\xFF\xFF\xFF"); 
                sendData("tsw plus,0\xFF\xFF\xFF"); 
                sendData("tsw send,0\xFF\xFF\xFF"); 
                sendData("tsw cool,0\xFF\xFF\xFF");
                sendData("tsw auto,0\xFF\xFF\xFF");
                sendData("center.minus.pic=17\xFF\xFF\xFF"); 
                sendData("center.minus.pic2=18\xFF\xFF\xFF");     
                sendData("center.plus.pic=15\xFF\xFF\xFF"); 
                sendData("center.plus.pic2=16\xFF\xFF\xFF");   
                sendData("center.ac_num.pco=2771\xFF\xFF\xFF"); 
                device.isAirConOn = 0;
                rmt_tx_trans(panasonic_powerOff, sizeof(panasonic_powerOff));
                device.airConSetting.acState = AC_OFF;   
            }
            else if(CameraData.num > 0){
                if(device.isLedOn == 0){
                    // led[0].red=255;
                    // led[0].blue=0;
                    // led[0].green=0;
                    // ws2812_update(led);
                    sendData("center.bulb.val=1\xFF\xFF\xFF");
                    sendData("center.bulb_state.txt=\"ON\"\xFF\xFF\xFF");
                    sendData("center.bulb_state.pco=65535\xFF\xFF\xFF");
                    gpio_set_level(RELAY_1_PIN, 0);
                    device.isLedOn = 1;
                }
                if(device.isDoorOn == 0){
                    // led[3].red=255;
                    // led[3].blue=0;
                    // led[3].green=0;
                    // ws2812_update(led);
                    sendData("center.door.val=1\xFF\xFF\xFF");
                    sendData("center.door_state.txt=\"OPEN\"\xFF\xFF\xFF");
                    sendData("center.door_state.pco=65535\xFF\xFF\xFF");
                    gpio_set_level(RELAY_4_PIN, 0);
                    device.isDoorOn = 1;
                }
                if((SensorData.temperature > 25 && SensorData.temperature <= 29) && (CameraData.num > 0 && CameraData.num <= 5)){
                    if(device.isFanOn == 0){
                        // led[1].red=255;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=1\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=65535\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 0);
                        device.isFanOn = 1;
                    }
                }
                else if((SensorData.temperature > 29 && SensorData.temperature <= 34) && (CameraData.num > 0 && CameraData.num <= 5)){
                    //Send AC 26 do
                    if(device.airConSetting.acState != AC_COOL_26){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_26, sizeof(panasonic_cool_26));
                        device.airConSetting.acState = AC_COOL_26;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=26\xFF\xFF\xFF");
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF"); 
                    }
                    if(device.isFanOn == 1){
                        // led[1].red=0;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=0\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 1);
                        device.isFanOn = 0;
                    }
                }
                else if((SensorData.temperature > 34) && (CameraData.num > 0 && CameraData.num <= 5)){
                    if(device.isFanOn == 0){
                        // led[1].red=255;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=1\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=65535\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 0);
                        device.isFanOn = 1;
                    }
                    if(device.airConSetting.acState != AC_COOL_26){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_26, sizeof(panasonic_cool_26));
                        device.airConSetting.acState = AC_COOL_26;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=26\xFF\xFF\xFF");
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF"); 
                    }
                    //Send AC 26 do               
                }
                else if((SensorData.temperature > 25 && SensorData.temperature <= 29) && (CameraData.num > 5 && CameraData.num <= 15)){
                    //send AC 27 do
                    if(device.isFanOn == 1){
                        // led[1].red=0;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=0\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 1);
                        device.isFanOn = 0;
                    }
                    if(device.airConSetting.acState != AC_COOL_27){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_27, sizeof(panasonic_cool_27));
                        device.airConSetting.acState = AC_COOL_27;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=27\xFF\xFF\xFF");
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF"); 
                    }
                }
                else if((SensorData.temperature > 29 && SensorData.temperature <= 34) && (CameraData.num > 5 && CameraData.num <= 15)){
                    //Send AC 24 do
                    if(device.isFanOn == 1){
                        // led[1].red=0;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=0\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 1);
                        device.isFanOn = 0;
                    }
                    if(device.airConSetting.acState != AC_COOL_24){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_24, sizeof(panasonic_cool_24));
                        device.airConSetting.acState = AC_COOL_24;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=24\xFF\xFF\xFF");   
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF");                     
                    }
                }
                else if((SensorData.temperature > 34) && (CameraData.num > 5 && CameraData.num <= 15)){
                    if(device.isFanOn == 0){
                        // led[1].red=255;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=1\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=65535\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 0);
                        device.isFanOn = 1;
                    }
                    if(device.airConSetting.acState != AC_COOL_23){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_23, sizeof(panasonic_cool_23));
                        device.airConSetting.acState = AC_COOL_23;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=23\xFF\xFF\xFF"); 
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF");  
                    }
                    //Send AC 23 do              
                }
                else if((SensorData.temperature > 25 && SensorData.temperature <= 29) && (CameraData.num > 15)){
                    //send AC 25 do
                    if(device.isFanOn == 1){
                        // led[1].red=0;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=0\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 1);
                        device.isFanOn = 0;
                    }
                    if(device.airConSetting.acState != AC_COOL_25){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_25, sizeof(panasonic_cool_25));
                        device.airConSetting.acState = AC_COOL_25;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=25\xFF\xFF\xFF");  
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF");  
                    }
                }
                else if((SensorData.temperature > 29 && SensorData.temperature <= 34) && (CameraData.num > 15)){
                    //Send AC 22 do
                    if(device.isFanOn == 1){
                        // led[1].red=0;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=0\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 1);
                        device.isFanOn = 0;
                    }
                    if(device.airConSetting.acState != AC_COOL_22){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_22, sizeof(panasonic_cool_22));
                        device.airConSetting.acState = AC_COOL_22;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=22\xFF\xFF\xFF");
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF");    
                    }     
                }
                else if((SensorData.temperature > 34) && (CameraData.num > 15)){
                    if(device.isFanOn == 0){
                        // led[1].red=255;
                        // led[1].blue=0;
                        // led[1].green=0;
                        // ws2812_update(led);
                        sendData("center.fan.val=1\xFF\xFF\xFF");
                        sendData("center.fan_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.fan_state.pco=65535\xFF\xFF\xFF");
                        gpio_set_level(RELAY_2_PIN, 0);
                        device.isFanOn = 1;
                    }
                    if(device.airConSetting.acState != AC_COOL_20){
                        // led[4].red=255;
                        // led[4].blue=0;
                        // led[4].green=0;
                        // ws2812_update(led);
                        rmt_tx_trans(panasonic_cool_20, sizeof(panasonic_cool_20));
                        device.airConSetting.acState = AC_COOL_20;
                        sendData("center.air_con.val=1\xFF\xFF\xFF");
                        sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                        sendData("center.ac_num.val=20\xFF\xFF\xFF");
                        sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                        sendData("center.cool.val=1\xFF\xFF\xFF");  
                    }
                    //Send AC 20 do                     
                }
                if(SensorData.eCo2 >= 800 || SensorData.aqi >= 3){
                    if(device.isAirOn == 0){
                        // led[2].red=255;
                        // led[2].blue=0;
                        // led[2].green=0;
                        // ws2812_update(led);
                        sendData("center.air.val=1\xFF\xFF\xFF");
                        sendData("center.air_state.txt=\"ON\"\xFF\xFF\xFF");
                        sendData("center.air_state.pco=65535\xFF\xFF\xFF");
                        gpio_set_level(RELAY_3_PIN, 0);
                        device.isAirOn = 1 ;                  
                    }
                }
                else{
                    if(device.isAirOn == 1){
                        // led[2].red=0;
                        // led[2].blue=0;
                        // led[2].green=0;
                        // ws2812_update(led);
                        sendData("center.air.val=0\xFF\xFF\xFF");
                        sendData("center.air_state.txt=\"OFF\"\xFF\xFF\xFF");
                        sendData("center.air_state.pco=2771\xFF\xFF\xFF");
                        gpio_set_level(RELAY_3_PIN, 1);
                        device.isAirOn = 0;
                    } 
                }
            }
        }
        if(deviceMode.autoMode == 0 && deviceMode.isNextionContact == 0){
            sendData("center.mode.val=1\xFF\xFF\xFF");
            sendData("center.mode_state.txt=\"MANUAL\"\xFF\xFF\xFF");
            sendData("tsw bulb,1\xFF\xFF\xFF");
            sendData("tsw fan,1\xFF\xFF\xFF");
            sendData("tsw air,1\xFF\xFF\xFF");
            sendData("tsw door,1\xFF\xFF\xFF");
            sendData("tsw air_con,1\xFF\xFF\xFF");
            if(device.isLedOn == 1){
                // led[0].red=255;
                // led[0].blue=0;
                // led[0].green=0;
                // ws2812_update(led);
                sendData("center.bulb.val=1\xFF\xFF\xFF");
                sendData("center.bulb_state.txt=\"ON\"\xFF\xFF\xFF");
                sendData("center.bulb_state.pco=65535\xFF\xFF\xFF");
                gpio_set_level(RELAY_1_PIN, 0);
                device.isLedOn = 1;
            }
            else{
                // led[0].red=0;
                // led[0].blue=0;
                // led[0].green=0;
                // ws2812_update(led);
                sendData("center.bulb.val=0\xFF\xFF\xFF");
                sendData("center.bulb_state.txt=\"OFF\"\xFF\xFF\xFF");
                sendData("center.bulb_state.pco=2771\xFF\xFF\xFF");
                device.isLedOn = 0;
                gpio_set_level(RELAY_1_PIN, 1);
            }
            if(device.isFanOn == 1){
                // led[1].red=255;
                // led[1].blue=0;
                // led[1].green=0;
                // ws2812_update(led);
                sendData("center.fan.val=1\xFF\xFF\xFF");
                sendData("center.fan_state.txt=\"ON\"\xFF\xFF\xFF");
                sendData("center.fan_state.pco=65535\xFF\xFF\xFF");
                gpio_set_level(RELAY_2_PIN, 0);
                device.isFanOn = 1;
            }
            else{
                // led[1].red=0;
                // led[1].blue=0;
                // led[1].green=0;
                // ws2812_update(led);
                sendData("center.fan.val=0\xFF\xFF\xFF");
                sendData("center.fan_state.txt=\"OFF\"\xFF\xFF\xFF");
                sendData("center.fan_state.pco=2771\xFF\xFF\xFF");
                gpio_set_level(RELAY_2_PIN, 1);
                device.isFanOn = 0;
            }
            if(device.isAirOn == 1){
                // led[2].red=255;
                // led[2].blue=0;
                // led[2].green=0;
                // ws2812_update(led);
                sendData("center.air.val=1\xFF\xFF\xFF");
                sendData("center.air_state.txt=\"ON\"\xFF\xFF\xFF");
                sendData("center.air_state.pco=65535\xFF\xFF\xFF");
                gpio_set_level(RELAY_3_PIN, 0);
                device.isAirOn = 1 ; 
            }
            else{
                // led[2].red=0;
                // led[2].blue=0;
                // led[2].green=0;
                // ws2812_update(led);
                sendData("center.air.val=0\xFF\xFF\xFF");
                sendData("center.air_state.txt=\"OFF\"\xFF\xFF\xFF");
                sendData("center.air_state.pco=2771\xFF\xFF\xFF");
                gpio_set_level(RELAY_3_PIN, 1);
                device.isAirOn = 0;
            }
            if(device.isDoorOn == 1){
                // led[3].red=255;
                // led[3].blue=0;
                // led[3].green=0;
                // ws2812_update(led);
                sendData("center.door.val=1\xFF\xFF\xFF");
                sendData("center.door_state.txt=\"OPEN\"\xFF\xFF\xFF");
                sendData("center.door_state.pco=65535\xFF\xFF\xFF");
                gpio_set_level(RELAY_4_PIN, 0);
                device.isDoorOn = 1;
            }
            else{
                // led[3].red=0;
                // led[3].blue=0;
                // led[3].green=0;
                // ws2812_update(led);
                sendData("center.door.val=0\xFF\xFF\xFF");
                sendData("center.door_state.txt=\"CLOSE\"\xFF\xFF\xFF");
                sendData("center.door_state.pco=2771\xFF\xFF\xFF");
                device.isDoorOn = 0;
                gpio_set_level(RELAY_4_PIN, 1);
            }
            if(device.isAirConOn == 1){
                // led[4].red=255;
                // led[4].blue=0;
                // led[4].green=0;
                // ws2812_update(led);
                sendData("center.air_con.val=1\xFF\xFF\xFF");
                sendData("center.air_con_state.txt=\"ON\"\xFF\xFF\xFF");
                sendData("center.air_con_state.pco=65535\xFF\xFF\xFF");
                sendData("tsw minus,1\xFF\xFF\xFF");
                sendData("tsw plus,1\xFF\xFF\xFF");
                sendData("tsw send,1\xFF\xFF\xFF");
                sendData("tsw cool,1\xFF\xFF\xFF");
                sendData("tsw auto,1\xFF\xFF\xFF");
                sendData("center.minus.pic=18\xFF\xFF\xFF"); 
                sendData("center.minus.pic2=17\xFF\xFF\xFF");     
                sendData("center.plus.pic=16\xFF\xFF\xFF"); 
                sendData("center.plus.pic2=15\xFF\xFF\xFF");   
                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 

                if(device.airConSetting.isAutoMode == 1){
                    sendData("center.auto.val=1\xFF\xFF\xFF");
                    sendData("center.cool.val=0\xFF\xFF\xFF");
                    rmt_tx_trans(panasonic_auto, sizeof(panasonic_auto));
                    device.airConSetting.acState = AC_AUTO;
                }
                if(device.airConSetting.isCoolMode == 1){
                    sendData("center.cool.val=1\xFF\xFF\xFF");
                    sendData("center.auto.val=0\xFF\xFF\xFF");
                    if(deviceMode.isNextionContact == 0){
                        switch(mobileData.ACSetting.acTemp){
                            case AC_COOL_16:
                                device.airConSetting.acTemp = 16;
                                rmt_tx_trans(panasonic_cool_16, sizeof(panasonic_cool_16));
                                device.airConSetting.acState = AC_COOL_16;
                                sendData("center.ac_num.val=16\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF");
                                break;
                            case AC_COOL_17:
                                device.airConSetting.acTemp = 17;
                                rmt_tx_trans(panasonic_cool_17, sizeof(panasonic_cool_17));
                                device.airConSetting.acState = AC_COOL_17;
                                sendData("center.ac_num.val=17\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_18:
                                device.airConSetting.acTemp = 18;
                                rmt_tx_trans(panasonic_cool_18, sizeof(panasonic_cool_18));
                                device.airConSetting.acState = AC_COOL_18;
                                sendData("center.ac_num.val=18\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_19:
                                device.airConSetting.acTemp = 19;
                                rmt_tx_trans(panasonic_cool_19, sizeof(panasonic_cool_19));
                                device.airConSetting.acState = AC_COOL_19;
                                sendData("center.ac_num.val=19\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_20:
                                device.airConSetting.acTemp = 20;
                                rmt_tx_trans(panasonic_cool_20, sizeof(panasonic_cool_20));
                                device.airConSetting.acState = AC_COOL_20;
                                sendData("center.ac_num.val=20\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_21:
                                device.airConSetting.acTemp = 21;
                                rmt_tx_trans(panasonic_cool_21, sizeof(panasonic_cool_21));
                                device.airConSetting.acState = AC_COOL_21;
                                sendData("center.ac_num.val=21\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_22:
                                device.airConSetting.acTemp = 22;
                                rmt_tx_trans(panasonic_cool_22, sizeof(panasonic_cool_22));
                                device.airConSetting.acState = AC_COOL_22;
                                sendData("center.ac_num.val=22\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_23:
                                device.airConSetting.acTemp = 23;
                                rmt_tx_trans(panasonic_cool_23, sizeof(panasonic_cool_23));
                                device.airConSetting.acState = AC_COOL_23;
                                sendData("center.ac_num.val=23\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_24:
                                device.airConSetting.acTemp = 24;
                                rmt_tx_trans(panasonic_cool_24, sizeof(panasonic_cool_24));
                                device.airConSetting.acState = AC_COOL_24;
                                sendData("center.ac_num.val=24\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_25:
                                device.airConSetting.acTemp = 25;
                                rmt_tx_trans(panasonic_cool_25, sizeof(panasonic_cool_25));
                                device.airConSetting.acState = AC_COOL_25;
                                sendData("center.ac_num.val=25\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_26:
                                device.airConSetting.acTemp = 26;
                                rmt_tx_trans(panasonic_cool_26, sizeof(panasonic_cool_26));
                                device.airConSetting.acState = AC_COOL_26;
                                sendData("center.ac_num.val=26\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_27:
                                device.airConSetting.acTemp = 27;
                                rmt_tx_trans(panasonic_cool_27, sizeof(panasonic_cool_27));
                                device.airConSetting.acState = AC_COOL_27;
                                sendData("center.ac_num.val=27\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_28:
                                device.airConSetting.acTemp = 28;
                                rmt_tx_trans(panasonic_cool_28, sizeof(panasonic_cool_28));
                                device.airConSetting.acState = AC_COOL_28;
                                sendData("center.ac_num.val=28\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_29:
                                device.airConSetting.acTemp = 29;
                                rmt_tx_trans(panasonic_cool_29, sizeof(panasonic_cool_29));
                                device.airConSetting.acState = AC_COOL_29;
                                sendData("center.ac_num.val=29\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                            case AC_COOL_30:
                                device.airConSetting.acTemp = 30;
                                rmt_tx_trans(panasonic_cool_30, sizeof(panasonic_cool_30));
                                device.airConSetting.acState = AC_COOL_30;
                                sendData("center.ac_num.val=30\xFF\xFF\xFF");
                                sendData("center.ac_num.pco=65535\xFF\xFF\xFF"); 
                                break;
                        }
                    }
                }
            }
            else{
                // led[4].red=0;
                // led[4].blue=0;
                // led[4].green=0;
                // ws2812_update(led);
                sendData("center.air_con.val=0\xFF\xFF\xFF");
                sendData("center.air_con_state.txt=\"OFF\"\xFF\xFF\xFF");
                sendData("center.air_con_state.pco=2771\xFF\xFF\xFF");
                sendData("center.cool.val=0\xFF\xFF\xFF");
                sendData("center.auto.val=0\xFF\xFF\xFF");
                sendData("tsw minus,0\xFF\xFF\xFF"); 
                sendData("tsw plus,0\xFF\xFF\xFF"); 
                sendData("tsw send,0\xFF\xFF\xFF"); 
                sendData("tsw cool,0\xFF\xFF\xFF");
                sendData("tsw auto,0\xFF\xFF\xFF");
                sendData("center.minus.pic=17\xFF\xFF\xFF"); 
                sendData("center.minus.pic2=18\xFF\xFF\xFF");     
                sendData("center.plus.pic=15\xFF\xFF\xFF"); 
                sendData("center.plus.pic2=16\xFF\xFF\xFF");   
                sendData("center.ac_num.pco=2771\xFF\xFF\xFF"); 
                rmt_tx_trans(panasonic_powerOff, sizeof(panasonic_powerOff));
                device.airConSetting.acState = AC_OFF;   
            }
        }
        if(mobileData.wifiSetting.isWifiSet == 1){
            ESP_LOGW(RX_TAG, "Save Wifi SSID = %s", mobileData.wifiSetting.WifiName);
            NVSDriverWriteString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_SSID,  mobileData.wifiSetting.WifiName);
            ESP_LOGW(RX_TAG, "Save Wifi PASS = %s", mobileData.wifiSetting.WifiPass);
            NVSDriverWriteString(NVS_NAMESPACE_CONFIG, NVS_KEY_WIFI_PASS,  mobileData.wifiSetting.WifiPass);
            ESP_LOGW(RX_TAG, "RESET");
            sendData("page 4\xFF\xFF\xFF");
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            for(int i = 1; i<= 100; i++){
                sprintf(data_out, "wifi_wait.j0.val=%d\xFF\xFF\xFF",i);
                sendData(data_out);
                vTaskDelay(10/portTICK_PERIOD_MS);
            }
            esp_restart();
        }
        if(mobileData.isUpdate == 1){
            if(NVSDriverReadString(NVS_NAMESPACE_OTA, NVS_KEY_IP_URL, &OTA.ip) != ESP_OK){
                OTA.ip = (char* )DEFAULT_OTA_IP;
            }
            char* url = (char*)malloc(40);
            sprintf(url,"%s%s%s","http://",OTA.ip,":8000/firmware.bin");
            ESP_LOGW(RX_TAG, "URL: %s", url);
            OTA.isUpdate = true;
            ESP_LOGW(RX_TAG, "UPDATE");
            OTARun(url);
            sendData("page 5\xFF\xFF\xFF");
            char* data_out = (char*)malloc(TX_BUF_SIZE);
            for(int i = 1; i<= 100; i++){
                sprintf(data_out, "firmware_wait.j0.val=%d\xFF\xFF\xFF",i);
                sendData(data_out);
                vTaskDelay(400/portTICK_PERIOD_MS);
            }
            // vTaskDelete(RxTaskHandle);
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

void appControlDevice(void){
    xTaskCreate(appControlDeviceTask, "appControlDeviceTask", 1024*5, NULL, 5, deviceControlHandle);
}

void NextionRun(void){
    xTaskCreate(NextionTxTask, "NextionTxTask", 1024*4, NULL, 3, TxTaskHandle);
    xTaskCreate(NextionRxTask, "NextionRxTask", 1024*5, NULL, 4, RxTaskHandle);
}

void NextionStop(void){
    vTaskDelete(RxTaskHandle);
    vTaskDelete(TxTaskHandle);
}