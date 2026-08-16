# ProjectEnvo

ProjectEnvo is an embedded environmental monitoring project built around an ESP32 and environmental sensors. The project is being developed as a modular firmware system, with hardware drivers kept separate from application-level sensor logic.

> **Current status:** The BMP280 sensor driver has been implemented and tested. Other parts of the environmental monitoring system are still under development.

## Current Features

### BMP280 Driver

- TemThe current firmware includes a custom BMP280 driver for communication over I2C.

Implemented functionality:

- I2C communication with the BMP280
- I2C device scanning
- BMP280 chip ID verification
- Sensor configuration
- Factory calibration data reading
- 20-bit raw temperature data extraction
- Temperature compensation
- `t_fine` calculation
- 20-bit raw pressure data extraction
- Pressure compensation
perature output in °C
- Pressure output in Pa / hPa

The driver has been tested with a BMP280 returning:

```text
Chip ID: 0x58
I2C Address: 0x76
```

Example measured output:

```text
Temperature: 31.95 °C
Pressure:    100203 Pa
             1002.03 hPa
```

## Project Structure

The project is organized to keep hardware drivers separate from higher-level application components.

```text
ProjectEnvo/
├── components/
│   └── sensors/
│       └── ...
│
├── drivers/
│   └── BMP280/
│       ├── BMP280.c
│       ├── BMP280.h
│       └── BMP280_error.h
│
├── main/
│   ├── main.c
│   └── CMakeLists.txt
│
├── CMakeLists.txt
└── README.md
```

The exact project structure may evolve as additional drivers and application modules are added.

## Hardware

Current hardware used for development:

- ESP32
- BMP280 environmental sensor
- I2C interface

### I2C Connections

The current ESP32 I2C configuration uses:

| Signal | ESP32 GPIO |
|---|---:|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| BMP280 I2C Address | `0x76` |

The I2C bus is currently configured for **100 kHz** operation.

## BMP280 Driver Flow

The driver follows this general initialization and measurement flow:

```text
ESP32
  │
  ├── Initialize I2C
  │
  ├── Check BMP280 chip ID
  │
  ├── Read factory calibration data
  │      ├── dig_T1 ... dig_T3
  │      └── dig_P1 ... dig_P9
  │
  ├── Configure BMP280
  │      ├── Temperature oversampling
  │      ├── Pressure oversampling
  │      ├── Operating mode
  │      └── Standby/filter configuration
  │
  ├── Read raw temperature
  │
  ├── Temperature compensation
  │      └── Calculate t_fine
  │
  ├── Read raw pressure
  │
  └── Pressure compensation
         └── Calculate pressure
```

## Calibration

The BMP280 contains factory calibration coefficients that are read from its internal calibration registers.

The driver currently reads the calibration block starting from:

```text
0x88
```

The temperature calibration coefficients are:

```text
dig_T1
dig_T2
dig_T3
```

and the pressure calibration coefficients are:

```text
dig_P1
dig_P2
dig_P3
dig_P4
dig_P5
dig_P6
dig_P7
dig_P8
dig_P9
```

These coefficients are required for converting the raw ADC measurements into compensated temperature and pressure values.

## Temperature Measurement

The BMP280 temperature measurement is stored as a 20-bit value across three registers.

The driver reconstructs it as:

```c
raw_temperature =
    ((uint32_t)temp_msb << 12) |
    ((uint32_t)temp_lsb << 4)  |
    ((uint32_t)temp_xlsb >> 4);
```

The raw value is then compensated using `dig_T1`, `dig_T2`, and `dig_T3`.

The resulting intermediate value, `t_fine`, is also required by the pressure compensation algorithm.

## Pressure Measurement

Pressure is also read as a 20-bit raw ADC value from three registers:

```c
raw_pressure =
    ((uint32_t)press_msb << 12) |
    ((uint32_t)press_lsb << 4)  |
    ((uint32_t)press_xlsb >> 4);
```

The compensated pressure is calculated using:

```text
t_fine
dig_P1 ... dig_P9
raw_pressure
```

The driver stores pressure internally in **Pa**. It can be converted to hPa using:

```c
pressure_hPa = pressure_Pa / 100.0;
```

## Current Configuration

The BMP280 is currently configured in normal mode with:

```text
Temperature oversampling : ×4
Pressure oversampling    : ×2
I2C clock                : 100 kHz
```

The current configuration register values observed during testing are:

```text
CTRL_MEAS = 0x6B
CONFIG    = 0x80
```

## I2C Communication

The project currently uses the ESP-IDF legacy I2C API:

```c
#include "driver/i2c.h"
```

The I2C abstraction provides functions for:

- Initializing the I2C peripheral
- Writing a register to a sensor
- Reading one or more registers from a sensor

For multi-byte reads, the final byte is terminated using a NACK so that the sensor knows the master has finished reading.

## Validation

The BMP280 driver has been tested for:

- Device detection on the I2C bus
- Correct chip ID detection
- Register write/read verification
- Calibration data retrieval
- Changing raw temperature values
- Temperature compensation
- Changing raw pressure values
- Pressure compensation

Observed sensor output was stable under stationary conditions and responded to environmental changes such as placing the sensor near a warm laptop.

## Current Development Status

### Completed

- [x] ESP32 I2C initialization
- [x] I2C bus scanning
- [x] BMP280 device detection
- [x] BMP280 chip ID verification
- [x] BMP280 register read/write functions
- [x] BMP280 configuration
- [x] Calibration data extraction
- [x] Temperature raw-data extraction
- [x] Temperature compensation
- [x] `t_fine` calculation
- [x] Pressure raw-data extraction
- [x] Pressure compensation
- [x] Basic temperature and pressure validation

### Planned

The rest of ProjectEnvo is still under development. Planned work may include:

- [ ] Additional environmental sensor drivers
- [ ] Sensor data processing
- [ ] Configurable sampling rates
- [ ] Data storage
- [ ] Network connectivity
- [ ] MQTT/data transmission
- [ ] System monitoring and diagnostics
- [ ] Power/battery monitoring
- [ ] Complete environmental monitoring application

## Building the Project

This project uses **ESP-IDF**.

After setting up the ESP-IDF environment:

```bash
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

## Notes

The BMP280 measures:

- Temperature
- Atmospheric pressure

It **does not measure humidity**. Humidity support would require a humidity-capable sensor such as the BME280 or a separate humidity sensor.

---

## License

Add the project's license information here when the project license is decided.
