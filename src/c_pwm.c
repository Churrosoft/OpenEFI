/*
 ** Esta wea se encarga del PWM para inyectores y bobinas, ademas de la
 *  sincronizacion del cigueñal
 */
#include "c_pwm.h"
#include "variables.h"

bool SINC = false;
bool sincB = false;
unsigned long T1 = 0;
unsigned long T2 = 0;
unsigned long Ta = 0;
unsigned long Tb = 0;

C_PWM myPWM = {
	0, 0, 0, C_PWM_INY, 1500,
#if mtr == 1
    0, 0, 0, C_PWM_ECN, 15
#endif
};

void c_pwm_setup(void) {
  tim_setup();
  exti_setup();
  /* Configure INY outputs*/
  rcc_periph_clock_enable(RCC_GPIOB);
  for (uint8_t i = 0; i < CIL; i++) {
    gpio_set_mode(C_PWM_INY_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, myPWM.inyPins[i]);
#if mtr == 1
    if (i < (CIL / 2)) {
      gpio_set_mode(C_PWM_ECN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                    GPIO_CNF_OUTPUT_PUSHPULL, myPWM.ecnPins[i]);
    }
#endif
  }
}

// TIMER's
void tim_setup() {
  // TIMER 2
  rcc_periph_clock_enable(RCC_TIM2);
  nvic_enable_irq(NVIC_TIM2_IRQ);
  rcc_periph_reset_pulse(RST_TIM2);
  timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
  // FIXME con el reloj a 48Mhz por el usb estos tiempos cambian, calcular tick
  // a 0.5ms de ser posible
  /*
   * 17580 = 1,139mS period 65599 8000000
   * 17599 = 1.138mS period 65599
   * 8800  = 0.8mS period 65599
   * var on 500 uS (test time):  	| 1500 uS
   * 32000: period
   * 450000: prescaler
   * 564.6 uS: realtime			| 1690 uS: realtime
   */
  timer_set_prescaler(TIM2, ((rcc_apb1_frequency) / 502400));
  timer_disable_preload(TIM2);
  timer_continuous_mode(TIM2);
  timer_set_period(TIM2, 32000);
  // TIMER 3
  rcc_periph_clock_enable(RCC_TIM3);
  nvic_enable_irq(NVIC_TIM3_IRQ);
  rcc_periph_reset_pulse(RST_TIM3);
  timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
  // FIXME con el reloj a 48Mhz por el usb estos tiempos cambian, calcular tick
  // a 0.5ms de ser posible
  /*
   * 17580 = 1,139mS period 65599
   * 17599 = 1.138mS period 65599
   * 8800  = 0.8mS period 65599
   */
  timer_set_prescaler(TIM3, ((rcc_apb1_frequency) / 17580));
  timer_disable_preload(TIM3);
  timer_continuous_mode(TIM3);
  timer_set_period(TIM3, 65599);
}

void tim2_isr() {
  if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
    /* Clear compare interrupt flag. */
    timer_clear_flag(TIM2, TIM_SR_CC1IF);
    // BUG con el cruce del motor *puede* pasar que se apaguen los inyectores
    // antes de tiempo
    // gpio_clear(C_PWM_INY_PORT, myPWM.inyPins[myPWM.inySubFlagB]);
    myPWM.inySubFlagB++;
    if (myPWM.inySubFlagB > CIL)
      myPWM.inySubFlagB = 0;
    timer_disable_counter(TIM2);
    timer_disable_irq(TIM2, TIM_DIER_CC1IE);
  }
}

void tim3_isr() {
  if (timer_get_flag(TIM3, TIM_SR_CC3IF)) {
    /* Clear compare interrupt flag. */

    timer_clear_flag(TIM3, TIM_SR_CC2IF);
    gpio_set(C_PWM_ECN_PORT, myPWM.ecnPins[myPWM.ecnSubFlagB]);
    myPWM.ecnSubFlagB++;
    if (myPWM.ecnSubFlagB >= (CIL / 2))
      myPWM.ecnSubFlagB = 0;
    timer_disable_counter(TIM3);
    timer_disable_irq(TIM3, TIM_DIER_CC2IE);
  }
}

// INTERRUPT
void exti_setup() {
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

void new_iny_time(unsigned long nTime) {
  /* Calculate and set the next compare value. */
  timer_set_oc_value(TIM2, TIM_OC1, (timer_get_counter(TIM2) + nTime));
  timer_enable_irq(TIM2, TIM_DIER_CC1IE);
  timer_enable_counter(TIM2);
}

void new_ecn_time(unsigned long nTime) {
  /* Calculate and set the next compare value. */
  timer_set_oc_value(TIM3, TIM_OC2, (timer_get_counter(TIM3) + nTime));
  timer_enable_irq(TIM3, TIM_DIER_CC2IE);
  timer_enable_counter(TIM3);
}

void exti0_isr() {
  SINC = sinc();
  if (SINC) {
    myPWM.inyFlag++;
    if (myPWM.inyFlag >= (PMSI - AVCI)) {
      if (myPWM.inySubFlagA > CIL)
        myPWM.inySubFlagA = 0;
      gpio_set(C_PWM_INY_PORT, myPWM.inyPins[myPWM.inySubFlagA]);
      // CHIMER
      new_iny_time(myPWM.time);
      myPWM.inySubFlagA++;
      myPWM.inyFlag = 0;
    }
#if mtr == 1
    myPWM.ecnFlag++;
    if (myPWM.ecnFlag >= (PMSI - myPWM.avc)) {
      gpio_set(C_PWM_ECN_PORT, myPWM.ecnPins[myPWM.ecnSubFlagA]);
      new_ecn_time(ECNT);
      myPWM.ecnSubFlagA++;
      if (myPWM.ecnSubFlagA > (CIL / 2))
        myPWM.ecnSubFlagA = 0;
      myPWM.ecnFlag = 0;
    }
#endif
  }
  exti_reset_request(EXTI0);
}
