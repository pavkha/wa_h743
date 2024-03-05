#pragma once

#include "iclock.h"
#include "singleton.h"
#include "sys_time.h"

#include <mutex.hpp>
#include <thread.hpp>

#include <array>

namespace rtos = cpp_freertos;

namespace hw {

class rtc_int : public iclock, public static_singleton<rtc_int> {
  public:
    friend class static_singleton<rtc_int>;

    void get(rtc_date& date, rtc_time& time) override;
    bool set(const rtc_date& date, const rtc_time& time) override;

  private:
    rtc_int() = default;
};

}    // namespace hw

class sys_time : public static_singleton<sys_time> {
  public:
    friend class static_singleton<sys_time>;

    bool get(systime_string& time);
    bool get(rtc_date& date, rtc_time& time);
    void update();

  private:
    sys_time();

    bool time_valide_{false};

    rtc_time time_{};
    rtc_date date_{};
    systime_string date_time_{""};

    iclock& rtc_;
    rtos::MutexStandard mutex_{};
};

class sys_time_thread : public rtos::Thread {
  public:
    sys_time_thread();

    void Run() override;

    static const auto sleep_time_ms{100};

  private:
    TickType_t sleep_time_{};
};
