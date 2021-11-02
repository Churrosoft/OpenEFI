#ifndef WEBUSB_INCLUDE
#define WEBUSB_INCLUDE
#include <stdint.h>
#include <stdbool.h>

#define CFG_TUSB_MCU 304
#define FAMILY stm32f4

#include "../lib/tinyusb/hw/bsp/board.h"
#include "../lib/tinyusb/hw/bsp/board_mcu.h"
#include "usb_descriptors.h"
#include "tusb.h"


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED     = 1000,
  BLINK_SUSPENDED   = 2500,

  BLINK_ALWAYS_ON   = UINT32_MAX,
  BLINK_ALWAYS_OFF  = 0
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

#define URL  "example.tinyusb.org/webusb-serial/"

static bool web_serial_connected = false;

void cdc_task(void);
void webserial_task(void);

#endif