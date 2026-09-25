#include "lcd.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_rom_sys.h"


static i2c_master_dev_handle_t lcd_handle;


// ============================================================
// LCD DEFINITIONS
// ============================================================

#define LCD_RS          0x01
#define LCD_RW          0x02
#define LCD_EN          0x04
#define LCD_BACKLIGHT   0x08


// ============================================================
// LOW LEVEL WRITE
// ============================================================

static void lcd_write(
    uint8_t data)
{
    i2c_master_transmit(
        lcd_handle,
        &data,
        1,
        100
    );
}


// ============================================================
// ENABLE PULSE
// ============================================================

static void lcd_enable(
    uint8_t data)
{
    lcd_write(
        data |
        LCD_EN |
        LCD_BACKLIGHT
    );

    esp_rom_delay_us(1);

    lcd_write(
        (data & ~LCD_EN) |
        LCD_BACKLIGHT
    );

    esp_rom_delay_us(50);
}


// ============================================================
// NIBBLE
// ============================================================

static void lcd_nibble(
    uint8_t nibble,
    uint8_t rs)
{
    uint8_t data =
        (nibble & 0x0F) << 4;

    if (rs)
        data |= LCD_RS;

    lcd_enable(data);
}


// ============================================================
// COMMAND
// ============================================================

static void lcd_command(
    uint8_t command)
{
    lcd_nibble(
        command >> 4,
        0
    );

    lcd_nibble(
        command & 0x0F,
        0
    );

    if (
        command == 0x01 ||
        command == 0x02
    )
    {
        vTaskDelay(
            pdMS_TO_TICKS(2)
        );
    }
}


// ============================================================
// DATA
// ============================================================

static void lcd_data(
    uint8_t data)
{
    lcd_nibble(
        data >> 4,
        1
    );

    lcd_nibble(
        data & 0x0F,
        1
    );
}


// ============================================================
// INIT
// ============================================================

esp_err_t lcd_init(
    i2c_master_bus_handle_t bus_handle)
{
    i2c_device_config_t config =
    {
        .dev_addr_length =
            I2C_ADDR_BIT_LEN_7,

        .device_address =
            LCD_ADDR,

        .scl_speed_hz =
            100000,
    };


    esp_err_t ret =
        i2c_master_bus_add_device(
            bus_handle,
            &config,
            &lcd_handle
        );

    if (ret != ESP_OK)
        return ret;


    vTaskDelay(
        pdMS_TO_TICKS(50)
    );


    lcd_nibble(0x03, 0);

    vTaskDelay(
        pdMS_TO_TICKS(5)
    );

    lcd_nibble(0x03, 0);

    vTaskDelay(
        pdMS_TO_TICKS(5)
    );

    lcd_nibble(0x03, 0);

    vTaskDelay(
        pdMS_TO_TICKS(1)
    );

    lcd_nibble(0x02, 0);


    // 4 bit / 2 line

    lcd_command(0x28);

    // Display OFF

    lcd_command(0x08);

    // Clear

    lcd_command(0x01);

    // Entry mode

    lcd_command(0x06);

    // Display ON

    lcd_command(0x0C);


    return ESP_OK;
}


// ============================================================
// CLEAR
// ============================================================

void lcd_clear(void)
{
    lcd_command(0x01);
}


// ============================================================
// CURSOR
// ============================================================

void lcd_set_cursor(
    uint8_t row,
    uint8_t col)
{
    static const uint8_t offsets[] =
    {
        0x00,
        0x40,
        0x10,
        0x50
    };


    lcd_command(
        0x80 |
        (offsets[row] + col)
    );
}


// ============================================================
// PRINT
// ============================================================

void lcd_print(
    const char *text)
{
    while (*text)
    {
        lcd_data(*text);
        text++;
    }
}