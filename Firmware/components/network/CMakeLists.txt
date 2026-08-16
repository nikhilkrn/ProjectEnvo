cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "wifi_handler.c"
                    REQUIRES esp_wifi esp_http_server nvs_flash esp_netif esp_event
                    INCLUDE_DIRS ".")