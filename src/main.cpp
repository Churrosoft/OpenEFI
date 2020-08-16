
#ifdef UNIT_TEST
#include <logger.h>
#endif

#ifdef UNIT_TEST_NANO_FLOAT
#include "../lib/nano-float/test/test.c"
#endif  //  UNIT_TEST

// librerias libopencm3
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/usb/usbd.h>
// Librerias locales (C)
extern "C" {
#include <fast_math.h>
#include "./helpers/utilsTimer.h"
#include "./usb/usb_conf.h"
#include "c_pwm.h"
}
// Librerias locales (C++)
#include "./helpers/timer_schedule.hpp"
#include "./usb/webusb.h"

int main(void) {
  utils_timer_setup();
  c_pwm_setup();

#ifdef UNIT_TEST
  enable_log();  //  Enable logging via Arm Semihosting. Note: ST Link must be
                 //  connected or this line will hang.
  debug_println("Starting...");
  debug_force_flush();
#endif

#ifdef UNIT_TEST_NANO_FLOAT
  test_nanofloat();
#endif  //  UNIT_TEST

  fast_mul_10(45);
  usbd_device* usbd_dev = usb_setup();
  usbd_dev = usb_setup();
  webusb_setup(usbd_dev);

#ifdef UNIT_TEST
  debug_println("Done");
  debug_force_flush();  //  Flush the debug buffer before we halt.
#endif
  for (;;) {
    timer_schedule();
    usbd_poll(usbd_dev);
  }  //  Loop forever.
}
