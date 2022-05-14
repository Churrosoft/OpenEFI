#include "rpm_calc.h"

#pragma GCC push_options
#pragma GCC optimize("O0")

void RPM::interrupt() {

  if ((CKP_GPIO_Port->IDR & CKP_Pin)) {
    RPM::t2_rpm = GetMicrosFromISR();
  } else {
    RPM::t_rpm = GetMicrosFromISR();
  }
  // ((b-a)/a)*100
  uint32_t rpmdiff = ((RPM::t2_rpm - RPM::t_rpm) / RPM::t_rpm) * 100;

  // mas de 70 => se paro el motor
  if (rpmdiff < 5 && rpmdiff > 70) {
    // ((t2 - t1) / 1000000) * DNT * 60
    RPM::_RPM = ((RPM::t2_rpm - RPM::t_rpm) / 1000000) * DNT * 60;
  }

  if (RPM::ckp_tick >= LOGIC_DNT * 2) {
    RPM::ckp_tick = 0;
  } else {
    RPM::ckp_tick++;
  }
}

void RPM::calc() { RPM::_DEG = ((360 / LOGIC_DNT) * RPM::ckp_tick) * 100; }

#pragma GCC pop_options
