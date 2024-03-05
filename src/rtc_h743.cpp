#include "rtc.h"
#include "rtc_h743_sys.h"

static uint32_t rcc_ret_rtc_clock_source() {
    uint32_t reg = RCC_BDCR;

    reg &= (RCC_BDCR_RTCSEL_MASK << RCC_BDCR_RTCSEL_SHIFT);

    return reg >> RCC_BDCR_RTCSEL_SHIFT;
};

static void rcc_force_backup_domain_reset() {
    uint32_t reg = RCC_BDCR;

    reg |= RCC_BDCR_BDRST;
    RCC_BDCR = reg;

    reg &= ~RCC_BDCR_BDRST;
    RCC_BDCR = reg;
}

static void rcc_lse_set_drive_capability(uint32_t lse_drive) {
    uint32_t reg = RCC_BDCR;

    reg &= ~(RCC_BDCR_LSEDRV_MASK << RCC_BDCR_LSEDRV_SHIFT);
    reg |= (lse_drive << RCC_BDCR_LSEDRV_SHIFT);
    RCC_BDCR = reg;
}

static void rcc_lse_enable() {
    uint32_t reg = RCC_BDCR;

    reg |= RCC_BDCR_LSEON;
    RCC_BDCR = reg;

    while ((RCC_BDCR & RCC_BDCR_LSERDY) == 0) {
    }
}

static void rcc_set_rtc_clock_source_lse() {
    if (rcc_ret_rtc_clock_source() != RCC_RTC_CLKSOURCE_LSE) {
        rcc_force_backup_domain_reset();
        rcc_lse_set_drive_capability(RCC_BDCR_LSEDRV_LOW);
        rcc_lse_enable();

        uint32_t reg = RCC_BDCR;

        reg &= ~(RCC_BDCR_RTCSEL_MASK << RCC_BDCR_RTCSEL_SHIFT);
        reg |= (RCC_BDCR_RTCSEL_LSE << RCC_BDCR_RTCSEL_SHIFT);
        RCC_BDCR = reg;
    }
}

static void rcc_rtc_clock_enable() {
    uint32_t reg = RCC_BDCR;

    reg |= RCC_BDCR_RTCEN;
    RCC_BDCR = reg;
}

static uint32_t pwr_is_enabled_backup_access() {
    uint32_t reg = PWR_CR1;

    return reg & PWR_CR1_DBP;
}

static void pwr_enable_backup_access() {
    if (pwr_is_enabled_backup_access() == 0) {
        uint32_t reg = PWR_CR1;

        reg |= PWR_CR1_DBP;
        PWR_CR1 = reg;

        while (pwr_is_enabled_backup_access() == 0) {
        }
    }
}

static void pwr_disable_backup_access() {
    uint32_t reg = PWR_CR1;

    reg &= ~PWR_CR1_DBP;
    PWR_CR1 = reg;
}

static uint32_t rtc_backup_get_register(uint32_t backup_reg) {
    uint32_t tmp;

    tmp = BKP0R;
    tmp += (backup_reg * 4U);

    return *(uint32_t*)tmp;
}

static void rtc_backup_set_register(uint32_t backup_reg, uint32_t backup_reg_val) {
    uint32_t tmp;

    tmp = BKP0R;
    tmp += (backup_reg * 4U);
    *(uint32_t*)tmp = backup_reg_val;
}

static void rtc_enable_write_protection() {
    RTC_WPR = RTC_WRITE_PROTECTION_ENABLE;
}

static void rtc_disable_write_protection() {
    RTC_WPR = RTC_WRITE_PROTECTION_DISABLE_1;
    RTC_WPR = RTC_WRITE_PROTECTION_DISABLE_2;
}

static uint32_t rtc_is_active_flag_init() {
    uint32_t reg = RTC_ISR;

    return reg & RTC_ISR_INITF;
}

static void rtc_enable_init_mode() {
    if (rtc_is_active_flag_init() == 0) {
        uint32_t reg = RTC_ISR;

        reg |= RTC_ISR_INIT;
        RTC_ISR = reg;

        while (rtc_is_active_flag_init() == 0) {
        }
    }
}

static void rtc_disable_init_mode() {
    RTC_ISR = ~RTC_ISR_INIT;
}

static void rtc_set_hour_format(uint32_t format) {
    uint32_t reg = RTC_CR;

    reg &= ~RTC_CR_FMT;
    reg |= (format << RTC_CR_FMT_SHIFT);
    RTC_CR = reg;
}

static void rtc_set_synch_prescaler(uint32_t prescaller) {
    uint32_t reg = RTC_PRER;

    reg &= ~(RTC_PRER_PREDIV_S_MASK << RTC_PRER_PREDIV_S_SHIFT);
    reg |= (prescaller << RTC_PRER_PREDIV_S_SHIFT);
    RTC_PRER = reg;
}

static void rtc_set_asynch_prescaler(uint32_t prescaller) {
    uint32_t reg = RTC_PRER;

    reg &= ~(RTC_PRER_PREDIV_A_MASK << RTC_PRER_PREDIV_A_SHIFT);
    reg |= (prescaller << RTC_PRER_PREDIV_A_SHIFT);
    RTC_PRER = reg;
}

static void rtc_configuration() {
    rtc_disable_write_protection();
    rtc_enable_init_mode();
    rtc_set_hour_format(RTC_HOURFORMAT_24HOUR);
    rtc_set_asynch_prescaler(RTC_ASYNCH_PRESCALER);
    rtc_set_synch_prescaler(RTC_SYNCH_PRESCALER);
    rtc_disable_init_mode();
    rtc_enable_write_protection();
}

static uint32_t rtc_is_shadow_reg_bypass_enabled() {
    uint32_t reg = RTC_CR;

    return reg & RTC_CR_BYPSHAD;
}

static void rtc_clear_flag_rs() {
    uint32_t reg = RTC_ISR;

    reg &= ~(RTC_ISR_RSF);
    RTC_ISR = reg;
}

static uint32_t rtc_is_active_flag_rs() {
    uint32_t reg = RTC_ISR;

    return reg & RTC_ISR_RSF;
}

static void rtc_wait_for_synchro() {
    rtc_clear_flag_rs();

    while (rtc_is_active_flag_rs() != 0) {
    }
}

void rtc_set_time(const rtc_time& time) {
    pwr_enable_backup_access();
    rtc_disable_write_protection();
    rtc_enable_init_mode();

    uint8_t hours = rtc_convert_bin2bcd(time.hours);
    uint8_t minutes = rtc_convert_bin2bcd(time.minutes);
    uint8_t seconds = rtc_convert_bin2bcd(time.seconds);

    uint32_t reg{0};

    if (time.hours_format != RTC_HOURFORMAT_24HOUR) {
        reg |= RTC_TR_PM << RTC_TR_PM_SHIFT;
    }

    reg |= (((hours >> 4) & RTC_TR_HT_MASK) << RTC_TR_HT_SHIFT) |
           ((hours & RTC_TR_HU_MASK) << RTC_TR_HU_SHIFT);

    reg |= (((minutes >> 4) & RTC_TR_MNT_MASK) << RTC_TR_MNT_SHIFT) |
           ((minutes & RTC_TR_MNU_MASK) << RTC_TR_MNU_SHIFT);

    reg |= (((seconds >> 4) & RTC_TR_ST_MASK) << RTC_TR_ST_SHIFT) |
           ((seconds & RTC_TR_SU_MASK) << RTC_TR_SU_SHIFT);

    RTC_TR = reg;

    rtc_disable_init_mode();
    if (rtc_is_shadow_reg_bypass_enabled() == 0) {
        rtc_wait_for_synchro();
    }
    rtc_enable_write_protection();
    pwr_disable_backup_access();
}

void rtc_set_date(const rtc_date& date) {
    pwr_enable_backup_access();
    rtc_disable_write_protection();
    rtc_enable_init_mode();

    uint8_t month = rtc_convert_bin2bcd(date.month);
    uint8_t day = rtc_convert_bin2bcd(date.day);
    uint8_t year = rtc_convert_bin2bcd(date.year);

    uint32_t reg{0};

    reg |= (date.week_day << RTC_DR_WDU_SHIFT);

    reg |= (((month >> 4) & RTC_DR_MT_MASK) << RTC_DR_MT_SHIFT) |
           ((month & RTC_DR_MU_MASK) << RTC_DR_MU_SHIFT);

    reg |= (((day >> 4) & RTC_DR_DT_MASK) << RTC_DR_DT_SHIFT) |
           ((day & RTC_DR_DU_MASK) << RTC_DR_DU_SHIFT);

    reg |= (((year >> 4) & RTC_DR_YT_MASK) << RTC_DR_YT_SHIFT) |
           ((year & RTC_DR_YU_MASK) << RTC_DR_YU_SHIFT);

    RTC_DR = reg;

    rtc_disable_init_mode();
    if (rtc_is_shadow_reg_bypass_enabled() == 0) {
        rtc_wait_for_synchro();
    }
    rtc_enable_write_protection();
    pwr_disable_backup_access();
}

void rtc_get_time(rtc_time& time) {
    uint32_t sec_frac_reg = RTC_SSR;
    uint32_t time_reg = RTC_TR;

    time.hours_format =
            (uint8_t)((time_reg & (RTC_TR_PM << RTC_TR_PM_SHIFT)) >> RTC_TR_PM_SHIFT);
    time.hours = (uint8_t)((time_reg & ((RTC_TR_HT_MASK << RTC_TR_HT_SHIFT) |
                                        (RTC_TR_HU_MASK << RTC_TR_HU_SHIFT))) >>
                           RTC_TR_HU_SHIFT);
    time.minutes = (uint8_t)((time_reg & ((RTC_TR_MNT_MASK << RTC_TR_MNT_SHIFT) |
                                          (RTC_TR_MNU_MASK << RTC_TR_MNU_SHIFT))) >>
                             RTC_TR_MNU_SHIFT);
    time.seconds = (uint8_t)((time_reg & ((RTC_TR_ST_MASK << RTC_TR_ST_SHIFT) |
                                          (RTC_TR_SU_MASK << RTC_TR_SU_SHIFT))) >>
                             RTC_TR_SU_SHIFT);

    time.millisec = static_cast<uint16_t>(
            ((RTC_SYNCH_PRESCALER - static_cast<uint16_t>(sec_frac_reg)) /
             (RTC_SYNCH_PRESCALER + 1.0f)) *
            1000);
}

void rtc_get_date(rtc_date& date) {
    uint32_t reg = RTC_DR;

    date.year = (uint8_t)((reg & ((RTC_DR_YT_MASK << RTC_DR_YT_SHIFT) |
                                  (RTC_DR_YU_MASK << RTC_DR_YU_SHIFT))) >>
                          RTC_DR_YU_SHIFT);
    date.month = (uint8_t)((reg & ((RTC_DR_MT_MASK << RTC_DR_MT_SHIFT) |
                                   (RTC_DR_MU_MASK << RTC_DR_MU_SHIFT))) >>
                           RTC_DR_MU_SHIFT);
    date.week_day = (uint8_t)((reg & ((RTC_DR_WDU_MASK << RTC_DR_WDU_SHIFT))) >>
                              RTC_DR_WDU_SHIFT);
    date.day = (uint8_t)((reg & ((RTC_DR_DT_MASK << RTC_DR_DT_SHIFT) |
                                 (RTC_DR_DU_MASK << RTC_DR_DU_SHIFT))) >>
                         RTC_DR_DU_SHIFT);
}

const rtc_time default_time{.hours_format = RTC_HOURFORMAT_24HOUR,
                            .hours = 0,
                            .minutes = 0,
                            .seconds = 0,
                            .millisec = 0};
const rtc_date default_date{.year = 0, .month = 1, .week_day = 6, .day = 1};

void rtc_init() {
    pwr_enable_backup_access();
    rcc_set_rtc_clock_source_lse();
    rcc_rtc_clock_enable();
    rtc_configuration();
    if (rtc_backup_get_register(RTC_BKP_DR0) != RTC_BKP_DR0_VAL) {
        rtc_set_time(default_time);
        rtc_set_date(default_date);
        rtc_backup_set_register(RTC_BKP_DR0, RTC_BKP_DR0_VAL);
    }
    pwr_disable_backup_access();
}
