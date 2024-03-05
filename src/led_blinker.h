#pragma once

#include <cstdint>
#include <thread.hpp>

class iled;

namespace rtos = cpp_freertos;

class led_blinker_thread : rtos::Thread {
  public:
    led_blinker_thread(const iled& led, uint16_t period, uint16_t pulse);
    led_blinker_thread(const led_blinker_thread&) = delete;
    led_blinker_thread& operator=(const led_blinker_thread&) = delete;
    led_blinker_thread(led_blinker_thread&) = delete;
    led_blinker_thread& operator=(led_blinker_thread&) = delete;
    ~led_blinker_thread() = default;

    void mode(uint16_t period, uint16_t pulse);

  private:
    void Run() override;

    enum class state : uint8_t { pause, pulse };
    state state_ = state::pulse;

    const iled& led_;

    TickType_t pause_;
    TickType_t pulse_;
};
