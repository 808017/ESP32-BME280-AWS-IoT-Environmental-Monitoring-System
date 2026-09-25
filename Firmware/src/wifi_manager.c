#include "wifi_manager.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"


#define WIFI_SSID       ""
#define WIFI_PASSWORD   "123456789"

#define WIFI_CONNECTED_BIT BIT0


static const char *TAG = "WIFI";

static EventGroupHandle_t wifi_event_group;


static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START
    )
    {
        esp_wifi_connect();
    }


    else if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED
    )
    {
        xEventGroupClearBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );

        esp_wifi_connect();

        ESP_LOGW(
            TAG,
            "WiFi disconnected"
        );
    }


    else if (
        event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP
    )
    {
        xEventGroupSetBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );

        ESP_LOGI(
            TAG,
            "WiFi connected"
        );
    }
}


esp_err_t wifi_manager_init(void)
{
    wifi_event_group =
        xEventGroupCreate();


    ESP_ERROR_CHECK(
        esp_netif_init()
    );


    ESP_ERROR_CHECK(
        esp_event_loop_create_default()
    );


    esp_netif_create_default_wifi_sta();


    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();


    ESP_ERROR_CHECK(
        esp_wifi_init(&cfg)
    );


    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            wifi_event_handler,
            NULL
        )
    );


    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            wifi_event_handler,
            NULL
        )
    );


    wifi_config_t config = {};


    strncpy(
        (char *)config.sta.ssid,
        WIFI_SSID,
        sizeof(config.sta.ssid) - 1
    );


    strncpy(
        (char *)config.sta.password,
        WIFI_PASSWORD,
        sizeof(config.sta.password) - 1
    );


    config.sta.pmf_cfg.capable = true;

    config.sta.pmf_cfg.required = false;


    ESP_ERROR_CHECK(
        esp_wifi_set_mode(
            WIFI_MODE_STA
        )
    );


    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &config
        )
    );


    ESP_ERROR_CHECK(
        esp_wifi_start()
    );


    return ESP_OK;
}


bool wifi_manager_is_connected(void)
{
    EventBits_t bits =
        xEventGroupGetBits(
            wifi_event_group
        );


    return
        (bits & WIFI_CONNECTED_BIT) != 0;
}