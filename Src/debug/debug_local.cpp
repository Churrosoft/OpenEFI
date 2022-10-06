#include "./debug_local.h"
#include "main.h"
uint64_t last_rpm = 0;

void on_setup() {}

void on_loop() {
  if (HAL_GetTick() - last_rpm >= 2500) {

    last_rpm = HAL_GetTick();

/*     EFI_INVERT_PIN(LED1_GPIO_Port, LED1_Pin);
 */

 /* if(usb.dev_state == USBD_STATE_CONFIGURED){

 } */

    /* EFI_LOG("Event: <RPM> %f <RPM Status> %d \r\n", RPM::_RPM,
    RPM::status);

    web_serial::send_debug_message(
      web_serial::debugMessage::LOG,
        "Event: <RPM> %f <RPM Status> %d \r\n", RPM::_RPM, RPM::status
    ); */
  }
}

void on_gpio_init() {}