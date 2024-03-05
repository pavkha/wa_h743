#include "application.h"
#include "common_sys.h"
#include "rtc.h"
#include "spi_h743.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

const rcc_pll_config rcc_clk_3v3_hse_25MHz_hclk_400MHz{
        .sysclock_source = rcc_osc::RCC_PLL,
        .pll_source = RCC_PLLCKSELR_PLLSRC_HSE,
        .hse_frequency = 25000000,
        .pll1{.divm = 2, .divn = 64, .divp = 2, .divq = 2, .divr = 2},
        .pll2{.divm = 32, .divn = 129, .divp = 2, .divq = 2, .divr = 2},
        .pll3{.divm = 32, .divn = 129, .divp = 2, .divq = 2, .divr = 2},
        .core_pre = 0,
        .hpre = 8,
        .ppre1 = 4,
        .ppre2 = 4,
        .ppre3 = 4,
        .ppre4 = 4,
        .flash_waitstates = 2,
        .voltage_scale = pwr_vos_scale::PWR_VOS_SCALE_2,
        .power_mode = PWR_SYS_SCU_LDO,
        .smps_level = 0};

static void sys_clk_init() {
    rcc_clock_setup_pll(&rcc_clk_3v3_hse_25MHz_hclk_400MHz);
}

static void gpio_init() {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOE);

    gpio_clear(e3_led_port, e3_led_pin);
    gpio_mode_setup(e3_led_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, e3_led_pin);

    gpio_set(lcd_port, lcd_cs_pin | lcd_bl_pin | lcd_d_cx_pin);
    gpio_mode_setup(lcd_port,
                    GPIO_MODE_OUTPUT,
                    GPIO_PUPD_NONE,
                    lcd_cs_pin | lcd_bl_pin | lcd_d_cx_pin);

    gpio_mode_setup(lcd_port, GPIO_MODE_AF, GPIO_PUPD_NONE, lcd_sck_pin | lcd_mosi_pin);
    gpio_set_af(lcd_port, GPIO_AF5, lcd_sck_pin | lcd_mosi_pin);

    gpio_mode_setup(
            uart4_port, GPIO_MODE_AF, GPIO_PUPD_NONE, uart4_rx_pin | uart4_tx_pin);
    gpio_set_af(uart4_port, GPIO_AF8, uart4_rx_pin | uart4_tx_pin);

    gpio_mode_setup(
            usart2_port, GPIO_MODE_AF, GPIO_PUPD_NONE, usart2_rx_pin | usart2_tx_pin);
    gpio_set_af(usart2_port, GPIO_AF7, usart2_rx_pin | usart2_tx_pin);
}

static void usart_overrun_disable(uint32_t usart) {
    uint32_t reg = USART_CR3(usart);

    reg |= USART_CR3_OVRDIS;
    USART_CR3(usart) = reg;
}

static void usart_init() {
    rcc_periph_clock_enable(RCC_UART4);
    usart_set_baudrate(UART4, 115200);
    usart_set_databits(UART4, 8);
    usart_set_parity(UART4, USART_PARITY_NONE);
    usart_set_stopbits(UART4, USART_CR2_STOPBITS_1);
    usart_set_mode(UART4, USART_MODE_TX_RX);
    usart_set_flow_control(UART4, USART_FLOWCONTROL_NONE);
    usart_overrun_disable(UART4);
    nvic_set_priority(NVIC_UART4_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    nvic_enable_irq(NVIC_UART4_IRQ);
    usart_enable_rx_interrupt(UART4);
    usart_enable(UART4);

    rcc_periph_clock_enable(RCC_USART2);
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_stopbits(USART2, USART_CR2_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_enable(USART2);
}

void init() {
    sys_clk_init();
    gpio_init();
    spi_init();
    rtc_init();
    usart_init();
}
