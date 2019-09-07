#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/systick.h>
#include <stdio.h>
#include <errno.h>
#include "helpers/micros-millis.cpp"

#include "program.cpp"
#include "defines.h"

/* Aca pongo todas las declaraciones de funciones con parametros porque sino el compilador se pone como una perra*/
int _write(int file, char *ptr, int len);
uint16_t exti_line_state;
static void clock_setup(void){
    /* 8MHz de cristal, 72Mhz de frecuencia de trabajo */
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* GPIOC clock pa el led */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* GPIOC clock pa el USART1 * */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
    /* Activamos la USART1*/
	rcc_periph_clock_enable(RCC_USART1);
}

static void usart_setup(void){
	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, USART1_BAUD);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX);

	/* Finally enable the USART. */
	usart_enable(USART1);
}

static void gpio_setup(void){
    /*Solo seteo los pines para el led en PC13 por ahora */
	gpio_set(GPIOC, GPIO13);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

static void exti_setup(void){
	
	rcc_periph_clock_enable(RCC_GPIOA); /* TODO: habilito clock del GPIA, luego poner todos juntos para no repetir y hacer desastre */
	rcc_periph_clock_enable(RCC_AFIO);/* Enable AFIO clock.*/
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* Set GPIO0 (in GPIO port A) to 'input float'. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI0, GPIOA);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
	exti_enable_request(EXTI0);
}

void exti0_isr(void){
	exti_line_state = GPIOA_IDR;
    I_RPM();
	exti_reset_request(EXTI0);
}


int _write(int file, char *ptr, int len){ /* Para usar printf como manera de imprimir en el serial*/
	int i;
	if (file == 1) {
		for (i = 0; i < len; i++)
			usart_send_blocking(USART1, ptr[i]);
		return i;
	}
	errno = EIO;
	return -1;
}

static void my_delay_1( void ){
   int i = 72e6/2/4;

   while( i > 0 ){
        i--;
        __asm__( "nop" );
     }
}


int main(void){
    /* Setup */
	clock_setup();
	gpio_setup();
    exti_setup();
	usart_setup();
    program_setup();


	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	microsA = 0;
	millisA =  0;
	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(899);

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
	
	while (1) {
        program_loop();
		gpio_toggle(GPIOC, GPIO13);
        my_delay_1();
		printf("Still alive!\r\n");
	}
}
	