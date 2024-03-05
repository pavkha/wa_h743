#pragma once

#include "logger.h"
#include "singleton.h"
#include "uart_stream_out.h"

class uart2_logger : public static_singleton<uart2_logger>, public logger {
  public:
    friend class static_singleton<uart2_logger>;

  private:
    uart2_logger()
        : logger{true, logger::level::debug, hw::uart2_stream_out::instance()} {}
};
