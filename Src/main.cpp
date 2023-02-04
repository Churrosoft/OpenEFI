/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#ifdef TESTING
#pragma GCC warning "TESTING ENABLED"
#endif

#include <algorithm>

extern "C" {

#ifdef TRACE
#include <stdio.h>
#include <stdlib.h>
#include <trace.h>
#endif

#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "usbd_cdc.h"
}
#include <cstdlib>

#include "aliases/memory.hpp"
#include "cpwm/cpwm.hpp"
#include "cpwm/rpm_calc.h"
#include "debug/debug_local.h"
#include "efi_config.hpp"
#include "engine/engine.hpp"
#include "engine_status.hpp"
#include "ignition/include/ignition.hpp"
#include "injection/injection.hpp"
#include "memory/include/config.hpp"
#include "pmic/pmic.hpp"
#include "sensors/sensors.hpp"
#include "usbd_cdc_if.h"
#include "webserial/commands.hpp"

#ifdef ENABLE_CAN_ISO_TP
#include "can/can_enviroment.h"
#include "can/can_wrapper.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SPI_HandleTypeDef hspi2;
bool MOTOR_ENABLE;
bool SINC;
int32_t _INY_T1 = 450;

uint8_t INJECTION_STRATEGY = INJECTION_MODE_SPI;
uint8_t IGNITION_STRATEGY = IGNITION_MODE_WASTED_SPARK;
uint32_t IGNITION_DWELL_TIME = DEFAULT_DWELL_TIME;
uint32_t last_cycle, last_mid_cycle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI2_Init(void);
void MX_NVIC_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef TESTING
uint32_t mocktick = 0;
uint32_t tickStep = 15000;    // 4k rpm // 50000 => 1200 // 80000 => 750
#define PIO_UNIT_TESTING
#endif
/* USER CODE END 0 */
#ifndef PIO_UNIT_TESTING
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */
  TIM_HandleTypeDef htim14;
  htim14.Instance = TIM14;

  set_default_engine_config();
  HAL_TIM_Base_DeInit(&htim1);
  HAL_TIM_Base_DeInit(&htim14);

  /* USER CODE END 1 */
  TIM2->ARR = 0;
  TIM2->CR1 &= ~TIM_CR1_UDIS;
  TIM2->EGR = TIM_EGR_UG;
  TIM2->CR1 |= TIM_CR1_UDIS;
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  // HAL_Init();

  /* USER CODE BEGIN Init */

  MOTOR_ENABLE = true;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CRC_Init();

  MX_CAN1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  // MX_TIM11_Init();
  trace_printf("Event: <CORE> Init on: %d ms\r\n", HAL_GetTick() - 0);

#ifdef ENABLE_US_TIM
  MX_TIM13_Init();
  HAL_TIM_Base_Start_IT(&htim13);
#endif

  MX_NVIC_Init();
  /*  HAL_InitTick(0); */
  /* USER CODE BEGIN 2 */

#ifdef ENABLE_PMIC
  PMIC::init();
  PMIC::enable();
  // PMIC::setup_spark();
#endif

  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MEMORY_CS_GPIO_Port, MEMORY_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

#ifdef ENABLE_WEBSERIAL
  MX_USB_DEVICE_Init();
#endif
  HAL_TIM_Base_Start_IT(&htim10);

#ifdef ENABLE_DEBUG_SETUP
  on_setup();
#endif

#ifdef ENABLE_MEMORY
  // SPI Memory:
  W25qxx_Init();
#endif

  /*   W25qxx_EraseChip(); */
  efi_config = efi_cfg::get();
  // SRAND Init:
  srand(HAL_GetTick());

#ifdef ENABLE_WEBSERIAL
  web_serial::setup();
#endif
  uint32_t StartTime = HAL_GetTick();

  /*   uint8_t arr0[] = {0xf, 0xda, 0xdd};
    uint32_t arr1[] = {0xf, 0xda, 0xdd};
    trace_printf("Event: <MEMORY_CRC> Calculated: %d ; %d", HAL_CRC_Calculate(&hcrc, arr1, 3), HAL_CRC_Calculate(&hcrc, (uint32_t *)arr0,
    3)); */
  // Event: <MEMORY_CRC> Calculated: 729167232 ; 29477989

#ifdef ENABLE_IGNITION
  ignition::setup();
#endif

#ifdef ENABLE_INJECTION
  injection::setup();
#endif
  /*   // Tabla VE
    tables::read_all({17, 17, 0x3});
    // Tabla correccion por bateria:
    tables::read_all({17, 17, 0x4});
    // WUE
    tables::read_all({17, 2, 0x5});
    // ASE %
    tables::read_all({17, 2, 0x6});
    // ASE Taper
    tables::read_all({17, 2, 0x7});
    // IDLE /Stepper config
    tables::read_all({17, 2, 0x7}); */

  // Core inits:
  trace_printf("Event: <CORE> Init on: %d ms\r\n", HAL_GetTick() - StartTime);
  // Enable CKP/CMP interrupts:
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

#ifdef ENABLE_ENGINE_FRONTEND
  Engine::onEFISetup();
#endif
  /* Infinite loop */

  /* USER CODE BEGIN WHILE */
  while (1) {
#ifdef ENABLE_DEBUG_LOOP
    on_loop();
#endif

#ifdef ENABLE_WEBSERIAL
    web_serial::loop();
    web_serial::command_handler();
#endif

#ifdef ENABLE_IGNITION
    ignition::interrupt();
#endif

#ifdef ENABLE_INJECTION
    injection::on_loop();
#endif

#ifdef ENABLE_ENGINE_FRONTEND
    Engine::onEFILoop();
#endif

#ifdef ENABLE_SENSORS
    sensors::loop();
#endif

#ifdef ENABLE_CAN_ISO_TP

#endif

    // mid priority code, runs every 50mS
    if (HAL_GetTick() - last_mid_cycle >= 50) {
      last_mid_cycle = HAL_GetTick();
#ifdef ENABLE_WEBSERIAL
      web_serial::command_handler();
#endif
    }

    // low priority code, runs every 150mS
    if (HAL_GetTick() - last_cycle >= 150) {
      last_cycle = HAL_GetTick();
#ifdef ENABLE_WEBSERIAL
      web_serial::send_deque();
#endif
#ifdef ENABLE_SENSORS
      sensors::loop_low_priority();
#endif
    }

    _RPM = RPM::_RPM;
    efi_status.cycleStatus = RPM::status;
    RPM::watch_dog();
  }
  /* USER CODE END WHILE */
  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

#endif

/**
 * @brief NVIC Configuration.
 * @retval None
 */
void MX_NVIC_Init(void) {
  /* EXTI9_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  /* HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); */

  /* TIM1_UP_TIM10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

  /* TIM1_TRG_COM_TIM11_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);

  /* TIM4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);

  /* CAN1 interrupt Init */
  HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

  /* OTG_FS_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(OTG_FS_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

// https://riunet.upv.es/bitstream/handle/10251/39538/ArticuloTimers.pdf?sequence=1
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
  trace_printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
