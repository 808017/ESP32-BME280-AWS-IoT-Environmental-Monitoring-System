idf_component_register(
    SRCS
        "main.c"
        "bme280.c"
        "lcd.c"
        "wifi_manager.c"
        "aws_iot.c"

    INCLUDE_DIRS
        "."

    EMBED_TXTFILES
        "A"
        "d"
        "p"

    REQUIRES
        esp_driver_i2c
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        mqtt
)