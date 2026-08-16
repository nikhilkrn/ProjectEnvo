
#include "driver/i2c.h"
#include "BMP280.h"

#include "esp_err.h"
#include "esp_log.h"

#define I2C_PORT I2C_NUM_0

void i2c_scan(void)
{
    ESP_LOGI("I2C_SCAN", "Scanning I2C bus...");
    
    int found = 0;

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        i2c_master_start(cmd);

        i2c_master_write_byte(cmd,(addr << 1) | I2C_MASTER_WRITE, true );

        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100) );

        i2c_cmd_link_delete(cmd);

        if (err == ESP_OK){
            ESP_LOGI("I2C_SCAN", "Device found at address: 0x%02X", addr);
            found++;
        }
    }

    if (found == 0){
        ESP_LOGI("I2C_SCAN", "No I2C devices found!");
    }
    else{
        ESP_LOGI("I2C_SCAN", "Scan complete. Found %d device(s).", found);
    }
}

esp_err_t i2c_init(){

    i2c_config_t config = {0};
    
    config.mode = I2C_MODE_MASTER;
    config.scl_io_num = GPIO_NUM_22;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.sda_io_num = GPIO_NUM_21;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = 100000;

    i2c_param_config(I2C_NUM_0,&config);

    return i2c_driver_install(I2C_NUM_0,I2C_MODE_MASTER,0,0,0);
}

esp_err_t i2c_master_write_to_sensor(uint8_t dev_id, uint8_t reg_addr ,uint8_t data){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);

    i2c_master_write_byte(cmd,dev_id << 1 | WRITE_REG, 0x01);
    i2c_master_write_byte(cmd,reg_addr, 0x01);
    i2c_master_write_byte(cmd,data, 0x01);
    i2c_master_stop(cmd);
    
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0,cmd,pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return err;
}

esp_err_t i2c_master_read_from_sensor(uint8_t dev_id,uint8_t reg_addr, uint8_t *output_data, uint8_t length ){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,dev_id << 1 | WRITE_REG,0x01);
    i2c_master_write_byte(cmd, reg_addr, 0x01);
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,dev_id << 1 | READ_REG ,0x01);
    i2c_master_read(cmd,output_data,length,I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0,cmd,pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return err;
}