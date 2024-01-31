#ifndef __ENS_DRIVER__
#define __ENS_DRIVER__

#include "esp_err.h"
#include "driver/i2c.h"

#define ENS160_PARTID                       0x0160
#define ENS160_I2C_ADDR_0          	        0x52		//ADDR low
#define ENS160_I2C_ADDR_1          	        0x53		//ADDR high
#define ENS160_BOOTING                      10
#define ENS160_1_BYTE_SIZE                  8
#define ENS160_2_BYTE_SIZE                  8*2

#define ENS160_DATA_STATUS_NEWDAT	        0x02
#define ENS160_DATA_STATUS_NEWGPR	        0x01
#define IS_NEWDAT(x) 			            (ENS160_DATA_STATUS_NEWDAT == (ENS160_DATA_STATUS_NEWDAT & (x)))
#define IS_NEWGPR(x) 			            (ENS160_DATA_STATUS_NEWGPR == (ENS160_DATA_STATUS_NEWGPR & (x)))
#define IS_NEW_DATA_AVAILABLE(x) 	        (0 != ((ENS160_DATA_STATUS_NEWDAT | ENS160_DATA_STATUS_NEWGPR ) & (x)))

#define ENS160_REG_PARTID                   0x00        //2 byte reg
#define ENS160_REG_OPMODE                   0x10 
#define ENS160_REG_COFIG                    0x11
#define ENS160_REG_COMMAND                  0x12
#define ENS160_REG_TEMP_IN                  0x13
#define ENS160_REG_RH_IN                    0x15
#define ENS160_REG_DATA_STATUS              0x20
#define ENS160_REG_DATA_AQI                 0x21
#define ENS160_REG_DATA_TVOC                0x22
#define ENS160_REG_DATA_ECO2                0x24
#define ENS160_REG_DATA_T                   0x30
#define ENS160_REG_DATA_RH                  0x32                                             
#define ENS160_REG_DATA_MISR		        0x38
#define ENS160_REG_GPR_WRITE_0              0x40
#define ENS160_REG_GPR_WRITE_1              ENS160_REG_GPR_WRITE_0 + 1
#define ENS160_REG_GPR_WRITE_2              ENS160_REG_GPR_WRITE_0 + 2
#define ENS160_REG_GPR_WRITE_3              ENS160_REG_GPR_WRITE_0 + 3
#define ENS160_REG_GPR_WRITE_4              ENS160_REG_GPR_WRITE_0 + 4
#define ENS160_REG_GPR_WRITE_5              ENS160_REG_GPR_WRITE_0 + 5
#define ENS160_REG_GPR_WRITE_6              ENS160_REG_GPR_WRITE_0 + 6
#define ENS160_REG_GPR_WRITE_7              ENS160_REG_GPR_WRITE_0 + 7
#define ENS160_REG_GPR_READ_0               0x48
#define ENS160_REG_GPR_READ_1               ENS160_REG_GPR_READ_0 + 1
#define ENS160_REG_GPR_READ_2               ENS160_REG_GPR_READ_0 + 2
#define ENS160_REG_GPR_READ_3               ENS160_REG_GPR_READ_0 + 3
#define ENS160_REG_GPR_READ_4               ENS160_REG_GPR_READ_0 + 4
#define ENS160_REG_GPR_READ_5               ENS160_REG_GPR_READ_0 + 5
#define ENS160_REG_GPR_READ_6               ENS160_REG_GPR_READ_0 + 6
#define ENS160_REG_GPR_READ_7               ENS160_REG_GPR_READ_0 + 7

#define ENS160_OPMODE_DEEP_SLEEP            0x00
#define ENS160_OPMODE_IDLE                  0x01
#define ENS160_OPMODE_STANDARD              0x02

#define ENS160_COMMAND_NOP		            0x00
#define ENS160_COMMAND_CLRGPR		        0xCC
#define ENS160_COMMAND_GET_APPVER	        0x0E 

esp_err_t i2c_master_init(void);
void ens160_init(void);
void ens160_measure(void);
uint16_t ens160_getAQI(void);
uint16_t ens160_getTVOC(void);
uint16_t ens160_geteCO2(void);

#endif