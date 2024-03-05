#include "lcd.h"
#include "rtc.h"
#include "spi_h743.h"

#include <cstdio>

extern "C" {
void lcd_rs_set(void) {
    gpio_set(lcd_port, lcd_d_cx_pin);
}

void lcd_rs_reset(void) {
    gpio_clear(lcd_port, lcd_d_cx_pin);
}

void lcd_cs_set(void) {
    gpio_set(lcd_port, lcd_cs_pin);
}

void lcd_cs_reset(void) {
    gpio_clear(lcd_port, lcd_cs_pin);
}

void lcd_bl_on(void) {
    gpio_clear(lcd_port, lcd_bl_pin);
}

void lcd_bl_off(void) {
    gpio_set(lcd_port, lcd_bl_pin);
}

int32_t lcd_get_tick(void) {
    return 0;
}

int32_t lcd_spi_transmit(uint8_t* pdata, uint32_t length) {
    return spi4_transmit(pdata, length);
}

int32_t lcd_spi_receive(uint8_t* pdata, uint32_t length) {
    return spi4_receive(pdata, length);
}

uint32_t lcd_get_time(void) {
    rtc_time time;

    rtc_get_time(time);
    return (time.hours << 16) | (time.minutes << 8) | (time.seconds);
}

uint32_t lcd_get_date(void) {
    rtc_date date;

    rtc_get_date(date);
    return (date.day << 16) | (date.month << 8) | (date.year);
}
}

void lcd_app_info() {
    LCD_Test();
}

void lcd_show_sys_time(uint8_t* sys_time) {
    uint8_t text[35];

    std::snprintf((char*)&text,
                  sizeof(text),
                  "Time:  %s",
                  reinterpret_cast<const char*>(sys_time));
    LCD_ShowStringFixFont(0, 64, text);
}
