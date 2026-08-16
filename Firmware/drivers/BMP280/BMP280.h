
#ifndef BMP280_H_
#define BMP280_H_   

#include<stdint.h>
#include "BMP280_error.h"
#include "esp_err.h"

#define BMP280_CHIP_ID         0x58
#define BMP280_I2C_ADDR    0x76

#define BMP280_TEMP_XLSB       0xFC
#define BMP280_TEMP_LSB        0xFB
#define BMP280_TEMP_MSB        0xFA
#define BMP280_PRESS_XLSB      0xF9
#define BMP280_PRESS_LSB       0xF8
#define BMP280_PRESS_MSB       0xF7
#define BMP280_CONFIG          0xF5
#define BMP280_CTRL_MEAS       0xF4
#define BMP280_STATUS          0xF3
#define BMP280_RESET           0xE0
#define BMP280_DEV_ID_REG      0xD0
#define BMP280_CALIBRATION_T1  0x88
#define BMP280_CALIBRATION_T2  0x8A
#define BMP280_CALIBRATION_T3  0x8C
#define BMP280_CALIBRATION_P1  0x8E
#define BMP280_CALIBRATION_P2  0x90
#define BMP280_CALIBRATION_P3  0x92
#define BMP280_CALIBRATION_P4  0x94
#define BMP280_CALIBRATION_P5  0x96
#define BMP280_CALIBRATION_P6  0x98
#define BMP280_CALIBRATION_P7  0x9A
#define BMP280_CALIBRATION_P8  0x9C
#define BMP280_CALIBRATION_P9  0x9E


#define BMP280_OVERSAMPLING_SKIPPED        0x000
#define BMP280_OVERSAMPLING_X_1            0x001
#define BMP280_OVERSAMPLING_X_2            0x002
#define BMP280_OVERSAMPLING_X_4            0x003
#define BMP280_OVERSAMPLING_X_8            0x004
#define BMP280_OVERSAMPLING_X_16           0x005



#define BMP280_NORMAL_MODE                 0x03
#define BMP280_FORCED_MODE                 0x01
#define BMP280_SLEEP_MODE                  0x00

#define BMP280_STANDBY_TIME_50MS            0x000
#define BMP280_STANDBY_TIME_62_5MS          0x001
#define BMP280_STANDBY_TIME_125MS           0x002
#define BMP280_STANDBY_TIME_250MS           0x003
#define BMP280_STANDBY_TIME_500MS           0x004
#define BMP280_STANDBY_TIME_1000MS          0x005
#define BMP280_STANDBY_TIME_2000MS          0x006
#define BMP280_STANDBY_TIME_4000MS          0x007

#define BMP280_RESET_VALUE                  0xB6


#define BMP280_DISABLE_SPI                 (0x00 << 0)

#define READ_REG                     0x01
#define WRITE_REG                    0x00


typedef struct
{
    uint8_t mode;
    uint16_t sampling_time_normal;
    uint8_t spi_mode;
    uint16_t filter;
    uint16_t oversamp_temp;
    uint16_t oversamp_press;   
}bmp280_config_t;


typedef struct{
    float temperature;
    float pressure;
}bmp280_data_t;

typedef struct{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;

}calibration_data_t;

bmp280_error_code_t bmp280_init();
esp_err_t i2c_init();
bmp280_error_code_t bmp280_setConfig();
bmp280_error_code_t bmp280_readtemp();
bmp280_error_code_t bmp280_readPressure();

#endif