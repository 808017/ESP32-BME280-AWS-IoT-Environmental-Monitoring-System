#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "nvs_flash.h"

#include "esp_log.h"

#include "driver/i2c_master.h"

#include "bme280.h"
#include "lcd.h"
#include "wifi_manager.h"
#include "aws_iot.h"


// ============================================================
// CONFIG
// ============================================================

#define SDA_GPIO       21
#define SCL_GPIO       22

// Historical data → DynamoDB
#define CLOUD_INTERVAL_MS   15000

// Live dashboard → AWS IoT
#define LIVE_INTERVAL_MS     1000

#define LIVE_TOPIC            "bme280/live"


static const char *TAG = "MAIN";

static QueueHandle_t sensor_queue;


// ============================================================
// AWS HISTORY CLOUD TASK
// ============================================================
// Publishes every 15 seconds to:
//     bme280/data
//
// Existing IoT Rule:
//     bme280/data
//
// Existing DynamoDB:
//     BME280SensorData
// ============================================================

static void cloud_task(void *arg)
{
    bme280_data_t data;

    while (1)
    {
        // ----------------------------------------------------
        // Wait 15 seconds
        // ----------------------------------------------------

        vTaskDelay(
            pdMS_TO_TICKS(
                CLOUD_INTERVAL_MS
            )
        );


        // ----------------------------------------------------
        // Check Wi-Fi
        // ----------------------------------------------------

        if (!wifi_manager_is_connected())
        {
            ESP_LOGW(
                TAG,
                "WiFi offline"
            );

            continue;
        }


        // ----------------------------------------------------
        // Get latest sensor data
        // ----------------------------------------------------

        if (
            xQueuePeek(
                sensor_queue,
                &data,
                0
            ) != pdTRUE
        )
        {
            ESP_LOGW(
                TAG,
                "No sensor data available"
            );

            continue;
        }


        // ----------------------------------------------------
        // Check AWS connection
        // ----------------------------------------------------

        if (!aws_iot_is_connected())
        {
            ESP_LOGW(
                TAG,
                "AWS IoT not connected"
            );

            continue;
        }


        // ----------------------------------------------------
        // Publish historical data
        // ----------------------------------------------------

        bool ret =
            aws_iot_publish(
                data.temperature,
                data.humidity
            );


        if (ret)
        {
            ESP_LOGI(
                TAG,
                "AWS history upload OK"
            );
        }
        else
        {
            ESP_LOGW(
                TAG,
                "AWS history upload failed"
            );
        }
    }
}


// ============================================================
// LIVE DASHBOARD TASK
// ============================================================
// Publishes latest sensor reading every 1 second to:
//
//     bme280/live
//
// This topic will later be used by the web dashboard
// for near-real-time updates.
// ============================================================

static void live_task(void *arg)
{
    bme280_data_t data;

    while (1)
    {
        // ----------------------------------------------------
        // Wait 1 second
        // ----------------------------------------------------

        vTaskDelay(
            pdMS_TO_TICKS(
                LIVE_INTERVAL_MS
            )
        );


        // ----------------------------------------------------
        // Check Wi-Fi
        // ----------------------------------------------------

        if (!wifi_manager_is_connected())
        {
            continue;
        }


        // ----------------------------------------------------
        // Get latest sensor data
        // ----------------------------------------------------

        if (
            xQueuePeek(
                sensor_queue,
                &data,
                0
            ) != pdTRUE
        )
        {
            continue;
        }


        // ----------------------------------------------------
        // Check AWS connection
        // ----------------------------------------------------

        if (!aws_iot_is_connected())
        {
            continue;
        }


        // ----------------------------------------------------
        // Publish live data
        // ----------------------------------------------------

        bool ret =
            aws_iot_publish_topic(
                LIVE_TOPIC,
                data.temperature,
                data.humidity
            );


        if (!ret)
        {
            ESP_LOGW(
                TAG,
                "Live publish failed"
            );
        }
    }
}


// ============================================================
// MAIN
// ============================================================

void app_main(void)
{
    // ========================================================
    // NVS
    // ========================================================

    esp_err_t ret =
        nvs_flash_init();


    if (
        ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND
    )
    {
        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        ESP_ERROR_CHECK(
            nvs_flash_init()
        );
    }
    else
    {
        ESP_ERROR_CHECK(ret);
    }


    // ========================================================
    // SENSOR QUEUE
    // ========================================================

    sensor_queue =
        xQueueCreate(
            1,
            sizeof(bme280_data_t)
        );


    if (sensor_queue == NULL)
    {
        ESP_LOGE(
            TAG,
            "Queue creation failed"
        );

        return;
    }


    // ========================================================
    // I2C BUS
    // ========================================================

    i2c_master_bus_config_t bus_config =
    {
        .clk_source =
            I2C_CLK_SRC_DEFAULT,

        .i2c_port =
            I2C_NUM_0,

        .sda_io_num =
            SDA_GPIO,

        .scl_io_num =
            SCL_GPIO,

        .glitch_ignore_cnt =
            7,

        .flags.enable_internal_pullup =
            true,
    };


    i2c_master_bus_handle_t bus_handle;


    ESP_ERROR_CHECK(
        i2c_new_master_bus(
            &bus_config,
            &bus_handle
        )
    );


    // ========================================================
    // LCD
    // ========================================================

    ESP_ERROR_CHECK(
        lcd_init(
            bus_handle
        )
    );


    lcd_clear();


    lcd_set_cursor(
        0,
        0
    );

    lcd_print(
        "   BME280 SENSOR"
    );


    // ========================================================
    // BME280
    // ========================================================

    ESP_ERROR_CHECK(
        bme280_init(
            bus_handle
        )
    );


    // ========================================================
    // WIFI
    // ========================================================

    ESP_ERROR_CHECK(
        wifi_manager_init()
    );


    // ========================================================
    // AWS IoT
    // ========================================================

    aws_iot_init();


    // ========================================================
    // HISTORY CLOUD TASK
    // ========================================================
    //
    // 15-second publishing
    // bme280/data
    //

    xTaskCreate(
        cloud_task,
        "cloud_task",
        8192,
        NULL,
        5,
        NULL
    );


    // ========================================================
    // LIVE CLOUD TASK
    // ========================================================
    //
    // 1-second publishing
    // bme280/live
    //

    xTaskCreate(
        live_task,
        "live_task",
        8192,
        NULL,
        5,
        NULL
    );


    // ========================================================
    // MAIN SENSOR LOOP
    // ========================================================

    while (1)
    {
        bme280_data_t data;


        ret =
            bme280_read(
                &data
            );


        if (ret == ESP_OK)
        {
            // =================================================
            // SERIAL LOG
            // =================================================

            ESP_LOGI(
                TAG,
                "Temperature: %.2f C | Humidity: %.2f %%",
                data.temperature,
                data.humidity
            );


            // =================================================
            // LCD TEMPERATURE
            // =================================================

            char temp[17];


            snprintf(
                temp,
                sizeof(temp),
                "Temp: %5.2f C",
                data.temperature
            );


            lcd_set_cursor(
                1,
                0
            );

            lcd_print(
                "                "
            );


            lcd_set_cursor(
                1,
                0
            );

            lcd_print(
                temp
            );


            // =================================================
            // LCD HUMIDITY
            // =================================================

            char hum[17];


            snprintf(
                hum,
                sizeof(hum),
                "Hum : %5.2f %%",
                data.humidity
            );


            lcd_set_cursor(
                2,
                0
            );

            lcd_print(
                "                "
            );


            lcd_set_cursor(
                2,
                0
            );

            lcd_print(
                hum
            );


            // =================================================
            // UPDATE QUEUE
            // =================================================

            xQueueOverwrite(
                sensor_queue,
                &data
            );


            // =================================================
            // LCD AWS STATUS
            // =================================================

            lcd_set_cursor(
                3,
                0
            );


            if (
                wifi_manager_is_connected()
            )
            {
                if (
                    aws_iot_is_connected()
                )
                {
                    lcd_print(
                        "AWS: ONLINE     "
                    );
                }
                else
                {
                    lcd_print(
                        "AWS: CONNECTING "
                    );
                }
            }
            else
            {
                lcd_print(
                    "WiFi: OFFLINE   "
                );
            }
        }
        else
        {
            ESP_LOGE(
                TAG,
                "BME280 read failed"
            );
        }


        // ====================================================
        // ONE SECOND SENSOR/LCD UPDATE
        // ====================================================

        vTaskDelay(
            pdMS_TO_TICKS(1000)
        );
    }
    }