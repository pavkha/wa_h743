#include "spi_h743.h"
#include "spi_h743_sys.h"

static void spi_init_master_(uint32_t spi,
                             uint32_t br,
                             uint32_t cpol,
                             uint32_t cpha,
                             uint32_t lsbfirst) {
    uint32_t reg = SPI_CFG1(spi);

    reg |= br;
    SPI_CFG1(spi) = reg;

    reg = SPI_CFG2(spi);
    reg |= (cpol | cpha | lsbfirst | SPI_CFG2_MASTER);
    SPI_CFG2(spi) = reg;
}

static void spi_enable_software_slave_management_(uint32_t spi) {
    uint32_t reg = SPI_CFG2(spi);

    reg |= SPI_CFG2_SSM;
    SPI_CFG2(spi) = reg;
}

static void spi_set_nss_high_(uint32_t spi) {
    uint32_t reg = SPI_CR1(spi);

    reg |= SPI_CR1_SSI;
    SPI_CR1(spi) = reg;
}

static void spi_set_half_duplex_unidirectional_mode_(uint32_t spi) {
    uint32_t reg = SPI_CFG2(spi);

    reg |= SPI_CFG2_BIDIMODE_1LINE_BIDIR;
    SPI_CFG2(spi) = reg;
}

static void spi_set_1line_tx(uint32_t spi) {
    uint32_t reg = SPI_CR1(spi);

    reg |= SPI_CR1_HDDIR;
    SPI_CR1(spi) = reg;
}

static void spi_set_1line_rx(uint32_t spi) {
    uint32_t reg = SPI_CR1(spi);

    reg &= ~SPI_CR1_HDDIR;
    SPI_CR1(spi) = reg;
}

static void spi_set_data_size_(uint32_t spi, uint16_t data_s) {
    uint32_t reg = SPI_CFG1(spi);

    reg |= data_s;
    SPI_CFG1(spi) = reg;
}

static void spi_enable_(uint32_t spi) {
    uint32_t reg = SPI_CR1(spi);

    reg |= SPI_CR1_SPE;
    SPI_CR1(spi) = reg;
}

static void spi_disable_(uint32_t spi) {
    uint32_t reg32 = SPI_CR1(spi);

    reg32 &= ~(SPI_CR1_SPE);
    SPI_CR1(spi) = reg32;
}

static void spi_set_tranfer_size(uint32_t spi, uint32_t size) {
    SPI_CR2(spi) = size;
}

static void spi_start_master_transfer(uint32_t spi) {
    uint32_t reg = SPI_CR1(spi);

    reg |= SPI_CR1_CSTART;
    SPI_CR1(spi) = reg;
}

static void spi_close_transfer(uint32_t spi) {
    uint32_t reg = SPI_IFCR(spi);

    reg |= (SPI_SR_EOT | SPI_SR_TXTF);
    SPI_IFCR(spi) = reg;
}

static void spi_enable_nss_pulse_mgt(uint32_t spi) {
    uint32_t reg = SPI_CFG2(spi);

    reg |= SPI_CFG2_SSOM;
    SPI_CFG2(spi) = reg;
}

void spi_init() {
    rcc_set_spi45_clksel(RCC_D2CCIP1R_DFSDM1SEL_PCLK2);
    rcc_periph_clock_enable(RCC_SPI4);

    spi_disable_(SPI4);
    spi_set_nss_high_(SPI4);
    spi_enable_software_slave_management_(SPI4);
    spi_set_half_duplex_unidirectional_mode_(SPI4);
    spi_set_data_size_(SPI4, SPI_DS_8BIT);
    spi_init_master_(SPI4,
                     SPI_CFG1_BAUDRATE_FPCLK_DIV_8,
                     SPI_CFG2_CPOL_CLK_TO_0_WHEN_IDLE,
                     SPI_CFG2_CPHA_CLK_TRANSITION_1,
                     SPI_CFG2_MSBFIRST);
    spi_enable_nss_pulse_mgt(SPI4);
}

int32_t spi_transmit(uint32_t spi, uint8_t* pdata, uint32_t length) {
    spi_set_1line_tx(spi);
    spi_set_tranfer_size(spi, length);
    spi_enable_(spi);
    spi_start_master_transfer(spi);

    for (auto i = 0U; i < length; ++i) {
        while ((SPI_SR(spi) & SPI_SR_TXP) == 0) {
        }
        SPI_DT8(spi) = pdata[i];
    }

    while (!(SPI_SR(spi) & SPI_SR_EOT)) {
    }

    spi_close_transfer(spi);
    spi_disable_(spi);

    return 0;
}

int32_t spi_receive(uint32_t spi, uint8_t* pdata, uint32_t length) {
    spi_set_1line_rx(spi);
    spi_set_tranfer_size(spi, length);
    spi_enable_(spi);
    spi_start_master_transfer(spi);

    for (auto i = 0U; i < length; ++i) {
        while ((SPI_SR(spi) & SPI_SR_RXP) == 0) {
        }
        pdata[i] = SPI_DR8(spi);
    }

    spi_close_transfer(spi);
    spi_disable_(spi);

    return 0;
}

int32_t spi4_transmit(uint8_t* pdata, uint32_t length) {
    return spi_transmit(SPI4, pdata, length);
}

int32_t spi4_receive(uint8_t* pdata, uint32_t length) {
    return spi_receive(SPI4, pdata, length);
}
