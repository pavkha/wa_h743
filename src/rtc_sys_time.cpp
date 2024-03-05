#include "rtc_sys_time.h"
#include "lcd.h"
#include "log.h"
#include "rtc.h"

#include <critical.hpp>
#include <ticks.hpp>

#include <cstdio>

namespace hw {

void rtc_int::get(rtc_date& date, rtc_time& time) {
    rtos::CriticalSection::Enter();

    rtc_get_time(time);
    rtc_get_date(date);

    rtos::CriticalSection::Exit();
}

bool rtc_int::set(const rtc_date& date, const rtc_time& time) {
    if ((date.week_day < 8 && date.week_day > 0) && (date.day < 32 && date.day > 0) &&
        (date.month < 13 && date.month > 0) && time.hours < 24 && time.minutes < 60 &&
        time.seconds < 60) {
        rtos::CriticalSection::Enter();

        rtc_set_date(date);
        rtc_set_time(time);

        rtos::CriticalSection::Exit();

        return true;
    }

    return false;
}

}    // namespace hw

sys_time::sys_time() : rtc_{hw::rtc_int::instance()} {}

bool sys_time::get(systime_string& sys_time) {
    rtos::LockGuard lg{mutex_};

    if (time_valide_) {
        sys_time = date_time_;
    }

    return time_valide_;
}

bool sys_time::get(rtc_date& date, rtc_time& time) {
    rtos::LockGuard lg{mutex_};

    if (time_valide_) {
        time = time_;
        date = date_;
    }

    return time_valide_;
}

void sys_time::update() {
    rtos::LockGuard lg{mutex_};

    rtc_.get(date_, time_);

    std::snprintf(reinterpret_cast<char*>(date_time_.data()),
                  date_time_.max_size(),
                  "%02x-%02x-20%02x %02x:%02x:%02x.%03d",
                  date_.day,
                  date_.month,
                  date_.year,
                  time_.hours,
                  time_.minutes,
                  time_.seconds,
                  time_.millisec);

    time_valide_ = true;
}

const auto sys_time_task_name{"sys_time"};
const auto sys_time_task_stack_size{configMINIMAL_STACK_SIZE * 2};
const auto sys_time_task_priority{configMAX_PRIORITIES - 1};

sys_time_thread::sys_time_thread()
    : Thread{sys_time_task_name, sys_time_task_stack_size, sys_time_task_priority} {
    this->Start();
}

void sys_time_thread::Run() {
    sleep_time_ = rtos::Ticks::MsToTicks(sleep_time_ms);
    static systime_string sys_time{}, prev_sys_time{};

    LOG_INFO("start", GetName());

    while (true) {
        Thread::Delay(sleep_time_);

        sys_time_update();
        get_sys_time(sys_time);

        if (sys_time != prev_sys_time) {
            prev_sys_time = sys_time;
            lcd_show_sys_time(sys_time.data());
        }
    }
}

bool get_sys_time(systime_string& systime) {
    return sys_time::instance().get(systime);
}

bool get_sys_time(rtc_date& date, rtc_time& time) {
    return sys_time::instance().get(date, time);
}

void sys_time_update() {
    sys_time::instance().update();
}
