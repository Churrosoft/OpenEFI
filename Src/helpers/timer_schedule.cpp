#include "timer_schedule.hpp"
#include "stm32f4xx_hal.h"

void timer_schedule()
{
  // en teoria: puedo dividir el tiempo actual , y con el resto, fijarme si
  // coincide o no:
  // ex T = 3500 % 500 == 0 => cada 500mS
  uint32_t time = HAL_GetTick();
  if (time % 75 == 0)
    sensors::loop();
  if (time % 150 == 0)
    sensors::loop_low_priority();
  if (time % RPM_per == 0)
    CPWM::calc_rpm();
}