
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
  table_data mock{
      {0, 600, 750, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3400, 3700, 4200, 4400, 4700, 5000},
      {100, 85, 88, 90, 92, 94, 96, 98, 100, 99, 99, 99, 99, 98, 97, 96, 94},
      {95, 82, 85, 87, 89, 91, 93, 95, 96, 96, 96, 96, 96, 95, 94, 93, 91},
      {90, 79, 82, 84, 86, 88, 89, 91, 92, 92, 92, 92, 92, 91, 90, 89, 88},
      {85, 75, 78, 80, 82, 84, 86, 88, 89, 89, 89, 89, 89, 87, 87, 86, 84},
      {75, 68, 72, 74, 76, 77, 79, 81, 82, 82, 82, 82, 81, 80, 80, 79, 78},
      {70, 64, 68, 70, 72, 74, 76, 77, 78, 78, 78, 78, 78, 77, 76, 75, 74},
      {65, 59, 64, 67, 69, 71, 72, 74, 75, 75, 75, 75, 74, 74, 73, 72, 71},
      {60, 55, 61, 63, 66, 67, 69, 70, 71, 71, 71, 71, 71, 70, 70, 69, 67},
      {55, 50, 57, 60, 62, 64, 65, 67, 68, 68, 68, 68, 67, 67, 66, 65, 64},
      {50, 46, 52, 56, 59, 60, 62, 63, 64, 64, 64, 64, 64, 63, 63, 62, 61},
      {45, 42, 48, 52, 55, 57, 58, 60, 60, 60, 60, 60, 60, 60, 59, 58, 57},
      {40, 39, 44, 47, 51, 53, 55, 56, 57, 57, 57, 57, 57, 56, 56, 55, 54},
      {35, 37, 40, 43, 47, 49, 51, 52, 53, 53, 53, 53, 53, 53, 52, 51, 51},
      {30, 36, 37, 40, 43, 45, 47, 49, 50, 50, 50, 50, 50, 49, 49, 48, 47},
      {25, 35, 36, 37, 39, 41, 43, 45, 46, 46, 46, 46, 46, 45, 45, 45, 44},
      {15, 35, 35, 35, 35, 35, 36, 37, 38, 38, 38, 38, 38, 38, 38, 38, 37},
  };
  injection::AlphaN::tps_rpm_ve = mock;
  RUN_TEST(test_AlphaN_air_mass_idle);
  RUN_TEST(test_AlphaN_air_mass_full_load);
  RUN_TEST(test_AlphaN_air_mass_full_cruising);
  RUN_TEST(test_AlphaN_fuel_mass_idle);

  return UnityEnd();
}