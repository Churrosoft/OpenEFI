#define TESTING

#include "../Src/cpwm/rpm_calc.h"
#include "../Inc/defines.h"
#include "../Inc/variables.h"
#include "../Src/cpwm/cpwm.hpp"

#include <unity.h>

// void setUp(void) {
//   /* Is run before every test, put unit init calls here. */
//   for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();
// }

// void tearDown(void) {
//   /* Is run after every test, put unit clean-up calls here. */
// }

void test_crank_rpm() {
    mocktick = 80000;

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(750.0, RPM::_RPM, "Check RPM::interrupt");
}

void test_running_rpm() {
    mocktick = 15000;

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(4000.0, RPM::_RPM, "Check RPM::interrupt");
}

void test_idle_rpm() {
    mocktick = 50000;

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1200.0, RPM::_RPM, "Check RPM::interrupt");
}

int run_rpm_tests() {
  UnityBegin("Src/cpwm/src/rpm_calc.cpp:57");

  RUN_TEST(test_crank_rpm);

  RUN_TEST(test_running_rpm);

  RUN_TEST(test_idle_rpm);

  return UnityEnd();
}