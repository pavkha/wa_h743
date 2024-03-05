#include "uart_stream_in.h"
#include "common_sys.h"

namespace hw {

uart_stream_in::~uart_stream_in() {}

template <uint16_t S>
int uart_stream_in_buff<S>::read(iuart_stream_in_buff::data_type* buff, uint16_t length) {
    disable_interrupts();
    uint16_t count = count_;
    enable_interrupts();

    if (count > 0) {
        if (length > count) {
            length = count;
        } else {
            count = length;
        }

        for (uint16_t i = 0; count > 0; --count) {
            buff[i++] = buff_[head_++];
            head_ %= S;
        }

        disable_interrupts();
        count_ -= length;
        enable_interrupts();

        return length;
    }

    return 0;
}

template <uint16_t S>
bool uart_stream_in_buff<S>::write(iuart_stream_in_buff::data_type data) {
    if (count_ < S) {
        ++count_;
        buff_[tail_++] = data;
        tail_ %= S;
        return true;
    }
    return false;
}

template <uint16_t S>
int uart_stream_in_rtos_buff<S>::read(iuart_stream_in_buff::data_type* buff,
                                      uint16_t length) {
    for (uint16_t i = 0; i < length; ++i) {
        buff_.Dequeue(&buff[i]);
    }

    return length;
}

template <uint16_t S>
bool uart_stream_in_rtos_buff<S>::write(iuart_stream_in_buff::data_type data) {
    return buff_.EnqueueFromISR(&data, nullptr);
}

int uart_stream_in::read(uint8_t* buff, uint16_t length) {
    return buff_.read(buff, length);
}

void uart_stream_in::receive() {
    if (((USART_CR1(uart_) & USART_CR1_RXNEIE) != 0) &&
        ((USART_ISR(uart_) & USART_ISR_RXNE) != 0)) {
        buff_.write(static_cast<uint8_t>(usart_recv(uart_)));
    }
}

// uart_stream_in_buff<1024> uart4_stream_in_buff{};
uart_stream_in_rtos_buff<1024> uart4_stream_in_buff{};

uart4_stream_in::uart4_stream_in()
    : uart_stream_in{uart4_handle, uart4_stream_in_buff} {};

extern "C" void uart4_isr(void) {
    uart4_stream_in::instance().receive();
}

}    // namespace hw
