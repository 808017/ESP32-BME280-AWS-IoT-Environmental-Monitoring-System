#include "aws_iot.h"

#include <stdio.h>
#include <stdbool.h>

#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "AWS_IOT";

// ============================================================
// AWS IoT Core configuration
// ============================================================

#define AWS_IOT_ENDPOINT \
    "mqtts://"

#define AWS_IOT_CLIENT_ID \
    "ESP32-BME280"


// ============================================================
// Embedded certificates
// ============================================================

extern const uint8_t amazon_root_ca_start[] asm(
    "_b");

extern const uint8_t device_cert_start[] asm(
    "_b");

extern const uint8_t private_key_start[] asm(
    "_b");


// ============================================================
// MQTT client
// ============================================================

static esp_mqtt_client_handle_t mqtt_client = NULL;

static volatile bool aws_connected = false;


// ============================================================
// MQTT EVENT HANDLER
// ============================================================

static void mqtt_event_handler(
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data)
{
    esp_mqtt_event_handle_t event =
        (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
        // ----------------------------------------------------
        // Connected
        // ----------------------------------------------------

        case MQTT_EVENT_CONNECTED:

            ESP_LOGI(
                TAG,
                "Connected to AWS IoT Core"
            );

            aws_connected = true;

            break;


        // ----------------------------------------------------
        // Disconnected
        // ----------------------------------------------------

        case MQTT_EVENT_DISCONNECTED:

            ESP_LOGW(
                TAG,
                "Disconnected from AWS IoT Core"
            );

            aws_connected = false;

            break;


        // ----------------------------------------------------
        // MQTT Error
        // ----------------------------------------------------

        case MQTT_EVENT_ERROR:

            ESP_LOGE(
                TAG,
                "MQTT error"
            );

            aws_connected = false;

            break;


        default:

            break;
    }
}


// ============================================================
// AWS IoT INITIALIZATION
// ============================================================

void aws_iot_init(void)
{
    ESP_LOGI(
        TAG,
        "Initializing AWS IoT MQTT..."
    );


    esp_mqtt_client_config_t mqtt_cfg =
    {
        // ----------------------------------------------------
        // AWS IoT endpoint
        // ----------------------------------------------------

        .broker.address.uri =
            AWS_IOT_ENDPOINT,


        // ----------------------------------------------------
        // Amazon Root CA
        // ----------------------------------------------------

        .broker.verification.certificate =
            (const char *)amazon_root_ca_start,


        // ----------------------------------------------------
        // Device certificate
        // ----------------------------------------------------

        .credentials.authentication.certificate =
            (const char *)device_cert_start,


        // ----------------------------------------------------
        // Private key
        // ----------------------------------------------------

        .credentials.authentication.key =
            (const char *)private_key_start,


        // ----------------------------------------------------
        // MQTT client ID
        // ----------------------------------------------------

        .credentials.client_id =
            AWS_IOT_CLIENT_ID,
    };


    // --------------------------------------------------------
    // Initialize MQTT client
    // --------------------------------------------------------

    mqtt_client =
        esp_mqtt_client_init(
            &mqtt_cfg
        );


    if (mqtt_client == NULL)
    {
        ESP_LOGE(
            TAG,
            "Failed to initialize MQTT client"
        );

        return;
    }


    // --------------------------------------------------------
    // Register MQTT event handler
    // --------------------------------------------------------

    esp_err_t err =
        esp_mqtt_client_register_event(
            mqtt_client,
            ESP_EVENT_ANY_ID,
            mqtt_event_handler,
            NULL
        );


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to register MQTT event: %s",
            esp_err_to_name(err)
        );

        return;
    }


    // --------------------------------------------------------
    // Start MQTT
    // --------------------------------------------------------

    err =
        esp_mqtt_client_start(
            mqtt_client
        );


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to start MQTT: %s",
            esp_err_to_name(err)
        );

        return;
    }


    ESP_LOGI(
        TAG,
        "AWS IoT MQTT started"
    );
}


// ============================================================
// AWS CONNECTION STATUS
// ============================================================

bool aws_iot_is_connected(void)
{
    return aws_connected;
}


// ============================================================
// PUBLISH SENSOR DATA TO ANY MQTT TOPIC
// ============================================================

bool aws_iot_publish_topic(
    const char *topic,
    float temperature,
    float humidity)
{
    // --------------------------------------------------------
    // Check MQTT client
    // --------------------------------------------------------

    if (mqtt_client == NULL)
    {
        ESP_LOGW(
            TAG,
            "MQTT client not initialized"
        );

        return false;
    }


    // --------------------------------------------------------
    // Check AWS connection
    // --------------------------------------------------------

    if (!aws_connected)
    {
        ESP_LOGW(
            TAG,
            "AWS IoT not connected"
        );

        return false;
    }


    // --------------------------------------------------------
    // Check topic
    // --------------------------------------------------------

    if (topic == NULL)
    {
        ESP_LOGW(
            TAG,
            "MQTT topic is NULL"
        );

        return false;
    }


    // --------------------------------------------------------
    // Create JSON payload
    // --------------------------------------------------------

    char payload[128];


    snprintf(
        payload,
        sizeof(payload),

        "{"
        "\"device\":\"ESP32-BME280\","
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f"
        "}",

        temperature,
        humidity
    );


    // --------------------------------------------------------
    // Publish MQTT message
    // --------------------------------------------------------

    int msg_id =
        esp_mqtt_client_publish(
            mqtt_client,
            topic,
            payload,
            0,
            1,
            0
        );


    // --------------------------------------------------------
    // Check publish result
    // --------------------------------------------------------

    if (msg_id < 0)
    {
        ESP_LOGE(
            TAG,
            "MQTT publish failed"
        );

        return false;
    }


    // --------------------------------------------------------
    // Log result
    // --------------------------------------------------------

    ESP_LOGI(
        TAG,
        "Published to %s",
        topic
    );

    ESP_LOGI(
        TAG,
        "Payload: %s",
        payload
    );


    return true;
}


// ============================================================
// PUBLISH SENSOR DATA TO EXISTING DATABASE TOPIC
// ============================================================
//
// This keeps your existing AWS/DynamoDB system unchanged.
//
// Topic:
//     bme280/data
//
// IoT Rule:
//     bme280/data
//
// DynamoDB:
//     BME280SensorData
//
// ============================================================

bool aws_iot_publish(
    float temperature,
    float humidity)
{
    return aws_iot_publish_topic(
        "bme280/data",
        temperature,
        humidity
    );
}