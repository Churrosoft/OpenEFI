#include "rpm_calc.h"

#pragma GCC push_options
#pragma GCC optimize("O0")

uint16_t RPM::WheelTooth;
uint32_t RPM::lastWheelTime;
uint32_t RPM::actualWheelTime;
RPM_STATUS RPM::status = RPM_STATUS::STOPPED;

uint32_t RPM::rotation_count;
uint32_t RPM::last_rotation_count;
uint8_t RPM::watch_dog_count;

uint32_t RPM::watch_dog_time;
uint32_t RPM::watch_dog_period = 250;

float RPM::_RPM;
float RPM::_DEG;

/* uint32_t getCurrentMicros(void)
{
  Ensure COUNTFLAG is reset by reading SysTick control and status register
  LL_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (LL_SYSTICK_IsActiveCounterFlag()) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
} */

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

    EFI_INVERT_PIN(LED0_GPIO_Port, LED0_Pin);

    if (lastWheelTime == 0) {
      lastWheelTime = GET_US_TIME;
    } else {
      actualWheelTime = GET_US_TIME;
      RPM::_RPM = US_IN_MINUTE / (actualWheelTime - lastWheelTime);
      lastWheelTime = GET_US_TIME;

      if (RPM::_RPM < 50) {
        RPM::status = RPM_STATUS::STOPPED;
      } else if (RPM::_RPM > 50 && RPM::_RPM < 400) {
        RPM::status = RPM_STATUS::SPIN_UP;
      } else if (RPM::_RPM > 400 && RPM::_RPM < 750) {
        RPM::status = RPM_STATUS::CRANK;
      } else {
        RPM::status = RPM_STATUS::RUNNING;
      }
    }

    RPM::WheelTooth = 0;
    RPM::rotation_count++;
  } else {
    RPM::WheelTooth++;
  }

#endif

  RPM::_DEG = ((360 / LOGIC_DNT) * (RPM::WheelTooth)) * 100;
}

void RPM::watch_dog() {

  if (HAL_GetTick() - RPM::watch_dog_time >= RPM::watch_dog_period) {

    RPM::watch_dog_time = HAL_GetTick();

    if (RPM::rotation_count == RPM::last_rotation_count) {

      if (RPM::watch_dog_count > 5) {
        RPM::_RPM = 0;
        RPM::rotation_count = 0;
        RPM::last_rotation_count = 0;
        RPM::watch_dog_count = 0;
      }
      RPM::watch_dog_count++;
    }

    RPM::last_rotation_count = RPM::rotation_count;
  }
}

#pragma GCC pop_options
