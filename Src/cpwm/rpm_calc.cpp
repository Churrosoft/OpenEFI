#include "rpm_calc.h"

extern "C"{
#include "trace.h"
}
#pragma GCC push_options
#pragma GCC optimize("O0")

uint16_t RPM::ckp_tick;
uint32_t RPM::t_rpm;
uint32_t RPM::t2_rpm;
float RPM::_RPM;
int32_t RPM::_DEG;

bool t = false;

void RPM::interrupt() {
//(CKP_GPIO_Port->IDR & CKP_Pin)

  if ((CKP_GPIO_Port->IDR & CKP_Pin)) {
    t=false;
    RPM::t2_rpm = (HAL_GetTick()*1000) + TIM13->CNT;
  } else {
    t=true;
    RPM::t_rpm =  (HAL_GetTick()*1000) + TIM13->CNT;
  }
  // ((b-a)/a)*100
  uint32_t rpmdiff = ((RPM::t2_rpm - RPM::t_rpm) / RPM::t_rpm) * 10000;

  // mas de 70 => se paro el motor
  if ((rpmdiff > 5 && rpmdiff < 70) || true) {
    // ((t2 - t1) / 1000000) * DNT * 60
  }
  RPM::_RPM = ((RPM::t2_rpm - RPM::t_rpm) / 1000000) * DNT * 60;

  trace_printf("T1 %d, T2 %d ", RPM::t_rpm, RPM::t2_rpm);
  trace_printf("RPM diff %f",RPM::t2_rpm - RPM::t_rpm);

  if (RPM::ckp_tick >= LOGIC_DNT * 2) {
    RPM::ckp_tick = 0;
  } else {
    RPM::ckp_tick++;
  }

  RPM::calc();
}

void RPM::calc() {
  RPM::_DEG = ((360 / LOGIC_DNT) * (RPM::ckp_tick / 2)) * 100;
}

#pragma GCC pop_options
