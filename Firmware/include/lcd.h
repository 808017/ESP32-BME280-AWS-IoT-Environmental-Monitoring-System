#ifndef LCD_H
#define LCD_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#define LCD_ADDR 0x27

esp_err_t lcd_init(
    i2c_master_bus_handle_t bus_handle
);

void lcd_clear(void);

void lcd_set_cursor(
    uint8_t row,
    uint8_t col
);

void lcd_print(
    const char *text
);

#endif