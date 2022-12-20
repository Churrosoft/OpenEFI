#define UNITY_INCLUDE_PRINT_FORMATTED

#include "../Inc/main.h"
#include "unity_config.h"
#include <unity.h>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <trace.h>
}

int main() {
  HAL_Init();

  UNITY_BEGIN();
  trace_initialize();
  debug_printf("INIT_TESTING \n\r");
  run_tests();
  run_rpm_tests();
  runIgnitionTests();
  run_aplha_n_tests();
  debug_printf("END_TESTING \n\r");

/*   // Que Dios y la Patria me lo demanden
  SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
  abort(); */
  UNITY_END();    // stop unit testing

  while (1) {
  }
}

/* void SysTick_Handler(void) { HAL_IncTick(); } */