#include <unity.h>

#include "aliases/testing.hpp"
/* 
#include "../Src/ignition/include/ignition.hpp"
#include "../Src/sensors/sensors.hpp"
 */
extern "C" {
#include "trace.h"
#include <stdio.h>
#include <stdlib.h>
}


void setup_ignition_mocks() {
  table_data mock{
      {0, 42000, 94000, 120000, 140000, 170000, 200000, 230000, 260000, 290000, 320000, 350000, 380000, 410000, 440000, 470000, 750000},
      {2500, 1300, 1000, 1610, 1930, 2260, 2590, 2910, 3240, 3570, 3679, 3679, 3679, 3679, 3679, 3679, 3679},
      {3000, 1300, 1000, 1590, 1910, 2230, 2550, 2880, 3200, 3520, 3629, 3629, 3629, 3629, 3629, 3629, 3629},
      {3500, 1300, 1000, 1570, 1889, 2200, 2520, 2840, 3160, 3479, 3590, 3590, 3590, 3590, 3590, 3590, 3590},
      {4000, 1300, 1000, 1550, 1860, 2180, 2490, 2810, 3120, 3440, 3540, 3540, 3540, 3540, 3540, 3540, 3540},
      {4500, 1300, 1000, 1530, 1839, 2150, 2460, 2770, 3080, 3390, 3500, 3500, 3500, 3500, 3500, 3500, 3500},
      {5000, 1300, 1000, 1510, 1810, 2120, 2430, 2730, 3040, 3350, 3450, 3450, 3450, 3450, 3450, 3450, 3450},
      {5500, 1300, 1000, 1490, 1789, 2090, 2400, 2700, 3000, 3300, 3410, 3410, 3410, 3410, 3410, 3410, 3410},
      {6000, 1300, 1000, 1470, 1770, 2070, 2360, 2660, 2960, 3260, 3360, 3360, 3360, 3360, 3360, 3360, 3360},
      {6600, 1300, 1440, 1440, 1739, 2030, 2330, 2620, 2910, 3210, 3310, 3310, 3310, 3310, 3310, 3310, 3310},
      {7100, 1300, 1000, 1430, 1720, 2000, 2290, 2580, 2870, 3160, 3260, 3260, 3260, 3260, 3260, 3260, 3260},
      {7600, 1300, 1000, 1410, 1689, 1980, 2260, 2550, 2830, 3120, 3220, 3220, 3220, 3220, 3220, 3220, 3220},
      {8100, 1300, 1000, 1390, 1670, 1950, 2230, 2510, 2800, 3080, 3170, 3170, 3170, 3170, 3170, 3170, 3170},
      {8600, 1300, 1000, 1370, 1639, 1920, 2200, 2480, 2760, 3030, 3130, 3130, 3130, 3130, 3130, 3130, 3130},
      {9100, 500, 1000, 1350, 1620, 1889, 2170, 2440, 2720, 2990, 3080, 3080, 3080, 3080, 3080, 3080, 3080},
      {9600, 500, 1000, 1330, 1600, 1870, 2140, 2410, 2680, 2950, 3040, 3040, 3040, 3040, 3040, 3040, 3040},
      {10100, 500, 1000, 1310, 1570, 1839, 2100, 2370, 2640, 2900, 2990, 2990, 2990, 2990, 2990, 2990, 2990}};

  ignition::avc_tps_rpm.assign(mock.begin(), mock.end());
}

void test_idle_advance() {
  setup_ignition_mocks();

  _RPM = 94000;
  sensors::values._MAP = 3500;

  ignition::loaded = true;

  ignition::interrupt();

  TEST_ASSERT_EQUAL_INT32_MESSAGE(1000, _AE, "Check Ignition interrupt, wrong advance value [IDLE Advance]");
}

void test_idle_to_acc_advance() {
  setup_ignition_mocks();

  _RPM = 170000;
  sensors::values._MAP = 6600;

  ignition::loaded = true;

  ignition::interrupt();

  TEST_ASSERT_EQUAL_INT32_MESSAGE(2030, _AE, "Check Ignition interrupt, wrong advance value [IDLE to ACC Advance]");
}

void test_acc_advance() {
  setup_ignition_mocks();

  _RPM = 94000;
  sensors::values._MAP = 6000;

  ignition::loaded = true;

  ignition::interrupt();

  TEST_ASSERT_EQUAL_INT32_MESSAGE(1000, _AE, "Check Ignition interrupt, wrong advance value [ACC Advance]");
}

void test_acc_to_idle_advance() {
  setup_ignition_mocks();

  _RPM = 260000;
  sensors::values._MAP = 100;

  ignition::loaded = true;

  ignition::interrupt();

  TEST_ASSERT_EQUAL_INT32_MESSAGE(3240, _AE, "Check Ignition interrupt, wrong advance value [ACC to IDLE Advance]");
}

int runIgnitionTests() {
  UnityBegin("Src/ignition/src/ignition.cpp:25");
  RUN_TEST(test_idle_advance);
  debug_printf("----------------------- \n\r");
  RUN_TEST(test_idle_to_acc_advance);
  debug_printf("----------------------- \n\r");
  RUN_TEST(test_acc_advance);
  debug_printf("----------------------- \n\r");
  RUN_TEST(test_acc_to_idle_advance);

  return UnityEnd();
}