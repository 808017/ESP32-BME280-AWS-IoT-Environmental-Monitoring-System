#include "bme280.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "BME280";


// ============================================================
// REGISTERS
// ============================================================

#define REG_ID          0xD0
#define REG_RESET       0xE0
#define REG_CTRL_HUM    0xF2
#define REG_STATUS      0xF3
#define REG_CTRL_MEAS   0xF4
#define REG_DATA        0xF7


// ============================================================
// DEVICE
// ============================================================

static i2c_master_dev_handle_t bme_handle;


// ============================================================
// CALIBRATION
// ============================================================

static uint16_t dig_T1;
static int16_t dig_T2;
static int16_t dig_T3;

static uint8_t dig_H1;
static int16_t dig_H2;
static uint8_t dig_H3;
static int16_t dig_H4;
static int16_t dig_H5;
static int8_t dig_H6;

static int32_t t_fine;


// ============================================================
// READ REGISTER
// ============================================================

static esp_err_t bme_read_reg(
    uint8_t reg,
    uint8_t *data,
    size_t len)
{
    return i2c_master_transmit_receive(
        bme_handle,
        &reg,
        1,
        data,
        len,
        100
    );
}


// ============================================================
// WRITE REGISTER
// ============================================================

static esp_err_t bme_write_reg(
    uint8_t reg,
    uint8_t value)
{
    uint8_t data[2] =
    {
        reg,
        value
    };

    return i2c_master_transmit(
        bme_handle,
        data,
        2,
        100
    );
}


// ============================================================
// CALIBRATION
// ============================================================

static esp_err_t read_calibration(void)
{
    uint8_t c1[26];
    uint8_t c2[7];

    esp_err_t ret;

    ret = bme_read_reg(
        0x88,
        c1,
        26
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = bme_read_reg(
        0xE1,
        c2,
        7
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    dig_T1 =
        (uint16_t)c1[0] |
        ((uint16_t)c1[1] << 8);

    dig_T2 =
        (int16_t)(
            (uint16_t)c1[2] |
            ((uint16_t)c1[3] << 8)
        );

    dig_T3 =
        (int16_t)(
            (uint16_t)c1[4] |
            ((uint16_t)c1[5] << 8)
        );

    dig_H1 = c1[25];

    dig_H2 =
        (int16_t)(
            (uint16_t)c2[0] |
            ((uint16_t)c2[1] << 8)
        );

    dig_H3 = c2[2];

    dig_H4 =
        (int16_t)(
            ((int16_t)c2[3] << 4) |
            (c2[4] & 0x0F)
        );

    dig_H5 =
        (int16_t)(
            ((int16_t)c2[5] << 4) |
            (c2[4] >> 4)
        );

    dig_H6 =
        (int8_t)c2[6];

    return ESP_OK;
}


// ============================================================
// TEMPERATURE COMPENSATION
// ============================================================

static float compensate_temperature(
    int32_t adc_T)
{
    float var1;
    float var2;

    var1 =
        (((float)adc_T / 16384.0f) -
         ((float)dig_T1 / 1024.0f))
        * (float)dig_T2;

    var2 =
        (((float)adc_T / 131072.0f) -
         ((float)dig_T1 / 8192.0f));

    var2 =
        var2 * var2 * (float)dig_T3;

    t_fine =
        (int32_t)(var1 + var2);

    return
        (var1 + var2) / 5120.0f;
}


// ============================================================
// HUMIDITY COMPENSATION
// ============================================================

static float compensate_humidity(
    int32_t adc_H)
{
    float var_H;

    var_H =
        ((float)t_fine - 76800.0f);

    var_H =
        (adc_H -
         ((float)dig_H4 * 64.0f +
          ((float)dig_H5 / 16384.0f) *
          var_H))
        *
        ((float)dig_H2 / 65536.0f *
         (1.0f +
          ((float)dig_H6 / 67108864.0f) *
          var_H *
          (1.0f +
           ((float)dig_H3 / 67108864.0f) *
           var_H)));

    var_H =
        var_H *
        (1.0f -
         ((float)dig_H1 *
          var_H /
          524288.0f));

    if (var_H > 100.0f)
        var_H = 100.0f;

    if (var_H < 0.0f)
        var_H = 0.0f;

    return var_H;
}


// ============================================================
// INIT
// ============================================================

esp_err_t bme280_init(
    i2c_master_bus_handle_t bus_handle)
{
    i2c_device_config_t config =
    {
        .dev_addr_length =
            I2C_ADDR_BIT_LEN_7,

        .device_address =
            BME280_ADDR,

        .scl_speed_hz =
            100000,
    };

    esp_err_t ret =
        i2c_master_bus_add_device(
            bus_handle,
            &config,
            &bme_handle
        );

    if (ret != ESP_OK)
        return ret;


    uint8_t chip_id;

    ret =
        bme_read_reg(
            REG_ID,
            &chip_id,
            1
        );

    if (ret != ESP_OK)
        return ret;


    ESP_LOGI(
        TAG,
        "Chip ID = 0x%02X",
        chip_id
    );


    if (chip_id != 0x60)
    {
        ESP_LOGE(
            TAG,
            "BME280 not detected"
        );

        return ESP_ERR_NOT_FOUND;
    }


    // Reset

    ret =
        bme_write_reg(
            REG_RESET,
            0xB6
        );

    if (ret != ESP_OK)
        return ret;


    vTaskDelay(
        pdMS_TO_TICKS(10)
    );


    // Calibration

    ret =
        read_calibration();

    if (ret != ESP_OK)
        return ret;


    // Humidity oversampling x1

    ret =
        bme_write_reg(
            REG_CTRL_HUM,
            0x02
        );

    if (ret != ESP_OK)
        return ret;


    ESP_LOGI(
        TAG,
        "BME280 initialized"
    );


    return ESP_OK;
}


// ============================================================
// READ SENSOR
// ============================================================

esp_err_t bme280_read(
    bme280_data_t *data)
{
    esp_err_t ret;

    // --------------------------------------------------------
    // Make sure humidity oversampling is enabled
    // --------------------------------------------------------

    ret = bme_write_reg(
        REG_CTRL_HUM,
        0x01
    );

    if (ret != ESP_OK)
        return ret;


    // --------------------------------------------------------
    // Start forced measurement
    //
    // Temperature x1
    // Pressure    x1
    // Forced mode
    // --------------------------------------------------------

    ret = bme_write_reg(
        REG_CTRL_MEAS,
        0x25
    );

    if (ret != ESP_OK)
        return ret;


    // --------------------------------------------------------
    // Give BME280 time to start conversion
    // --------------------------------------------------------

    vTaskDelay(
        pdMS_TO_TICKS(2)
    );


    // --------------------------------------------------------
    // Wait until measurement is finished
    // --------------------------------------------------------

    uint8_t status = 0;

    bool measurement_done = false;


    for (int i = 0; i < 100; i++)
    {
        ret = bme_read_reg(
            REG_STATUS,
            &status,
            1
        );

        if (ret != ESP_OK)
            return ret;


        // Bit 3 = measuring
        //
        // 1 = measurement running
        // 0 = measurement finished

        if ((status & 0x08) == 0)
        {
            measurement_done = true;
            break;
        }


        vTaskDelay(
            pdMS_TO_TICKS(1)
        );
    }


    if (!measurement_done)
    {
        ESP_LOGE(
            TAG,
            "BME280 measurement timeout"
        );

        return ESP_ERR_TIMEOUT;
    }


    // --------------------------------------------------------
    // Small delay before reading
    // --------------------------------------------------------

    vTaskDelay(
        pdMS_TO_TICKS(2)
    );


    // --------------------------------------------------------
    // Read fresh temperature + humidity
    // --------------------------------------------------------

    uint8_t raw[8];

    ret = bme_read_reg(
        REG_DATA,
        raw,
        8
    );

    if (ret != ESP_OK)
        return ret;


    // --------------------------------------------------------
    // RAW TEMPERATURE
    // --------------------------------------------------------

    int32_t adc_T =
        ((int32_t)raw[3] << 12) |
        ((int32_t)raw[4] << 4) |
        ((int32_t)raw[5] >> 4);


    // --------------------------------------------------------
    // RAW HUMIDITY
    // --------------------------------------------------------

    int32_t adc_H =
        ((int32_t)raw[6] << 8) |
        raw[7];


    // --------------------------------------------------------
    // DEBUG
    // --------------------------------------------------------

    ESP_LOGI(
        TAG,
        "RAW T=%ld H=%ld",
        (long)adc_T,
        (long)adc_H
    );


    // --------------------------------------------------------
    // COMPENSATION
    // --------------------------------------------------------

    data->temperature =
        compensate_temperature(
            adc_T
        );


    data->humidity =
        compensate_humidity(
            adc_H
        );


    ESP_LOGI(
        TAG,
        "T=%.2f C H=%.2f %%",
        data->temperature,
        data->humidity
    );


    return ESP_OK;
}