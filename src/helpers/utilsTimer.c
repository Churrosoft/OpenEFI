#include "utilsTimer.h"

UtilsTimer Time = {0, 0, 0};

void sys_tick_handler(void) {
  Time.temp++;
  Time.micros++;
  if (Time.temp == 1000) {
    Time.millis++;
    Time.temp = 0;
  }
}

void utils_timer_setup(void) {
  // FIXME: revisar luego si coinciden los tiempos por el cambio a 48Mhz que
  // hace el USB
  /* 72MHz / 8 => 9000000 counts per second */
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
  /* 9000000/900 = 100 overflows per second - every 0.1ms one interrupt */
  systick_set_reload(899);
  systick_interrupt_enable();
  /* Start counting. */
  systick_counter_enable();
}

uint32_t millis() { return Time.millis; }

uint32_t micros() { return Time.micros; }