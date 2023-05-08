/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

#include "stm32f4xx_ll_exti.h"
#include "crc.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/


#define ADC_CS_Pin GPIO_PIN_0
#define ADC_CS_GPIO_Port GPIOC

#define ENABLE_5V_Pin GPIO_PIN_1
#define ENABLE_5V_GPIO_Port GPIOC


#define RPM_OUT_Pin GPIO_PIN_11
#define RPM_OUT_GPIO_Port GPIOE

#define MEMORY_CS_Pin GPIO_PIN_13
#define MEMORY_CS_GPIO_Port GPIOE


#define GNC_GLP_IN_Pin GPIO_PIN_11
#define GNC_GLP_IN_GPIO_Port GPIOB
 

#define MIL_OUT_Pin GPIO_PIN_8
#define MIL_OUT_GPIO_Port GPIOC
#define CHK_OUT_Pin GPIO_PIN_9
#define CHK_OUT_GPIO_Port GPIOC
#define GNC_GLP_OUT_Pin GPIO_PIN_8
#define GNC_GLP_OUT_GPIO_Port GPIOA



#define ACC_DET_Pin GPIO_PIN_12
#define ACC_DET_GPIO_Port GPIOC
#define LED_CAN_RX_Pin GPIO_PIN_6
#define LED_CAN_RX_GPIO_Port GPIOD
#define LED_CAN_TX_Pin GPIO_PIN_7
#define LED_CAN_TX_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
