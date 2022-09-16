#include "sensors.hpp"

using namespace sensors;

SensorValues sensors::values = {0, 0, 0, 0};

/*
Default mapping:
TEMP => 0
TPS => 1
IAT => 2
MAP => 3

APS (Accelerator Pedal Sensor) || OTS => 5
extLAMB (o lambda 1 cable) => 6

Bateria => 7

TODO:
FRS (fuel rail sensor[pressure])
*/

void sensors::loop() {
#if TPS_DUAL
#error Function not yet implemented.
#else
  sensors::values._TPS = TPS::get_value(get_input(1));
#endif
  sensors::values._MAP = MAP::get_value(get_input(3));
};

void sensors::loop_low_priority() {
  sensors::values.TEMP = TEMP::get_value(get_input(0));
  sensors::values.IAT = IAT::get_value(get_input(2));
  sensors::values.BATT = BATT::get_value(get_input(7));
}