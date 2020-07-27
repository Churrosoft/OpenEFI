#ifdef UNIT_TEST
#include <logger.h>
#endif

#include <fast_math.h>

#ifdef UNIT_TEST_NANO_FLOAT
#include "../lib/nano-float/test/test.c"
#endif //  UNIT_TEST

// librerias libopencm3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
// Librerias locales
/* #include "defines.h" */
extern "C" {
 #include "./usb/usb_conf.h"
}

#include "./usb/webusb.h"

int main(void)
{
#ifdef UNIT_TEST
	enable_log(); //  Enable logging via Arm Semihosting. Note: ST Link must be connected or this line will hang.
	debug_println("Starting...");
	debug_force_flush();
#endif

#ifdef UNIT_TEST_NANO_FLOAT
	test_nanofloat();
#endif //  UNIT_TEST

#ifdef UNIT_TEST
	debug_println("Done");
	debug_force_flush(); //  Flush the debug buffer before we halt.
#endif

	/* fast_mul_10(45); */
	usbd_device *usbd_dev = usb_setup();
	usbd_dev = usb_setup();
	webusb_setup(usbd_dev);

	for (;;)
	{
	} //  Loop forever.
}
