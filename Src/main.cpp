/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#define DEBUG
/* Includes ------------------------------------------------------------------*/
extern "C"
{
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"
#include "ll_spi.h"
#include "usbd_cdc_if.h"
}

#ifdef TRACE
#include <stdio.h>
#include <stdlib.h>
#include <trace.h>
#endif

#ifdef TESTING
#pragma GCC warning "TESTING ENABLED"
#include <unity.h>
#include "../test/cpwm_test.cpp"
extern int run_tests(void);
#endif
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "aliases/memory.hpp"
#include "sensors/sensors.hpp"
#include "variables.h"
#include "pmic/pmic.hpp"
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
bool MOTOR_ENABLE;
bool SINC;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
/* USER CODE BEGIN Init */
#ifdef TESTING
  trace_initialize();
  printf("INIT_TESTING \n");
  run_tests();
  puts("END_TESTING \n");
#endif
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  // disable PMIC until setup
  PMIC::init();
  // Initialize rest of peripherals
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM9_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();

  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 2 */

#if Alpha == 1
  MOTOR_ENABLE = true;
#else
  MOTOR_ENABLE = can_turn_on();
#endif
  /* USER CODE END 2 */
  PMIC::enable();
  PMIC::setup_spark();
  HAL_Delay(100);

  /* Infinite loop */

  // habilitamos el pmic:
  /*   HAL_GPIO_WritePin(INY1_GPIO_Port, INY1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ECN1_GPIO_Port, ECN1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(PMIC_ENABLE_GPIO_Port, PMIC_ENABLE_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);


  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
  spi_send_byte(0b01001011);
  spi_send_byte(0b00111101);
  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);

  // a seguro se lo levaron preso asi que:
  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
  spi_read_byte();
  spi_read_byte();
  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);

  HAL_Delay(100);

  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
  PMIC::dtc_check();
  // 0x6A84 = 0110 1100 1000 0100
  //0110 (DAC cmd)
  //1100 (Max current = 19A )
  //100 (overlap setting: 50% )
  //00100 (Nominal current = 4A)

  spi_send_byte(0b01101100);
  spi_send_byte(0b10000100);
  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
  spi_read_byte();
  spi_read_byte();
  HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
 */

  while (1)
  {
    /* USER CODE END WHILE */
    // PMIC::demo_spark();

    uint8_t buffer[] = "Gueeeenaaass\n";
    CDC_Transmit_FS(buffer, sizeof(buffer));
    HAL_Delay(2000);
    
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  HAL_GPIO_WritePin(MIL_OUT_GPIO_Port, MIL_OUT_Pin, GPIO_PIN_SET);
  while (1)
  {
    HAL_GPIO_WritePin(CHK_OUT_GPIO_Port, CHK_OUT_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(CHK_OUT_GPIO_Port, CHK_OUT_Pin, GPIO_PIN_RESET);
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
