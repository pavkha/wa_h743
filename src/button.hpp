#pragma once

#include "ibutton.hpp"

#include <libopencm3/stm32/gpio.h>

namespace hw {

class button : public ibutton {
  public:
    button(uint32_t port, uint16_t pin, active_level level)
        : ibutton{level}, port_{port}, pin_{pin} {}
    button(const button&) = delete;
    button& operator=(button&&) = delete;
    button(button&&) = delete;
    button& operator=(const button&) = delete;
    ~button() = default;

  private:
    const uint32_t port_;
    const uint16_t pin_;

    active_level pin_read() const override {
        return static_cast<active_level>((gpio_port_read(port_) & pin_) == 1);
    }
};

}    // namespace hw
