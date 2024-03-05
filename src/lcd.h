#pragma once

#include <libopencm3/stm32/gpio.h>

const auto lcd_port{GPIOE};
const auto lcd_d_cx_pin{GPIO13};
const auto lcd_bl_pin{GPIO10};
const auto lcd_cs_pin{GPIO11};
const auto lcd_mosi_pin{GPIO14};
const auto lcd_sck_pin{GPIO12};

extern "C" void LCD_Test(void);
extern "C" void LCD_ShowSysTime(const char* sys_time);
extern "C" void LCD_ShowStringFixFont(uint16_t x, uint16_t y, uint8_t* p);

void lcd_app_info();
void lcd_show_sys_time(uint8_t* sys_time);

#define LCD_SPI_DRV SPI4
