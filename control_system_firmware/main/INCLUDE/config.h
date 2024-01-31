#ifndef __CONFIG__
#define __CONFIG__

#include "driver/i2c.h"

#define UART_TXD_PIN                                           (10)
#define UART_RXD_PIN                                           (3)
#define UART_PORT_NUM                                          UART_NUM_1
#define UART_BAUD_RATE                                         (115200)
#define TX_BUF_SIZE                                            (256)
#define RX_BUF_SIZE                                            (1024) 

#define I2C_MASTER_SDA_PIN                                     (5)
#define I2C_MASTER_SCL_PIN                                     (4)
#define I2C_MASTER_NUM                                         (0)
#define I2C_MASTER_FREQ_HZ                                     (400000)
#define I2C_MASTER_TX_BUF_DISABLE                              (0)
#define I2C_MASTER_RX_BUF_DISABLE                              (0)
#define I2C_MASTER_TIMEOUT                                     (1000)
#define ACK_VAL                                                (0x0)
#define NACK_VAL                                               (0x1)
#define ACK_CHECK_EN                                           (0x1)
#define ACK_CHECK_DIS                                          (0x0)
#define I2C_WRITE_BIT                                          I2C_MASTER_WRITE
#define I2C_READ_BIT                                           I2C_MASTER_READ

#define RELAY_1_PIN                                             (9)
#define RELAY_2_PIN                                             (8)
#define RELAY_3_PIN                                             (7)
#define RELAY_4_PIN                                             (6)
#define GPIO_OUTPUT_PIN_SEL                                     ((1ULL<<RELAY_1_PIN) | (1ULL<<RELAY_2_PIN) | (1ULL<<RELAY_3_PIN) | (1ULL<<RELAY_4_PIN))

#define LED_NUMBER                                              (5)
#define PIXEL_SIZE                                              (12) // each colour takes 4 bytes
#define SAMPLE_RATE                                             (93750) //93750
#define ZERO_BUFFER                                             (60)
#define I2S_NUM                                                 (0)
#define I2S_DO_IO                                               (GPIO_NUM_1)
#define I2S_DI_IO                                               (-1)

#define RMT_TX_CHANNEL                                          RMT_CHANNEL_0
#define RMT_TX_GPIO_NUM                                         (0)
#define RMT_TX_CARRIER_EN                                       1;

#define NVS_NAMESPACE_CONFIG                                    "CONFIG"
#define NVS_KEY_WIFI_SSID                                       "WSSID"
#define NVS_KEY_WIFI_PASS                                       "WPASS"

#define NVS_NAMESPACE_OTA                                       "OTA"
#define NVS_KEY_IP_URL                                          "IP"

//hihi
#define DEFAULT_SSID                                            "HOANG DAT"
//123456890
#define DEFAULT_PASS                                            "24072002"
#define AWS_URI                                                 "mqtts://a3suuuxay09k3c-ats.iot.ap-southeast-2.amazonaws.com"
#define ESP_SOFT_RESET                                          "espreset"

#define DEFAULT_OTA_IP                                          "192.168.1.6"


#endif