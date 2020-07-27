#include "sensors.hpp"

sensors::SensorValues sensors::values = {0, 0, 0, 0};

void sensors::loop()
{
#if TPS_DUAL
#error Function not yet implemented.
#else
  sensors::values._TPS = TPS::get_value(get_input(7));
#endif
  sensors::values.IAT = IAT::get_value(get_input(5));
  sensors::values._MAP = MAP::get_value(get_input(4));
};

void sensors::loop_low_priority()
{
  values.TEMP = TEMP::get_value(get_input(6));
}

void sensors::setup()
{
  input_setup();
  adc_setup();
}