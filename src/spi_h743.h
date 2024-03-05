#pragma once

#include <cstdint>

void spi_init();

int32_t spi4_transmit(uint8_t* pdata, uint32_t length);
int32_t spi4_receive(uint8_t* pdata, uint32_t length);
