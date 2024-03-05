#pragma once

#include "rtc.h"

class iclock {
  public:
    virtual ~iclock() = default;

    virtual void get(rtc_date& date, rtc_time& time) = 0;
    virtual bool set(const rtc_date& date, const rtc_time& time) = 0;
};
