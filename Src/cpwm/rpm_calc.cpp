#include "rpm_calc.h"

#pragma GCC push_options
#pragma GCC optimize("O0")

uint16_t RPM::WheelTooth;
uint32_t RPM::lastWheelTime;
uint32_t RPM::actualWheelTime;

float RPM::_RPM;
float RPM::_DEG;

void RPM::interrupt() {

#if USING_EXPERIMENTAL_RPM_CALC
  if ((CKP_GPIO_Port->IDR & CKP_Pin)) {
    RPM::t2_rpm = (HAL_GetTick() * 1000) + TIM13->CNT;
  } else {
    RPM::t_rpm = (HAL_GetTick() * 1000) + TIM13->CNT;
  }
  // ((b-a)/a)*100
  uint32_t rpmdiff = ((RPM::t2_rpm - RPM::t_rpm) / RPM::t_rpm) * 10000;

  // mas de 70 => se paro el motor
  if (rpmdiff > 5 && rpmdiff < 70) {
    RPM::_RPM = ((RPM::t2_rpm - RPM::t_rpm) / 1000000) * DNT * 60;
  }

  trace_printf("T1 %d, T2 %d ", RPM::t_rpm, RPM::t2_rpm);
  trace_printf("RPM diff %f", RPM::t2_rpm - RPM::t_rpm);

  if (RPM::WheelTooth >= LOGIC_DNT * 2) {
    RPM::WheelTooth = 0;
  } else {
    RPM::WheelTooth++;
  }
#else

  if (RPM::WheelTooth >= LOGIC_DNT) {

    if (lastWheelTime == 0) {
      lastWheelTime = GET_US_TIME;
    } else {
      actualWheelTime = GET_US_TIME;
      RPM::_RPM = US_IN_MINUTE / (actualWheelTime - lastWheelTime);
      lastWheelTime = GET_US_TIME;
    }

    RPM::WheelTooth = 0;

  } else {
    RPM::WheelTooth++;
  }

#endif

  RPM::_DEG = ((360 / LOGIC_DNT) * (RPM::WheelTooth)) * 100;
}

#pragma GCC pop_options
