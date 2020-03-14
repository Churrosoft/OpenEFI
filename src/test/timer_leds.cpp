#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>

uint16_t ct = 0;
uint16_t ct2 = 0;
uint16_t pines[5] = {GPIO10, GPIO14, GPIO15, GPIO11};
//TIMER VAR:
uint32_t compare_time = 0;
uint32_t new_time = 0;

#define LED1_PORT GPIOC
#define LED1_PIN GPIO13


//TIMER
static void tim_setup(void){
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM2);

	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 8740));

	/* Disable preload. */
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);

	/* count full range, as we'll update compare value continuously */
	timer_set_period(TIM2, 65559);

	/* Set the initual output compare value for OC1. */
	//timer_set_oc_value(TIM2, TIM_OC1, 500);

	/* Counter enable. */
	//timer_enable_counter(TIM2);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	//timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

	gpio_toggle(LED1_PORT, LED1_PIN);
	gpio_clear(GPIOB, GPIO10 | GPIO14 | GPIO15 | GPIO11);

	timer_disable_counter(TIM2);
	timer_disable_irq(TIM2, TIM_DIER_CC1IE);

	
	}
}





//INTERRUPT
/* Set STM32 to 72 MHz. */
static void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

static void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO11);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO10);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);
}

static void exti_setup(void)
{
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

void exti0_isr(void)
{
	ct++;
	if(ct >= 400){
		
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
		new_time = compare_time + 30;

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


int main(void)
{
	clock_setup();
	gpio_setup();
	tim_setup();
	exti_setup();

	while (1)
		__asm("nop");

	return 0;
}