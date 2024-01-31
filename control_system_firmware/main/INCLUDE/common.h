#ifndef __COMMON__
#define __COMMON__

#include "stdint.h"
#include "config.h"
#include "../WS2812B/ws2812b.h"

#define FW_VERSION_STR MKSTR(FW_VERSION_MAJOR)\
                       MKSTR(FW_VERSION_MINOR)\
                       MKSTR(FW_VERSION_BUILD) 

typedef struct
{
    char* WifiName;
    char* WifiPass;
    char DevIP[20];
}WifiData_t;

WifiData_t WifiData;

typedef struct
{
    bool isWifiConnected;
    bool isNextion;
}SetUp_t;

SetUp_t SetUp;

typedef struct
{
    float temperature;       
    float humid;       
    uint16_t eCo2;  
    uint16_t tvoc;
    uint16_t  aqi
}SensorData_t;

SensorData_t SensorData;

typedef enum {
    AC_OFF, 
    AC_ON, 
    AC_AUTO,
    AC_COOL_16 = 16,
    AC_COOL_17,
    AC_COOL_18,
    AC_COOL_19,
    AC_COOL_20,
    AC_COOL_21,
    AC_COOL_22,
    AC_COOL_23,
    AC_COOL_24,
    AC_COOL_25,
    AC_COOL_26,  
    AC_COOL_27,
    AC_COOL_28,
    AC_COOL_29,
    AC_COOL_30,  
} acState_t;

typedef struct
{
    bool isLedOn;
    bool isFanOn;
    bool isAirOn;
    bool isDoorOn;
    bool isAirConOn;
    struct airConSetting_t{
        bool isAutoMode;
        bool isCoolMode;
        uint16_t acTemp;
        acState_t acState;
    } airConSetting;
}device_t;

device_t device;

typedef struct
{
    char* data;
    uint16_t num;
    uint16_t F_G1;
    uint16_t F_G2;
    uint16_t F_G3;
    uint16_t F_G4;
    uint16_t M_G1;
    uint16_t M_G2;
    uint16_t M_G3;
    uint16_t M_G4;
    uint16_t maleNum;
    uint16_t femaleNum;
    uint16_t G1Num;
    uint16_t G2Num;
    uint16_t G3Num;
    uint16_t G4Num;
}CameraData_t;

CameraData_t CameraData;

typedef struct
{
    bool isAutoMode;
    bool isLedOn;
    bool isFanOn;
    bool isAirOn;
    bool isDoorOn;
    bool isAirConOn;
    struct ACSetting_t{
        bool isAutoMode;
        bool isCoolMode;
        uint16_t acTemp;
        acState_t acState;
    } ACSetting;

    struct wifiSetting_t{
        bool isWifiSet;
        char* WifiName;
        char* WifiPass;
    } wifiSetting;

    bool isUpdate;
}mobileData_t;

mobileData_t mobileData;

typedef struct
{
    bool center;
    bool wifi;
    bool fw;
}page_t;

page_t page;

typedef struct
{
    bool isUpdate;
    char* ip;
}OTA_t;

OTA_t OTA;

typedef struct
{
    bool autoMode;
    bool isNextionContact;
}deviceMode_t;

deviceMode_t deviceMode;

typedef struct
{
    bool isLed1On;
    bool isLed2On;
    bool isLed3On;
    bool isLed4On;
    bool isLed5On;
}ledState_t;

ledState_t ledState;

ws2812_pixel_t led[5];

#endif
