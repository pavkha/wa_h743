#pragma once

#include <cstdint>

struct rtc_time {
    uint8_t hours_format;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t millisec;
};

struct rtc_date {
    uint8_t year;
    uint8_t month;
    uint8_t week_day;
    uint8_t day;
};

void rtc_init();

void rtc_get_time(rtc_time& time);
void rtc_get_date(rtc_date& date);

void rtc_set_time(const rtc_time& time);
void rtc_set_date(const rtc_date& date);

static inline uint8_t rtc_convert_bin2bcd(uint8_t bin) {
    return ((bin / 10) << 4) | (bin % 10);
}

static inline uint8_t rtc_convert_bcd2bin(uint8_t bcd) {
    return ((bcd >> 4U) * 10) | (bcd & 0xF0);
}
