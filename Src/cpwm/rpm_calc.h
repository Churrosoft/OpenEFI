#ifndef CPWM_H
#define CPWM_H

#include "stdint.h"
#include "defines.h"
#include "variables.h"
#include <cmath>
#include "../sensors/utils/sinc.h"

namespace RPM
{
   
    void interrupt(void);

    extern uint16_t ckp_tick;
    extern uint32_t t_rpm;
    extern uint32_t t2_rpm;
    extern bool tooth_status;

    extern int32_t _DEG;
    extern float _RPM;

    // calculo de rpms
    void calc(void);
} // namespace RPM

#endif