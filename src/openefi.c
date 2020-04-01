// librerias C
#include <stdlib.h>
// librerias libopencm3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
// Librerias locales
#include "defines.h"
#include "./helpers/utilsTimer.c"
#include "./usb/usb_conf.c"
#include "./usb/webusb.c"
//WIP: Sensores/ADC
#include "./sensores/utils/input_handler.c"

#define CPWM_ENABLE

#ifdef CPWM_ENABLE
#include "./C_PWM.c"
#endif

int main(void){
	int i;
	// setup inicial de clock, IO y USB
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set(GPIOC, GPIO13);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	// iniciamos el systimer asi podemos contar tambien el bootime:
	utils_timer_setup();
	adc_setup(); //WIP: Sensores/ADC
	usbd_device* usbd_dev = usb_setup();
	usbd_dev = usb_setup();

	webusb_setup(usbd_dev);
	// setup de C_PWM:
	#ifdef CPWM_ENABLE
	c_pwm_setup();
	#endif
	gpio_clear(GPIOC, GPIO13);
	while (1){
		for (i = 0; i < 0x800000; i++){
			usbd_poll(usbd_dev);
		}
		get_input(0); //WIP: Sensores/ADC
		// TODO: resto del ciclo (obtener data de sensores, calculo de tiempos, etc)
	}
}