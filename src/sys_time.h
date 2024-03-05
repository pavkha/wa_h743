#pragma once

#include "rtc.h"

#include <array>
#include <cstdint>

using systime_string = std::array<uint8_t, 26>;

void sys_time_update();
bool get_sys_time(systime_string& systime);
bool get_sys_time(rtc_date& date, rtc_time& time);
