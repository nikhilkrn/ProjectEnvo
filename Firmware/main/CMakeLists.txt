idf_component_register(SRCS "main.c" 
# "../drivers/BMP280/BMP280.c"
                    INCLUDE_DIRS "."
                    #  "../drivers/BMP280"
                    REQUIRES network BMP280)
