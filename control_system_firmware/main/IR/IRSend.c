#include "IRSend.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include "soc/uart_struct.h"
#include "stdio.h"
#include "string.h"
#include "esp_log.h"
// #include "ir_tools.h"
#include "../INCLUDE/config.h"

#define TAG "RMT_SEVICE"

#define SIGNAL_LENGTH                           228

void rmt_tx_init(void){
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
    rmt_tx.clk_div = 80;
    rmt_tx.mem_block_num = 1;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_tx.tx_config.carrier_duty_percent = 33;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.tx_config.loop_en = false;

    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
}

esp_err_t rmt_tx_trans(uint16_t signal[], int signal_len){
    signal_len = (signal_len / sizeof(signal[0]))/2;
    esp_err_t ret;
    rmt_item32_t signal_on[signal_len];
    for(int i = 0, t = 0; i < (signal_len*2) - 1; i += 2, t++){
        signal_on[t].duration0 = signal[i];
        signal_on[t].level0 = 1;
        signal_on[t].duration1 = signal[i + 1];
        signal_on[t].level1 = 0;
    }
    ret = rmt_write_items(RMT_TX_CHANNEL, signal_on, signal_len, true);
    // vTaskDelay(2000/portTICK_PERIOD_MS);
    return ret;
}
