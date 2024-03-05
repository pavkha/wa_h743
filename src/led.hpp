#pragma once

#include "iled.hpp"

#include <libopencm3/stm32/gpio.h>

namespace hw {

class led : public iled {
  public:
    led(uint32_t port, uint16_t pin, active_level level)
        : iled{level}, port_{port}, pin_{pin} {}

  private:
    const uint32_t port_;
    const uint16_t pin_;

    void pin_set() const override { gpio_set(port_, pin_); }
    void pin_reset() const override { gpio_clear(port_, pin_); }
    void pin_toggle() const override { gpio_toggle(port_, pin_); }
};

}    // namespace hw
