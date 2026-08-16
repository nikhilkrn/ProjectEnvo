cmake_minimum_required(VERSION 3.22)

idf_component_register(SRCS "bmp280_app.c"
                        REQUIRES driver
                    INCLUDE_DIRS "." "../../drivers/BMP280")