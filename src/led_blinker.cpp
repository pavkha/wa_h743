#include "led_blinker.h"
#include "FreeRTOSConfig.h"
#include "led.hpp"
#include "log.h"

#include <ticks.hpp>

const auto led_blink_task_name{"led_blink"};
const auto led_blink_task_stack_size{configMINIMAL_STACK_SIZE * 2};
const auto led_blink_task_priority{configMAX_PRIORITIES - 1};

led_blinker_thread::led_blinker_thread(const iled& led,
                                       uint16_t period = 1000,
                                       uint16_t pulse = 500)
    : Thread(led_blink_task_name, led_blink_task_stack_size, led_blink_task_priority),
      led_{led} {
    mode(period, pulse);
    this->Start();
}

void led_blinker_thread::mode(uint16_t period, uint16_t pulse) {
    if (pulse >= period) {
        period = 1000;
        pulse = 5;
    }
    pause_ = rtos::Ticks::MsToTicks(period - pulse);
    pulse_ = rtos::Ticks::MsToTicks(pulse);
}

void led_blinker_thread::Run() {
    LOG_INFO("start", GetName());

    while (true) {
        switch (state_) {
            case state::pause:
                led_.off();
                Thread::Delay(pause_);
                state_ = state::pulse;
                break;
            case state::pulse:
                led_.on();
                Thread::Delay(pulse_);
                state_ = state::pause;
                break;
            default:
                state_ = state::pause;
                break;
        }
    }
}
