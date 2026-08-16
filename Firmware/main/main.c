#include <stdio.h>
#include "wifi_hnadler.h"
#include "nvs_flash.h"
#include "BMP280.h"
#include "bmp280_app.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{   
    // ESP_ERROR_CHECK(nvs_flash_init());
    // Wifi_init();
    i2c_init();
    i2c_scan();
    bmp280_init();
    bmp280_setConfig();

    while(1){
        bmp280_readtemp();
        bmp280_readPressure();
        vTaskDelay(pdMS_TO_TICKS(1010));
    }
    
}
