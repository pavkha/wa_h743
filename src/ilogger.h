#pragma once

#include <array>
#include <cstdint>

class ilogger {
  public:
    enum class level : uint8_t { emergency, error, warning, info, debug };

    virtual bool log(const char* msg, level log_level, const char* src) = 0;

    virtual ~ilogger() = default;
};
