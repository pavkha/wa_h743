#pragma once

#include "uart_logger.h"

using current_logger = uart2_logger;

#define LOG(message, level, source) \
    (current_logger::instance().log((message), (level), (source)))

#define LOG_EMERGENCY(message, source) \
    (current_logger::instance().log((message), logger::level::emergency, (source)))

#define LOG_ERROR(message, source) \
    (current_logger::instance().log((message), logger::level::error, (source)))

#define LOG_WARNING(message, source) \
    (current_logger::instance().log((message), logger::level::warning, (source)))

#define LOG_INFO(message, source) \
    (current_logger::instance().log((message), logger::level::info, (source)))

#define LOG_DEBUG(message, source) \
    (current_logger::instance().log((message), logger::level::debug, (source)))
