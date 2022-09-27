#include "./cpwm.hpp"

#include <stdio.h>

#include <string>

extern "C" {
#ifdef TESTING
#include <trace.h>
#include <unity.h>
#endif
}

#define USE_NEW_RPM_CALC

uint32_t _POS;
uint32_t _RPM;
uint32_t _rpm_time = 0;

uint16_t CPWM::iny_time = 15;
uint16_t CPWM::iny_pin = 0;

uint16_t CPWM::eng_pin = 0;

uint_fast16_t CPWM::avc_deg = 15;
uint16_t CPWM::avc_time = 0;

uint16_t CPWM::ckp_tick = 0;
float CPWM::ckp_deg = 4.56f;

TIM_HandleTypeDef CPWM::c_tim3;
TIM_HandleTypeDef CPWM::c_tim4;

void CPWM::set_iny(uint16_t value) { iny_time = value; }

void CPWM::set_avc(float deg, uint16_t time) {
  CPWM::avc_deg = grad_to_dnt(deg);
  CPWM::avc_time = time;
}

void CPWM::write_iny(uint8_t chanel, uint8_t pinState) {
  uint16_t iny_sec[CIL] = INY_SECUENCY_FULL_MPI;
// esto luego se pasa a SPI viteh'
#ifdef TESTING
  switch (chanel) {
    case 0:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(49, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    case 1:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(99, CPWM::ckp_tick, "Check AVCPER / ckp_deg");

      break;
    case 2:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(149, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    case 3:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(199, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    default:
      break;
  }
#else
  switch (iny_sec[chanel] - 1) {
    case 0:
      HAL_GPIO_WritePin(INY1_GPIO_Port, INY1_Pin, (GPIO_PinState)pinState);
      break;
    case 1:
      HAL_GPIO_WritePin(INY2_GPIO_Port, INY2_Pin, (GPIO_PinState)pinState);
      break;
    case 2:
      HAL_GPIO_WritePin(INY3_GPIO_Port, INY3_Pin, (GPIO_PinState)pinState);
      break;
    case 3:
      HAL_GPIO_WritePin(INY4_GPIO_Port, INY4_Pin, (GPIO_PinState)pinState);
      break;
    default:
      break;
  }
#endif
}

void CPWM::write_ecn(uint8_t chanel, uint8_t pinState) {
  uint16_t ecn_sec[CIL] = ING_SECUENCY;

#ifdef TESTING
  switch (chanel) {
    case 0:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(56, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    case 1:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(115, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    case 2:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(173, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    case 3:
      TEST_ASSERT_EQUAL_INT16_MESSAGE(232, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
      break;
    default:
      break;
  }
#else
  switch (ecn_sec[chanel] - 1) {
    case 0:
      HAL_GPIO_WritePin(ECN1_GPIO_Port, ECN1_Pin, (GPIO_PinState)pinState);
      break;
    case 1:
      HAL_GPIO_WritePin(ECN2_GPIO_Port, ECN2_Pin, (GPIO_PinState)pinState);
      break;
    case 2:
      HAL_GPIO_WritePin(ECN3_GPIO_Port, ECN3_Pin, (GPIO_PinState)pinState);
      break;
    case 3:
      HAL_GPIO_WritePin(ECN4_GPIO_Port, ECN4_Pin, (GPIO_PinState)pinState);
      break;
    default:
      break;
  }
#endif
}

void CPWM::interrupt() {
  CPWM::ckp_deg = ROUND_16((360 / LOGIC_DNT) * CPWM::ckp_tick);

  if (abs(CPWM::ckp_deg - (AVCPER - AVCI) * (CPWM::iny_pin + 1)) <= 5) {
#ifdef TESTING

    trace_printf("------------------------------------\n");
    trace_printf("INY on cil:%d\nDEG: %d.%d \nTICK: %d \n", CPWM::iny_pin, (int16_t)CPWM::ckp_deg,
                 // odio formatear numeros en C
                 ((int16_t)(CPWM::ckp_deg - (int16_t)CPWM::ckp_deg) * 100), CPWM::ckp_tick);
    trace_printf("------------------------------------\n");

    CPWM::write_iny(CPWM::iny_pin, GPIO_PIN_SET);
#else

    CPWM::write_iny(CPWM::iny_pin, GPIO_PIN_SET);

    CPWM::c_tim3.Instance = TIM3;
    CPWM::c_tim3.Init.Prescaler = 1200;
    CPWM::c_tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    CPWM::c_tim3.Init.Period = CPWM::iny_time;
    CPWM::c_tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    CPWM::c_tim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Start_IT(&CPWM::c_tim3) != HAL_OK) Error_Handler();
#endif
#ifdef TESTING
    // esto solo hace falta cuando estamos en el modo testing porque no disponemos del timer
    if (CPWM::iny_pin < L_CIL)
      CPWM::iny_pin++;
    else
      CPWM::iny_pin = 0;
#endif
  }

  if (abs(CPWM::ckp_deg - (AVCPER - CPWM::avc_deg) * (CPWM::eng_pin + 1)) <= 5) {
#ifdef TESTING
    trace_printf("------------------------------------\n");
    trace_printf("EGN on cil:%d\nDEG: %d.%d \nTICK: %d \n", CPWM::eng_pin, (int16_t)CPWM::ckp_deg,
                 // odio formatear numeros en C
                 ((int16_t)(CPWM::ckp_deg - (int16_t)CPWM::ckp_deg) * 100), CPWM::ckp_tick);
    trace_printf("------------------------------------\n");
    CPWM::write_ecn(CPWM::eng_pin, GPIO_PIN_SET);
#else
    CPWM::write_ecn(CPWM::eng_pin, GPIO_PIN_SET);
    htim4.Instance = TIM4;
    CPWM::c_tim4.Init.Prescaler = 6000;
    CPWM::c_tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    CPWM::c_tim4.Init.Period = IGNITION_DWELL_TIME;
    CPWM::c_tim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    CPWM::c_tim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim4) != HAL_OK) Error_Handler();
#endif

#ifdef TESTING

    if (CPWM::eng_pin < L_CIL)
      CPWM::eng_pin++;
    else
      CPWM::eng_pin = 0;
#endif
  }

  if (CPWM::ckp_tick >= LOGIC_DNT * 2)
    CPWM::ckp_tick = 0;
  else
    CPWM::ckp_tick++;
}

void CPWM::tim3_irq() {
  CPWM::write_iny(CPWM::iny_pin, GPIO_PIN_RESET);

  if (CPWM::iny_pin < L_CIL && INJECTION_STRATEGY != INJECTION_MODE_SPI)
    CPWM::iny_pin++;
  else
    CPWM::iny_pin = 0;
  HAL_TIM_Base_Stop_IT(&CPWM::c_tim3);
}

void CPWM::tim4_irq() {
  CPWM::write_ecn(CPWM::eng_pin, GPIO_PIN_RESET);

  // Modo secuencial:
  if (CPWM::eng_pin < L_CIL && IGNITION_STRATEGY == IGNITION_MODE_SECUENCIAL)
    CPWM::eng_pin++;
  else if (CPWM::eng_pin >= L_CIL && IGNITION_STRATEGY == IGNITION_MODE_SECUENCIAL) {
    CPWM::eng_pin = 0;
  }

  // Modo wasted-spark
  if (CPWM::eng_pin < (L_CIL / 2) && IGNITION_STRATEGY == IGNITION_MODE_WASTED_SPARK) {
    CPWM::eng_pin++;
  } else if (CPWM::eng_pin >= (L_CIL / 2) && IGNITION_STRATEGY == IGNITION_MODE_WASTED_SPARK) {
    CPWM::eng_pin = 0;
  }

  HAL_TIM_Base_Stop_IT(&CPWM::c_tim4);
}
