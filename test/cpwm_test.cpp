#define UNITY_INCLUDE_PRINT_FORMATTED

extern "C" {
#include "stdio.h"
#include "unity_config.h"
}
#include <unity.h>

#include "../Src/cpwm/cpwm.hpp"

void setUp(void) {}    /* Is run before every test, put unit init calls here. */
void tearDown(void) {} /* Is run after every test, put unit clean-up calls here. */

// vamo' a testear el cilindro '1'
// FIXME: en realidad son 48/55 revisar como redondea los numeros en la FPU
// para eso, vamos a mover el tick de CPWM a 49, o 144.05°
// tendria que hacer ignicion en el mismo, luego se cambia a 56/171.05° para probar la ignicion
void test_first_cilinder_egn(void) {
  CPWM::ckp_tick = 56;
  CPWM::interrupt();
}

void test_first_cilinder_iny(void) {
  CPWM::ckp_tick = 49;
  CPWM::interrupt();
}

void test_second_cilinder_egn(void) {
  CPWM::ckp_tick = 99;
  CPWM::interrupt();
}

void test_second_cilinder_iny(void) {
  CPWM::ckp_tick = 115;
  CPWM::interrupt();
}
void test_third_cilinder_egn(void) {
  CPWM::ckp_tick = 149;
  CPWM::interrupt();
}

void test_third_cilinder_iny(void) {
  CPWM::ckp_tick = 173;
  CPWM::interrupt();
}
void test_fourth_cilinder_egn(void) {
  CPWM::ckp_tick = 199;
  CPWM::interrupt();
}

void test_fourth_cilinder_iny(void) {
  CPWM::ckp_tick = 232;
  CPWM::interrupt();
}

int run_tests() {
  UnityBegin("Src/cpwm/src/cpwm.cpp:57");

  RUN_TEST(test_first_cilinder_egn);
  RUN_TEST(test_first_cilinder_iny);

  RUN_TEST(test_second_cilinder_egn);
  RUN_TEST(test_second_cilinder_iny);

  RUN_TEST(test_third_cilinder_egn);
  RUN_TEST(test_third_cilinder_iny);

  RUN_TEST(test_fourth_cilinder_egn);
  RUN_TEST(test_fourth_cilinder_iny);

  return UnityEnd();
}