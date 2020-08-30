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

#include "defines.h"
#include "variables.h"
#include "./helpers/c_pwm_utils.h"

#ifndef C_PWM_HEADER
#define C_PWM_HEADER

struct C_PWM
{
    //variables para el control de la inyeccion
    uint16_t inyFlag;
    uint8_t inySubFlagA;   //! se utiliza para manejar el array con los inyectores (encendido)
    uint8_t inySubFlagB;   //! se utiliza para manejar el array con los inyectores (apagado)
    uint16_t inyPins[CIL]; //! array con pines para los inyectores
    uint16_t time;         //! tiempo de inyeccion
//variables para el control de encendido
#if mtr == 1
    uint16_t ecnFlag;
    uint8_t ecnSubFlagA; //! se utiliza para manejar el array con las bobinas (apagado)
    uint8_t ecnSubFlagB; //! se utiliza para manejar el array con las bobinas (encendido)
    uint16_t ecnPins[CIL];
    uint16_t avc; //! avance de encendido
#endif
};
typedef struct C_PWM C_PWM;

extern C_PWM myPWM;

//declaracion de funciones:
void tim_setup(void);             // Inicia el timer2 && timer3
void exti_setup(void);            // Inicia la interrupcion externa en el pin PB0
void new_iny_time(unsigned long); // setea el tiempo proximo para el ISR del TIM2
void new_ecn_time(unsigned long); // setea el tiempo proximo para el ISR del TIM3
void c_pwm_setup(void);           // Inicia los pines, configura el ISR para las interrupciones e inicia el timer

#endif