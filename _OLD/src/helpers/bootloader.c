#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>

#define RTC_BKP_DR(reg)  MMIO16(BACKUP_REGS_BASE + 4 + (4 * (reg)))
void rtc_backup_write(uint8_t reg, uint32_t value);

void rtc_backup_write(uint8_t reg, uint32_t value) {
    rcc_periph_clock_enable(RCC_PWR);
    rcc_periph_clock_enable(RCC_BKP);

    pwr_disable_backup_domain_write_protect();
    RTC_BKP_DR((int)reg*2) = value & 0xFFFFUL;
    RTC_BKP_DR((int)reg*2+1) = (value & 0xFFFF0000UL) >> 16;
    pwr_enable_backup_domain_write_protect();
}