#pragma once

#include "common_uart.h"
#include "istream_out.h"
#include "singleton.h"

namespace hw {

class uart_stream_out : public istream_out {
  public:
    explicit uart_stream_out(uart_handle_type uart) : uart_{uart} {}
    virtual ~uart_stream_out() = 0;

    bool write(uint8_t* data, uint16_t length) override;

  private:
    const uart_handle_type uart_;
};

class uart4_stream_out : public static_singleton<uart4_stream_out>,
                         public uart_stream_out {
  public:
    friend class static_singleton<uart4_stream_out>;
    ~uart4_stream_out() = default;

  private:
    uart4_stream_out() : uart_stream_out{uart4_handle} {};
};

class uart2_stream_out : public static_singleton<uart2_stream_out>,
                         public uart_stream_out {
  public:
    friend class static_singleton<uart2_stream_out>;
    ~uart2_stream_out() = default;

  private:
    uart2_stream_out() : uart_stream_out{uart2_handle} {};
};

}    // namespace hw
