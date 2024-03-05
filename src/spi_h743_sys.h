#pragma once

#include "common_sys.h"

#include <libopencm3/stm32/rcc.h>

#define SPI4_BASE 0x40013400U
#define SPI4 SPI4_BASE

#define SPI_CR1(spi_base) REG32((spi_base) + 0x00)
#define SPI_CR2(spi_base) REG32((spi_base) + 0x04)
#define SPI_CFG1(spi_base) REG32((spi_base) + 0x08)
#define SPI_CFG2(spi_base) REG32((spi_base) + 0x0c)
#define SPI_SR(spi_base) REG32((spi_base) + 0x14)
#define SPI_IFCR(spi_base) REG32((spi_base) + 0x18)
#define SPI_DT8(spi_base) REG8((spi_base) + 0x20)
#define SPI_DR8(spi_base) REG8((spi_base) + 0x30)

#define SPI_CR1_SPE (1 << 0)
#define SPI_CR1_CSTART (1 << 9)
#define SPI_CR1_HDDIR (1 << 11)
#define SPI_CR1_SSI (1 << 12)

#define SPI_CFG1_BAUDRATE_FPCLK_DIV_8 (1 << 29)

#define SPI_CFG2_BIDIMODE_1LINE_BIDIR (3 << 17)
#define SPI_CFG2_MASTER (1 << 22)
#define SPI_CFG2_MSBFIRST (0 << 23)
#define SPI_CFG2_CPHA_CLK_TRANSITION_1 (0 << 24)
#define SPI_CFG2_CPOL_CLK_TO_0_WHEN_IDLE (0 << 25)
#define SPI_CFG2_SSOM (1 << 30)
#define SPI_CFG2_SSM (1 << 26)

#define SPI_DS_8BIT (0x7 << 0)

#define SPI_SR_RXP (1 << 0)
#define SPI_SR_TXP (1 << 1)
#define SPI_SR_EOT (1 << 3)
#define SPI_SR_TXTF (1 << 4)
