
#ifndef BMP280_ERROR_H_
#define BMP280_ERROR_H_

typedef enum {
    BMP280_OK = 0,
    BMP280_ERROR,
    BMP280_CHIPID_MISMATCH,
    BMP280_I2C_ADDR_ERROR,
}bmp280_error_code_t;

#endif