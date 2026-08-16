
#ifndef BMP280_APP_H_
#define BMP280_APP_H_

#include "esp_err.h"


esp_err_t i2c_master_write_to_sensor(uint8_t dev_id, uint8_t reg_addr ,uint8_t data);

esp_err_t i2c_master_read_from_sensor(uint8_t dev_id,uint8_t reg_addr, uint8_t *output_data, uint8_t length );

void i2c_scan(void);

#endif