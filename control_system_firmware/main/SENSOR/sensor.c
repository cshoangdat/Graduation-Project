#include "sensor.h"
#include "../SENSOR/aht21.h"
#include "../SENSOR/ens160.h"
#include "../I2C/i2cDriver.h"
#include "../INCLUDE/config.h"
#include "../INCLUDE/common.h"
#include "esp_log.h"

const char* TAG = "SENSOR";

static void aht21Task(void *ptr){
    aht21_init();
    vTaskDelay(AHT21_BOOTING*2/portTICK_PERIOD_MS);
    while(1){
        SensorData.temperature = aht21_getTemperature();
        SensorData.humid = aht21_getHumidity();
        ESP_LOGD(TAG, "Temperature: %.2f", SensorData.temperature);
        ESP_LOGD(TAG, "Humidity: %.2f", SensorData.humid);
        vTaskDelay(1500/portTICK_PERIOD_MS);
        // printf("remaining memory of Task readAHT : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }
}

static void ens160Task(void *ptr){
    ens160_init();
    while(1){
        ens160_measure();
        SensorData.aqi = ens160_getAQI();
        SensorData.eCo2 = ens160_geteCO2();
        SensorData.tvoc = ens160_getTVOC();
        ESP_LOGD(TAG, "AQI: %hu", SensorData.aqi);
        ESP_LOGD(TAG, "TVOC: %hu",  SensorData.tvoc);
        ESP_LOGD(TAG, "eCO2: %hu", SensorData.eCo2);
        vTaskDelay(1500/portTICK_PERIOD_MS);
        // printf("remaining memory of Task readENS : %d byte",uxTaskGetStackHighWaterMark(NULL));
    }
}

void sensorRead(void){
    esp_err_t ret = i2c_master_init();
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "i2c init failed");
    }
    ESP_LOGI(TAG, "I2C init complete");
    xTaskCreate(aht21Task, "aht21Task", 1024*2, NULL, 9, NULL);
    xTaskCreate(ens160Task, "ens160Task", 1024*2, NULL, 9, NULL);
}