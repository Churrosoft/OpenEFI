/*
	** Esta wea se encarga del PWM para inyectores y bobinas, ademas de la sincronizacion del cigueñal
*/

#include "defines.h"

// libopencm3:
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>

uint16_t ct = 0;
uint16_t ct2 = 0;
uint16_t pines[L_CIL] = C_PWM_INY;
//TIMER VAR:
uint32_t compare_time = 0;
uint32_t new_time = 0;

// Este led se prende/apaga con cada interrupcion externa
#define LED1_PORT GPIOC
#define LED1_PIN GPIO13

//declaracion de funciones:
static void tim_setup(void);	// Inicia el timer2
void tim2_isr(void);			// ISR del timer2
static void exti_setup(void);	// Inicia la interrupcion externa en el pin PA1
void exti0_isr(void);			// ISR de la interrupcion del pin PA1

/*
 * Inicia los pines, configura el ISR para las interrupciones e inicia el timer
 */
static void c_pwm_setup(void){
	tim_setup();
	exti_setup();
}

//TIMER
static void tim_setup(){
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	rcc_periph_reset_pulse(RST_TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	//FIXME con el reloj a 48Mhz por el usb estos tiempos cambian, calcular tick a 0.5ms de ser posible
	/*
	 * 17580 = 1,139mS period 65599
	 * 17599 = 1.138mS period 65599
	 * 8800  = 0.8mS period 65599
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 8800));
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2, 65599);

}

void tim2_isr(){
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

	gpio_toggle(LED1_PORT, LED1_PIN);
	//BUG con el cruce del motor *puede* pasar que se apaguen los inyectores antes de tiempo
	gpio_clear(GPIOB, GPIO12 | GPIO14 | GPIO15 | GPIO13);

	timer_disable_counter(TIM2);
	timer_disable_irq(TIM2, TIM_DIER_CC1IE);

	
	}
}

//INTERRUPT
static void exti_setup(){
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOB);

	/* Enable AFIO clock. */
	rcc_periph_clock_enable(RCC_AFIO);

	/* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* Set GPIO0 (in GPI1 port A) to 'input float'. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO1);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI0, GPIOB);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
	exti_enable_request(EXTI0);
}

void exti0_isr(){
	ct++;
	//FIXME esto esta horriblemente mal
	if(ct >= 120){
		
		gpio_set(GPIOB, pines[ct2]);
		ct = 0;
		ct2++;
		
		//CHIMER
		compare_time = timer_get_counter(TIM2);

		/* Calculate and set the next compare value. */
		new_time = compare_time + 7;

		timer_set_oc_value(TIM2, TIM_OC1, new_time);

		timer_enable_counter(TIM2);
		timer_enable_irq(TIM2, TIM_DIER_CC1IE);
		//FIN
		if(ct2 == 4){
			ct2 = 0;
		}
	}

	exti_reset_request(EXTI0);
}
