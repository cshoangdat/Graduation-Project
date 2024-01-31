#include "aht21.h"
#include <stdio.h>
#include "esp_console.h"
#include "esp_log.h"
#include <string.h>
#include <unistd.h>
#include "../INCLUDE/config.h"

#define TAG "AHT21_DRIVER"
i2c_cmd_handle_t cmd;

typedef struct
{
    int32_t humid;
    int32_t temp;
}data_t;

static bool aht21_checkStatus(void){
    bool result = false;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AHT21_I2C_ADDR  << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    uint8_t status;
    i2c_master_read(cmd, &status, sizeof(uint8_t), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    if((AHT21_STATUS_OK & status) == AHT21_STATUS_OK){
        result = true;
        ESP_LOGI(TAG, "AHT21 is ready");
    } 
    return result;
}

static void aht21_startMeasure(void){
    // vTaskDelay(10/portTICK_PERIOD_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AHT21_I2C_ADDR  << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, AHT21_COMMAND_TRIGGER, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, AHT21_MEASUREMENT_DATA0, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, AHT21_MEASUREMENT_DATA1, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
}

static void aht21_readMeasure(data_t* const data){
    // vTaskDelay(AHT21_BOOTING/portTICK_PERIOD_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AHT21_I2C_ADDR  << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    uint8_t*measure = (uint8_t*)malloc(7*sizeof(uint8_t));
    i2c_master_read(cmd, measure, 7*sizeof(uint8_t), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    data->humid = (measure[1] << 12U) | (measure[2] << 4U) | ((measure[3] & 0xF0) >> 4U);
    data->temp = ((measure[3] & 0x0F) << 16U) | (measure[4] << 8U) | (measure[5]);
    free(measure);
}

void aht21_init(void){
    if(aht21_checkStatus()){
        vTaskDelay(10/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "AHT21 init done");
    }
    else{
        ESP_LOGE(TAG, "AHT21 init failed");
    }
}

float aht21_getTemperature(void){
    aht21_startMeasure();
    data_t data;
    aht21_readMeasure(&data);
    return ((((float)data.temp / (1 << 20U))*200.0) - 50);
}

float aht21_getHumidity(void){
    aht21_startMeasure();
    data_t data;
    aht21_readMeasure(&data);
    return (((float)data.humid / (1 << 20U))*100.0);
}






