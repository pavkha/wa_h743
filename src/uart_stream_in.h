#pragma once

#include "common_uart.h"
#include "istream_in.h"
#include "singleton.h"

#include <queue.hpp>

namespace hw {

namespace rtos = cpp_freertos;

class iuart_stream_in_buff {
  public:
    virtual ~iuart_stream_in_buff() = default;

    using data_type = uint8_t;

    virtual int read(data_type* data, uint16_t length) = 0;
    virtual bool write(data_type data) = 0;
};

template <uint16_t S>
class uart_stream_in_buff : public iuart_stream_in_buff {
  public:
    int read(data_type* data, uint16_t length) override;
    bool write(data_type data) override;

  private:
    uint8_t buff_[S];
    uint16_t head_{0};
    uint16_t tail_{0};
    uint16_t count_{0};
};

template <uint16_t S>
class uart_stream_in_rtos_buff : public iuart_stream_in_buff {
  public:
    int read(data_type* data, uint16_t length) override;
    bool write(data_type data) override;

  private:
    rtos::Queue buff_{S, sizeof(data_type)};
};

class uart_stream_in : public istream_in {
  public:
    uart_stream_in(uart_handle_type uart, iuart_stream_in_buff& buff)
        : uart_{uart}, buff_{buff} {}
    virtual ~uart_stream_in() = 0;

    int read(uint8_t* buff, uint16_t length) override;
    void receive() override;

  private:
    const uart_handle_type uart_;
    iuart_stream_in_buff& buff_;
};

class uart4_stream_in : public static_singleton<uart4_stream_in>, public uart_stream_in {
  public:
    friend class static_singleton<uart4_stream_in>;
    ~uart4_stream_in() = default;

  private:
    uart4_stream_in();
};

}    // namespace hw
