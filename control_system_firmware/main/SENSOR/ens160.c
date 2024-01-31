#include "ens160.h"
#include <stdio.h>
#include "esp_console.h"
#include "esp_log.h"
#include <string.h>
#include <unistd.h>
#include "../INCLUDE/config.h"

#define TAG "ENS160_DRIVER"
i2c_cmd_handle_t cmd;

typedef struct
{
    uint16_t aqi;
    uint16_t tvoc;
    uint16_t eco2;
}data_t;

data_t data;

static void i2c_set_up_frame(uint8_t data_addr){
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ENS160_I2C_ADDR_1  << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
}

static esp_err_t i2c_write_data(uint8_t data_addr, uint16_t data){
    i2c_set_up_frame(data_addr);
    uint8_t value[2];
    memcpy(value, &data, sizeof(uint16_t));
    i2c_master_write(cmd, value, sizeof(value), ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    // usleep(1000*10);
    return ret;
}

static esp_err_t i2c_read_data(uint8_t data_addr, uint16_t *data){
    i2c_set_up_frame(data_addr);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ENS160_I2C_ADDR_1 << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    uint8_t value[2];
    i2c_master_read(cmd, value, sizeof(value), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK) {
        memcpy(data, value, sizeof(uint16_t));
    }
    return ret;
}

static void ens160_checkPartID(void){
    uint16_t data;
    i2c_read_data(ENS160_REG_PARTID, &data);
    ESP_LOGI(TAG, "ENS160 PART ID: 0x0%hX", data);
    if(data == ENS160_PARTID) ESP_LOGI(TAG,"ENS160 is ready");
    else ESP_LOGE(TAG,"ENS160 wrong part ID");
}

static void ens160_setMode(uint16_t OPMODE){
    i2c_write_data(ENS160_REG_OPMODE, OPMODE);
    char* mode = (char*) malloc(20*sizeof(char));
    switch(OPMODE){
        case ENS160_OPMODE_DEEP_SLEEP:
            strcpy(mode, "Deep sleep");
            break;
        case ENS160_OPMODE_IDLE:
            strcpy(mode, "Idle");
            break;
        case ENS160_OPMODE_STANDARD:
            strcpy(mode, "Standard");
            break;
        default:
            strcpy(mode, "Unknown");
            break;
    }
    ESP_LOGD(TAG,"ENS160 set mode: %s", mode);
    free(mode);
}

static void ens160_clearCommand(void){
    i2c_write_data(ENS160_REG_COMMAND, ENS160_COMMAND_NOP);
    i2c_write_data(ENS160_REG_COMMAND, ENS160_COMMAND_CLRGPR);
    uint16_t status;
    i2c_read_data(ENS160_REG_DATA_STATUS, &status);
    ESP_LOGD(TAG, "ENS160 clear command status: 0x%hX", status);  
}

static void ens160_getFirmware(void){
    uint16_t read0;
    uint16_t read1;
    i2c_write_data(ENS160_REG_COMMAND, ENS160_COMMAND_GET_APPVER);
    i2c_read_data(ENS160_REG_GPR_READ_0, &read0);
    i2c_read_data(ENS160_REG_GPR_READ_1, &read1);
    uint16_t release = (read0 >> 4) & 0xf;
    uint16_t version = read0 & 0xf;
    ESP_LOGD(TAG, "ENS160 FW_Release: %hx", release);
    ESP_LOGD(TAG, "ENS160 FW_Version: %hu", version);
    ESP_LOGD(TAG, "ENS160 FW_Sub-Version: %hu", read1);
    vTaskDelay(ENS160_BOOTING/portTICK_PERIOD_MS);
}

void ens160_init(void){
    ens160_checkPartID();
    ens160_setMode(ENS160_OPMODE_IDLE);
    ens160_clearCommand();
    ens160_getFirmware();
    ens160_setMode(ENS160_OPMODE_STANDARD);
    ESP_LOGI(TAG, "ENS160 init done");
    vTaskDelay(ENS160_BOOTING/portTICK_PERIOD_MS);
}

void ens160_measure(void){
    ESP_LOGD(TAG, "ENS160 Start Measurement");
    uint16_t status;
    i2c_read_data(ENS160_REG_DATA_STATUS, &status);
    ESP_LOGD(TAG, "ENS160 Status: %hX", status);
    if(IS_NEWDAT(status)){
        i2c_read_data(ENS160_REG_DATA_AQI, &data.aqi);
        i2c_read_data(ENS160_REG_DATA_TVOC, &data.tvoc);
        i2c_read_data(ENS160_REG_DATA_ECO2, &data.eco2);
        data.aqi = (data.aqi) & 0x7;
    }
}

uint16_t ens160_getAQI(void){
    return data.aqi;
}

uint16_t ens160_getTVOC(void){
    return data.tvoc;
}

uint16_t ens160_geteCO2(void){
    return data.eco2;
}



