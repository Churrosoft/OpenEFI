#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <errno.h>
#include "program.cpp"

int _write(int file, char *ptr, int len);

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
	usart_set_baudrate(USART1, 230400);
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

int _write(int file, char *ptr, int len){
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
	usart_setup();
    program_setup();

	while (1) {
        program_loop();
		gpio_toggle(GPIOC, GPIO13);
        my_delay_1();
		printf("Still alive! %i %f %f\r\n");
	}
}
	