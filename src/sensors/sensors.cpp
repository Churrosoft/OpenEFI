#include "./input_handler.cpp"
#include "./tps.cpp"
#include "defines.h"

#ifndef SENSORS_CPP
#define SENSORS_CPP

namespace sensors
{
  /** @brief Setup inicial de todos los sensores
    */
  static void setup(void);

  /** @brief Loop principal de los sensores, corre cada 75mS */
  static void loop(void);
  /** @brief Loop secundario, corre cada 250mS */
  static void loop_low_priority(void);

  //Struct con info de todos los sensores
  struct values
  {
    uint8_t TPS;
    uint16_t MAP;
  } values{0, 0};

} // namespace sensors

static void sensors::loop()
{
#if TPS_DUAL
#error Function not yet implemented.
#else
  values.TPS = TPS::get_value(get_input(7));
#endif
};

static void sensors::loop_low_priority()
{
}

static void sensors::setup()
{
  input_setup();
  adc_setup();
}

#endif