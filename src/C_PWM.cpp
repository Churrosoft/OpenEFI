/*
	** Esta wea se encarga del PWM para inyectores y bobinas, ademas de la sincronizacion del cigueñal
*/
uint16_t ct = 0;
uint16_t ct2 = 0;
uint16_t pines[5] = {GPIO12, GPIO14, GPIO15, GPIO13};
//TIMER VAR:
uint32_t compare_time = 0;
uint32_t new_time = 0;

#define LED1_PORT GPIOC
#define LED1_PIN GPIO13


//TIMER
static void tim_setup(void){
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	rcc_periph_reset_pulse(RST_TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

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

void tim2_isr(void){
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

	gpio_toggle(LED1_PORT, LED1_PIN);
	gpio_clear(GPIOB, GPIO12 | GPIO14 | GPIO15 | GPIO13);

	timer_disable_counter(TIM2);
	timer_disable_irq(TIM2, TIM_DIER_CC1IE);

	
	}
}

//INTERRUPT
static void exti_setup(void){
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOB);

	/* Enable AFIO clock. */
	rcc_periph_clock_enable(RCC_AFIO);

	/* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* Set GPIO0 (in GPIO port A) to 'input float'. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI0, GPIOB);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
	exti_enable_request(EXTI0);
}

void exti0_isr(void){
	ct++;
	if(ct >= 120){
		
		gpio_set(GPIOB, pines[ct2]);
		ct = 0;
		ct2++;
		
		//CHIMER
			/*
		 * Get current timer value to calculate next
		 * compare register value.
		 */
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
