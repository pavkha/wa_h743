#include "logger.h"
#include "istream_out.h"
#include "sys_time.h"
#include "text_colors.h"

#include <cstdio>
#include <cstring>

logger::~logger() {}

bool logger::log(const char* msg, level log_level, const char* src) {
    rtos::LockGuard lg{mutex_};

    if (static_cast<uint8_t>(log_level) > static_cast<uint8_t>(level_)) {
        return false;
    }

    static systime_string systime;
    static std::array<uint8_t, 512> logger_string;

    auto level_string_color_end = [this]() -> const char* {
        return color_ ? text_color_default : text_color_empty;
    };

    sys_time_update();
    get_sys_time(systime);

    std::snprintf(reinterpret_cast<char*>(logger_string.data()),
                  logger_string.max_size(),
                  "%s <%s%s%s>\t[%s] %s\n",
                  reinterpret_cast<char*>(systime.data()),
                  level_string_color(log_level),
                  level_string(log_level),
                  level_string_color_end(),
                  src,
                  msg);

    out_.write(logger_string.data(),
               static_cast<uint16_t>(
                       std::strlen(reinterpret_cast<const char*>(logger_string.data()))));

    return true;
}

auto logger::level_string(level log_level) const -> const char* {
    static const char* level_strings[] = {
            "EMERGENCY", "ERROR", "WARNING", "INFO", "DEBUG"};

    auto level = static_cast<uint8_t>(log_level);

    if (level > sizeof(level_strings) / sizeof(level_strings[0])) {
        return "UNDEFINED";
    }

    return level_strings[level];
}

auto logger::level_string_color(level log_level) const -> const char* {
    if (!color_) {
        return text_color_empty;
    }

    switch (log_level) {
        case level::emergency:
            return text_color_red;
        case level::error:
            return text_color_red;
        case level::warning:
            return text_color_orange;
        case level::info:
            return text_color_green;
        case level::debug:
            return text_color_blue;
        default:
            return text_color_red;
    }
}
