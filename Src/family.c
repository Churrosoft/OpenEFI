/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "stm32f4xx_hal.h"
#include "main.h"
#include "../lib/tinyusb/hw/bsp/board.h"
#include "../lib/tinyusb/hw/bsp/board_mcu.h"

//--------------------------------------------------------------------+
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------+
/* void OTG_FS_IRQHandler(void)
{
  tud_int_handler(0);
}
 */
//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM
//--------------------------------------------------------------------+
UART_HandleTypeDef UartHandle;

void board_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**USB_OTG_FS GPIO Configuration
    PA9     ------> USB_OTG_FS_VBUS
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP
    */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral clock enable */
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

  /* Peripheral interrupt init */
  HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
  /* USER CODE BEGIN USB_OTG_FS_MspInit 1 */

  /* USER CODE END USB_OTG_FS_MspInit 1 */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

  // board_vbus_sense_init();
}

//--------------------------------------------------------------------+
// Board porting API
//--------------------------------------------------------------------+

void board_led_write(bool state)
{
  /*  HAL_GPIO_WritePin(LED_PORT, LED_PIN, state ? LED_STATE_ON : (1-LED_STATE_ON));*/
}

uint32_t board_button_read(void)
{
  return 0;
}

int board_uart_read(uint8_t *buf, int len)
{
  (void)buf;
  (void)len;
  return 0;
}

int board_uart_write(void const *buf, int len)
{
#ifdef UART_DEV
  HAL_UART_Transmit(&UartHandle, (uint8_t *)(uintptr_t)buf, len, 0xffff);
  return len;
#else
  (void)buf;
  (void)len;
  (void)UartHandle;
  return 0;
#endif
}

uint32_t board_millis(void)
{
  return HAL_GetTick();
}
