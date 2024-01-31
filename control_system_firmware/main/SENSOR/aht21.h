#ifndef __AHT_DRIVER__
#define __AHT_DRIVER__

#include "esp_err.h"
#include "driver/i2c.h"

#define AHT21_I2C_ADDR                      0X38
#define AHT21_BOOTING                       80

#define AHT21_COMMAND_TRIGGER               0xAC
#define AHT21_GET_STATUS                    0x71
#define AHT21_STATUS_OK                     0x18
#define AHT21_MEASUREMENT_DATA0             0x33
#define AHT21_MEASUREMENT_DATA1             0x00

// esp_err_t i2c_master_init(void);
void aht21_init(void);
float aht21_getTemperature(void);
float aht21_getHumidity(void);

#endif