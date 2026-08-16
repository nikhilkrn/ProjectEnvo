#include "BMP280.h"
#include "BMP280_error.h"
#include "bmp280_app.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define BMP280_CHECK_ERROR(error) \
        do{\
            if(error != BMP280_OK) \
            {\
                ESP_LOGI("BMP280","Error Occured with code: (%d)",error);\
                return error; \
            } \
        }while(0)


bmp280_config_t bmp280_config;
bmp280_error_code_t bmp280_err;
calibration_data_t bmp280_calibration_data;

bmp280_data_t bmp280_data;

int32_t t_fine;

const char *tag = "BMP280";


bmp280_error_code_t bmp280_calibrate(){
    uint8_t calibration_data[24];
    
    bmp280_err = i2c_master_read_from_sensor(BMP280_I2C_ADDR,BMP280_CALIBRATION_T1,calibration_data,sizeof(calibration_data));
    BMP280_CHECK_ERROR(bmp280_err);

    bmp280_calibration_data.dig_t1 = ((uint16_t)calibration_data[1]<<8) | calibration_data[0];
    bmp280_calibration_data.dig_t2 = (int16_t)(((uint16_t)calibration_data[3]<<8) | calibration_data[2]);
    bmp280_calibration_data.dig_t3 = (int16_t)(((uint16_t)calibration_data[5]<<8) | calibration_data[4]);
    bmp280_calibration_data.dig_p1 = (((uint16_t)calibration_data[7]<<8) | calibration_data[6]);
    bmp280_calibration_data.dig_p2 = (int16_t)(((uint16_t)calibration_data[9]<<8) | calibration_data[8]);
    bmp280_calibration_data.dig_p3 = (int16_t)(((uint16_t)calibration_data[11]<<8) | calibration_data[10]);
    bmp280_calibration_data.dig_p4 = (int16_t)(((uint16_t)calibration_data[13]<<8) | calibration_data[12]);
    bmp280_calibration_data.dig_p5 = (int16_t)(((uint16_t)calibration_data[15]<<8) | calibration_data[14]);
    bmp280_calibration_data.dig_p6 = (int16_t)(((uint16_t)calibration_data[17]<<8) | calibration_data[16]);
    bmp280_calibration_data.dig_p7 = (int16_t)(((uint16_t)calibration_data[19]<<8) | calibration_data[18]);
    bmp280_calibration_data.dig_p8 = (int16_t)(((uint16_t)calibration_data[21]<<8) | calibration_data[20]);
    bmp280_calibration_data.dig_p9 = (int16_t)(((uint16_t)calibration_data[23]<<8) | calibration_data[22]);
    
    printf("T1 = %u\n", bmp280_calibration_data.dig_t1);
    printf("T2 = %d\n", bmp280_calibration_data.dig_t2);
    printf("T3 = %d\n", bmp280_calibration_data.dig_t3);

    printf("P1 = %u\n", bmp280_calibration_data.dig_p1);
    printf("P2 = %d\n", bmp280_calibration_data.dig_p2);
    printf("P3 = %d\n", bmp280_calibration_data.dig_p3);

    return bmp280_err;
}


bmp280_error_code_t bmp280_init(){

    uint8_t chip_id;

    bmp280_err = i2c_master_read_from_sensor(BMP280_I2C_ADDR,BMP280_DEV_ID_REG,&chip_id,sizeof(chip_id));
    BMP280_CHECK_ERROR(bmp280_err);

    ESP_LOGI(tag,"CHIP_ID: 0x%02X",chip_id);

    if(chip_id != BMP280_CHIP_ID){
        ESP_LOGE(tag,"Chip Id not matched");
        return BMP280_CHIPID_MISMATCH;
    }

    bmp280_err = i2c_master_write_to_sensor(BMP280_I2C_ADDR,BMP280_RESET,BMP280_RESET_VALUE);
    BMP280_CHECK_ERROR(bmp280_err);
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_LOGI(tag,"Reset Done");

    bmp280_config.mode = BMP280_NORMAL_MODE;
    bmp280_config.spi_mode = BMP280_DISABLE_SPI;
    bmp280_config.oversamp_press = BMP280_OVERSAMPLING_X_2;
    bmp280_config.oversamp_temp = BMP280_OVERSAMPLING_X_4;
    bmp280_config.sampling_time_normal = BMP280_STANDBY_TIME_1000MS;


    bmp280_err = bmp280_calibrate();
    BMP280_CHECK_ERROR(bmp280_err);

    ESP_LOGI(tag,"Calibration Done");

    ESP_LOGI(tag,"Sensor Init Done");
    return BMP280_OK;
}

bmp280_error_code_t bmp280_setConfig(){

    uint8_t ctrl_meas_reg = (bmp280_config.oversamp_temp << 5 | bmp280_config.oversamp_press <<2 | bmp280_config.mode);

    bmp280_err = i2c_master_write_to_sensor(BMP280_I2C_ADDR,BMP280_CTRL_MEAS, ctrl_meas_reg);

    BMP280_CHECK_ERROR(bmp280_err);

    uint8_t read_config;
    i2c_master_read_from_sensor(BMP280_I2C_ADDR,BMP280_CTRL_MEAS,&read_config,sizeof(read_config));
    ESP_LOGI(tag,"Write Config: 0x%02X",ctrl_meas_reg);
    ESP_LOGI(tag,"READ CONFIG: 0x%02X",read_config);


    uint8_t config_reg = (bmp280_config.sampling_time_normal << 5 | bmp280_config.filter <<2 | bmp280_config.spi_mode);

    bmp280_err = i2c_master_write_to_sensor(BMP280_I2C_ADDR,BMP280_CONFIG, config_reg);

    BMP280_CHECK_ERROR(bmp280_err);

    i2c_master_read_from_sensor(BMP280_I2C_ADDR,BMP280_CONFIG,&read_config,sizeof(read_config));
    ESP_LOGI(tag,"Write Config: 0x%02X",config_reg);
    ESP_LOGI(tag,"READ CONFIG: 0x%02X",read_config);

    ESP_LOGI(tag,"Configuratation Done");

    return bmp280_err;
}



bmp280_error_code_t bmp280_readtemp(){

    uint8_t temp_data[3];

    bmp280_err = i2c_master_read_from_sensor(BMP280_I2C_ADDR,BMP280_TEMP_MSB,temp_data,sizeof(temp_data));

    BMP280_CHECK_ERROR(bmp280_err);
    int32_t raw_data = (int32_t)((uint8_t)temp_data[0] << 12 | (uint8_t)temp_data[1] << 4 | (uint8_t)temp_data[2] >> 4);

    printf("Temperature_Raw: %ld\n",raw_data);
    
    double var1,var2,temparature;

    var1 = (((double)raw_data)/16384.0 - ((double)bmp280_calibration_data.dig_t1)/1024.0) * ((double)bmp280_calibration_data.dig_t2);
    var2 = ((((double)raw_data)/131072.0 - ((double)bmp280_calibration_data.dig_t1)/8192.0) * (((double)raw_data)/131072.0 - ((double) bmp280_calibration_data.dig_t1)/8192.0)) * ((double)bmp280_calibration_data.dig_t3);
    
    t_fine = var1 + var2;

    temparature = (t_fine) / 5120.0;
    bmp280_data.temperature = temparature;

    printf("Temperature: %.2f\n",temparature);

    return BMP280_OK;

}



bmp280_error_code_t bmp280_readPressure(){
    uint8_t press_data[3];

    bmp280_err = i2c_master_read_from_sensor(BMP280_I2C_ADDR,BMP280_PRESS_MSB,press_data,sizeof(press_data));
    BMP280_CHECK_ERROR(bmp280_err);

    uint32_t press_raw_data = (uint8_t)press_data[0] << 12 | (uint8_t)press_data[1]<< 4 | (uint8_t)press_data[2] >> 4;
    printf("RAW Pressure: %ld\n",press_raw_data);

    double var1,var2,pressure;

    var1 = ((double)t_fine/2.0) - 64000.0; 
    var2 = var1 * var1 * ((double)bmp280_calibration_data.dig_p6) / 32768.0; 
    var2 = var2 + var1 * ((double)bmp280_calibration_data.dig_p5) * 2.0; 
    var2 = (var2/4.0)+(((double)bmp280_calibration_data.dig_p4) * 65536.0); 
    var1 = (((double)bmp280_calibration_data.dig_p3) * var1 * var1 / 524288.0 + ((double)bmp280_calibration_data.dig_p2) * var1) / 524288.0; 
    var1 = (1.0 + var1 / 32768.0)*((double)bmp280_calibration_data.dig_p1); 
    if (var1 == 0.0) { 
        pressure = 0;
        bmp280_data.pressure = pressure;
        return BMP280_OK; // avoid exception caused by division by zero 
    } 
    pressure = 1048576.0 - (double)press_raw_data; 
    pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1; 
    var1 = ((double)bmp280_calibration_data.dig_p9) * pressure * pressure / 2147483648.0; 
    var2 = pressure * ((double)bmp280_calibration_data.dig_p8) / 32768.0; 
    pressure = pressure + (var1 + var2 + ((double)bmp280_calibration_data.dig_p7)) / 16.0;

    printf("Pressure: %.2f hPa\n", (pressure/100.0));
    bmp280_data.pressure = pressure;

    return bmp280_err;

}