#pragma once

#include "button.hpp"
#include "cmd_path.h"
#include "cmd_shell.h"
#include "lcd.h"
#include "led.hpp"
#include "led_blinker.h"
#include "log.h"
#include "rtc.h"
#include "rtc_sys_time.h"
#include "uart_stream_in.h"
#include "uart_stream_out.h"
#include "version.h"

#include <libopencm3/stm32/gpio.h>

const auto e3_led_port{GPIOE};
const auto e3_led_pin{GPIO3};
const auto e3_led_blink_period_ms{1000};
const auto e3_led_blink_pulse_ms{100};
const auto e3_led_blink_stack_size{configMINIMAL_STACK_SIZE};
const auto e3_led_blink_prio{configMAX_PRIORITIES - 1};

const auto k1_button_port{GPIOC};
const auto k1_button_pin{GPIO13};

const auto uart4_port{GPIOB};
const auto uart4_rx_pin{GPIO8};
const auto uart4_tx_pin{GPIO9};

const auto usart2_port{GPIOA};
const auto usart2_rx_pin{GPIO3};
const auto usart2_tx_pin{GPIO2};
