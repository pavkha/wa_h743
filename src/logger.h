#pragma once

#include "ilogger.h"

#include <mutex.hpp>

class istream_out;

namespace rtos = cpp_freertos;

class logger : public ilogger {
  public:
    logger(bool color, level log_level, istream_out& out)
        : color_{color}, level_{log_level}, out_{out} {}

    bool log(const char* msg, level log_level, const char* src) override;

    auto level_string(level log_level) const -> const char*;
    auto level_string_color(level log_level) const -> const char*;

    virtual ~logger() = 0;

  private:
    bool color_;
    level level_;

    istream_out& out_;
    rtos::MutexStandard mutex_{};
};
