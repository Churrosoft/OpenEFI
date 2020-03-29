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

struct C_PWM{
	//variables para el control de la inyeccion
	uint16_t inyFlag;
	uint8_t  inySubFlagA;  // se utiliza para manejar el array con los inyectores (encendido)
	uint8_t  inySubFlagB;  // se utiliza para manejar el array con los inyectores (apagado)
	uint16_t inyPins[CIL]; // array con pines para los inyectores
	uint16_t time;         //tiempo de inyeccion
	//variables para el control de encendido
	#if mtr == 1
		uint16_t ecnFlag;
		uint8_t  ecnSubFlagA; // se utiliza para manejar el array con las bobinas (apagado)
		uint8_t  ecnSubFlagB; // se utiliza para manejar el array con las bobinas (encendido)
		uint16_t ecnPins[CIL];
		uint16_t avc; // avance de encendido
	#endif

}myPWM = {
	0,0,0,C_PWM_INY,150
	#if mtr == 1
	,0,0,0,C_PWM_ECN,15
	#endif
};

// Este led se prende/apaga con cada interrupcion externa
#define LED1_PORT GPIOC
#define LED1_PIN GPIO13

//declaracion de funciones:
static void tim_setup(void);	// Inicia el timer2 && timer3
static void exti_setup(void);	// Inicia la interrupcion externa en el pin PB0
void new_iny_time(unsigned long);   // setea el tiempo proximo para el ISR del TIM2
void new_ecn_time(unsigned long);   // setea el tiempo proximo para el ISR del TIM3
/*
 * Inicia los pines, configura el ISR para las interrupciones e inicia el timer
 */
static void c_pwm_setup(void){
	tim_setup();
	exti_setup();
	/* Configure INY outputs*/
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);
}

//TIMER's
static void tim_setup(){
	// TIMER 2
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
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency ) / 17580));
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2, 65599);
	// TIMER 3
	rcc_periph_clock_enable(RCC_TIM3);
	nvic_enable_irq(NVIC_TIM3_IRQ);
	rcc_periph_reset_pulse(RST_TIM3);
	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	//FIXME con el reloj a 48Mhz por el usb estos tiempos cambian, calcular tick a 0.5ms de ser posible
	/*
	 * 17580 = 1,139mS period 65599
	 * 17599 = 1.138mS period 65599
	 * 8800  = 0.8mS period 65599
	 */
	timer_set_prescaler(TIM3, ((rcc_apb1_frequency ) / 17580));
	timer_disable_preload(TIM3);
	timer_continuous_mode(TIM3);
	timer_set_period(TIM3, 65599);

}

void tim2_isr(){
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);
		gpio_toggle(LED1_PORT, LED1_PIN);
		//BUG con el cruce del motor *puede* pasar que se apaguen los inyectores antes de tiempo
		gpio_clear(GPIOA, myPWM.inyPins[myPWM.inySubFlagB]);
		myPWM.inySubFlagB++;
		if(myPWM.inySubFlagB > CIL) myPWM.inySubFlagB = 0;
		timer_disable_counter(TIM2);
		timer_disable_irq(TIM2, TIM_DIER_CC1IE);
	}
}

void tim3_isr(){
	if (timer_get_flag(TIM3, TIM_SR_CC3IF)) {
		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM3, TIM_SR_CC2IF);
		gpio_set(GPIOC, myPWM.ecnPins[myPWM.ecnSubFlagB]);
		myPWM.ecnSubFlagB++;
		if(myPWM.ecnSubFlagB > (L_CIL) ) myPWM.ecnSubFlagB = 0;
		timer_disable_counter(TIM3);
		timer_disable_irq(TIM3, TIM_DIER_CC2IE);
	}
}

//INTERRUPT
static void exti_setup(){
	/* Enable GPIOA | AFI0 clock. */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_AFIO);
	/* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* Set GPIO0 (in GPIO port A) to 'input float'. */
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI0, GPIOB);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
	exti_enable_request(EXTI0);

}

void new_iny_time(unsigned long nTime){
	/* Calculate and set the next compare value. */
	timer_set_oc_value(TIM2, TIM_OC1, ( timer_get_counter(TIM2) + nTime));
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
	timer_enable_counter(TIM2);
}

void new_ecn_time(unsigned long nTime){
	/* Calculate and set the next compare value. */
	timer_set_oc_value(TIM3, TIM_OC2, ( timer_get_counter(TIM3) + nTime));
	timer_enable_irq(TIM3, TIM_DIER_CC2IE);
	timer_enable_counter(TIM3);
}

void exti0_isr(){
	myPWM.inyFlag++;
	if(myPWM.inyFlag >= (PMSI - AVCI)){
		gpio_set(GPIOC, GPIO14 | GPIO15);
		gpio_set(GPIOA, myPWM.inyPins[myPWM.inySubFlagA]);
		//CHIMER
		new_iny_time(myPWM.time);
		myPWM.inySubFlagA++;
		if(myPWM.inySubFlagA > CIL) myPWM.inySubFlagA = 0;
		myPWM.inyFlag = 0;
	}
#if mtr == 1
	myPWM.ecnFlag++;
	if(myPWM.ecnFlag >= (PMSI - myPWM.avc)){
		gpio_clear(GPIOC, myPWM.ecnPins[myPWM.ecnSubFlagA]);
		//CHIMER
		new_ecn_time(ECNT);
		myPWM.ecnSubFlagA++;
		if(myPWM.ecnSubFlagA > (CIL/2) ) myPWM.ecnSubFlagA = 0;
		myPWM.ecnFlag = 0;
	}
#endif
	exti_reset_request(EXTI0);
}
