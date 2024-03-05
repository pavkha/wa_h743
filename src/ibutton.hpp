#pragma once

#include <cstdint>

class ibutton {
  public:
    enum class active_level : uint8_t { low, high };

    ibutton(active_level level) : active_level_{level} {}
    virtual ~ibutton() = default;

    bool pushed() const {
        return ((static_cast<uint8_t>(pin_read()) ^
                 static_cast<uint8_t>(active_level_)) == 0);
    }

  private:
    const active_level active_level_;

    virtual active_level pin_read() const = 0;
};
