#include <stdint.h>

#ifndef CONFIG_HPP
#define CONFIG_HPP
// config del motor, la idea es ir pasando todo lo que esta clavadasimo en "defines.h" para aca

struct injectorConfig
{
    float flowCCMin;
    int8_t injectorCount;
    float fuelPressure;
    float fuelDensity;
};

struct injectionConfig {
  float targetLambda; // a cuanto tiene que apuntar la lambda (tendria que ser una tabla)
  float targetStoich; // proporcion A/F 14.7 nasta (o menos), 9.0 ethanol
  bool enable_alpha_n;
  bool enable_speed_density;
  injectorConfig injector;

};

struct engine_config {
  bool ready;    // true cuando ya se leyo desde la memoria
  injectionConfig Injection;
};

extern engine_config efi_config;

void set_default_engine_config();    // safe-defaults mientras se lee de la memoria
void get_config();                   // lee desde memoria
void write_config(engine_config);    // webserial => memoria

#endif
