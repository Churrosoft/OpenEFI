
#include <unity.h>

#include "../../Src/injection/injection.hpp"
extern "C" {
#include "stdio.h"
#include "unity_config.h"
}

void test_AlphaN_air_mass_idle() {
  int32_t VE = 40;
  float air_mass = injection::AlphaN::get_airmass(VE);

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(192.1739, air_mass, "Check AlphaN / get_airmass VE = 40 [IDLE]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(72.06522, injection::AlphaN::get_airmass(15), "Check AlphaN / get_airmass VE = 15 [IDLE]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(187.3696, injection::AlphaN::get_airmass(39), "Check AlphaN / get_airmass VE = 39 [IDLE]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(120.1087, injection::AlphaN::get_airmass(25), "Check AlphaN / get_airmass VE = 25 [IDLE]");
}

void test_AlphaN_air_mass_full_load() {
  int32_t VE = 93;
  float air_mass = injection::AlphaN::get_airmass(VE);

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(446.8044, air_mass, "Check AlphaN / get_airmass VE = 93 [FULL_LOAD]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(432.3913, injection::AlphaN::get_airmass(90), "Check AlphaN / get_airmass VE = 90 [FULL_LOAD]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(408.3696, injection::AlphaN::get_airmass(85), "Check AlphaN / get_airmass VE = 85 [FULL_LOAD]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(360.3261, injection::AlphaN::get_airmass(75), "Check AlphaN / get_airmass VE = 75 [FULL_LOAD]");
}

void test_AlphaN_air_mass_full_cruising() {
  int32_t VE = 65;
  float air_mass = injection::AlphaN::get_airmass(VE);

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(312.2826, air_mass, "Check AlphaN / get_airmass VE = 60 [CRUISING]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(288.2609, injection::AlphaN::get_airmass(60), "Check AlphaN / get_airmass VE = 60 [FULL_LOAD]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(264.2391, injection::AlphaN::get_airmass(55), "Check AlphaN / get_airmass VE = 55 [FULL_LOAD]");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(297.8696, injection::AlphaN::get_airmass(62), "Check AlphaN / get_airmass VE = 62 [FULL_LOAD]");
}

void test_AlphaN_fuel_mass_idle() {
  float fuel_mass = injection::AlphaN::calculate_injection_fuel();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.334963, fuel_mass, "Check AlphaN / calculate_injection_fuel VE = 80 [IDLE]");
}



int run_aplha_n_tests() {
  UnityBegin("Src/injection/injection.hpp:19");

  RUN_TEST(test_AlphaN_air_mass_idle);
  RUN_TEST(test_AlphaN_air_mass_full_load);
  RUN_TEST(test_AlphaN_air_mass_full_cruising);
  RUN_TEST(test_AlphaN_fuel_mass_idle);

  return UnityEnd();
}