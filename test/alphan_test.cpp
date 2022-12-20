
#include <unity.h>

#include "../../Src/injection/injection.hpp"
extern "C" {
#include "stdio.h"
#include "unity_config.h"
}

void test_AlphaN_air_mass_idle() {
  int32_t VE = 40;
  float air_mass = injection::AlphaN::get_airmass(VE);

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(192.1739, air_mass, "Check AlphaN / get_airmass [IDLE]");
}

void test_AlphaN_air_mass_full_load() {
  int32_t VE = 93;
  float air_mass = injection::AlphaN::get_airmass(VE);

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(446.8044, air_mass, "Check AlphaN / get_airmass [FULL_LOAD]");
}

void test_AlphaN_air_mass_full_cruising() {
  int32_t VE = 65;
  float air_mass = injection::AlphaN::get_airmass(VE);

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(312.2826, air_mass, "Check AlphaN / get_airmass [CRUISING]");
}

int run_aplha_n_tests() {
  UnityBegin("Src/injection/injection.hpp:19");

  RUN_TEST(test_AlphaN_air_mass_idle);
  RUN_TEST(test_AlphaN_air_mass_full_load);
  RUN_TEST(test_AlphaN_air_mass_full_cruising);

  return UnityEnd();
}