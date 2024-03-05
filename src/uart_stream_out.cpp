#include "uart_stream_out.h"

namespace hw {

uart_stream_out::~uart_stream_out() {}

bool uart_stream_out::write(uint8_t* data, uint16_t length) {
    for (auto i = 0U; i < length; ++i) {
        usart_send_blocking(uart_, data[i]);
    }

    return true;
}

}    // namespace hw
