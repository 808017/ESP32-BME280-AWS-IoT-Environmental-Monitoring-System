#ifndef BME280_H
#define BME280_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#define BME280_ADDR 0x76

typedef struct
{
    float temperature;
    float humidity;
} bme280_data_t;

esp_err_t bme280_init(
    i2c_master_bus_handle_t bus_handle
);

esp_err_t bme280_read(
    bme280_data_t *data
);

#endif