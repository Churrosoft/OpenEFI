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

void test_stoped_rpm() {
  tickStep = 12000000;

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(5.0, RPM::_RPM, "Check RPM::interrupt");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
      (uint8_t)RPM_STATUS::STOPPED, RPM::status,
      "Check RPM::interrupt, status calculation (STOPPED)");
}

void test_spinup_rpm() {
  tickStep = 800000;
  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(75.0, RPM::_RPM, "Check RPM::interrupt");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
      (uint8_t)RPM_STATUS::SPIN_UP, RPM::status,
      "Check RPM::interrupt, status calculation (SPIN_UP)");
}

void test_crank_rpm() {
  tickStep = 100000;
  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(600.0, RPM::_RPM, "Check RPM::interrupt");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
      (uint8_t)RPM_STATUS::CRANK, RPM::status,
      "Check RPM::interrupt, status calculation (CRANK)");
}

void test_running_rpm() {
  tickStep = 15000;

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(4000.0, RPM::_RPM, "Check RPM::interrupt");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
      (uint8_t)RPM_STATUS::RUNNING, RPM::status,
      "Check RPM::interrupt, status calculation (RUNNING)");
}

void test_idle_rpm() {
  tickStep = 50000;

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  for (int8_t i = 0; i < LOGIC_DNT + 1; i++) RPM::interrupt();

  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1200.0, RPM::_RPM, "Check RPM::interrupt");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
      (uint8_t)RPM_STATUS::RUNNING, RPM::status,
      "Check RPM::interrupt, status calculation (RUNNING)");
}

int run_rpm_tests() {
  UnityBegin("Src/cpwm/src/rpm_calc.cpp:57");

  RUN_TEST(test_stoped_rpm);
  RUN_TEST(test_spinup_rpm);

  RUN_TEST(test_crank_rpm);

  RUN_TEST(test_running_rpm);

  RUN_TEST(test_idle_rpm);

  return UnityEnd();
}