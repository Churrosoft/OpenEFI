#include "timer_schedule.hpp"

void timer_schedule() {
  // en teoria: puedo dividir el tiempo actual , y con el resto, fijarme si
  // coincide o no:
  // ex T = 3500 % 500 == 0 => cada 500mS
  if (Time.millis % 75 == 0)
    sensors::loop();
  if (Time.millis % 150 == 0)
    sensors::loop_low_priority();
  if (Time.millis % RPM_per == 0)
    RPM();
}