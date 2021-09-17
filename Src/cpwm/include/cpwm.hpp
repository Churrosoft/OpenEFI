#ifndef CPWM_H
#define CPWM_H

#include "stdint.h"
#include "defines.h"
#include "variables.h"
#include <cmath>
#include "../../sensors/utils/sinc.h"

namespace CPWM
{
    void set_iny(uint16_t);
    void set_avc(float, uint16_t);
    void setup(void);
    void interrupt(void);
    //void write_iny(uint8_t, uint8_t);
    // private vars:
    extern uint16_t iny_time;
    // la inyeccion se hace en un 2-step (inyeccion a X grados - avance de inyeccion), el bool sirve para distinguir entre las dos etapas
    extern bool iny_flag;
    extern uint16_t iny_pin;
    extern uint16_t eng_pin;

    // en el encendido, se utilizan grados y tiempo en uS para ser mas exactos
    // (min 1.55° sin tiempo y usando falling/rising , falling 3.10°)
    extern uint_fast16_t avc_deg;
    extern uint16_t avc_time;
    extern uint16_t ckp_tick;
    extern float ckp_deg;
    void write_iny(uint8_t, uint8_t);
    void write_ecn(uint8_t, uint8_t);

    extern TIM_HandleTypeDef c_tim3;
    extern TIM_HandleTypeDef c_tim4;

    void tim3_irq();
    void tim4_irq();

    // calculo de rpms
    void calc_rpm(void);
} // namespace CPWM

#endif