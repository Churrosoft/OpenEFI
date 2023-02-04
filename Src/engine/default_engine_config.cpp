#include "defines.h"
#include "efi_config.hpp"
#include "memory/include/config.hpp"

engine_config efi_config;

void set_default_engine_config() {
#pragma GCC warning "decrepado, borrar implementaciones de esto"
}

void efi_cfg::set_default() {
  engine_config cfg;
  injectionConfig inj;
  injectorConfig injector;

// FIXME: hasta que agregue esta tabla al tuner esto va escrito en piedra
#pragma GCC warning "WIP: agregar tablas de lambda, todavia no es funcional"
  inj.targetLambda = 0.98;
  inj.targetStoich = 14.7;
  inj.enable_alphaN = true;

  injector.flowCCMin = 110;
  injector.fuelDensity = 0.726;
  // FIXME: en teoria tendria esto tendria que ser la presion, no el multiplicador sobre la presion base
  injector.fuelPressure = 1;
  // promedio a "ojimetro" de:
  // https://documents.holley.com/techlibrary_terminatorxv2injectordata.pdf
  injector.offTime = 250;
  injector.onTime = 750;

  inj.injector = injector;
  inj.alphaN_ve_table = TABLES_INJECTION_VE_SETTINGS;

  cfg.ready = false;
  cfg.Injection = inj;

  efi_config = cfg;
}
/* 
void to_json(json& j, const engine_config& p) {
  j = json{
      {"ready", p.ready},
      {"Injection",
       {{"targetLambda", p.Injection.targetLambda},
        {"targetStoich", p.Injection.targetStoich},
        {"enable_alphaN", p.Injection.enable_alphaN},
        {"enable_speedDensity", p.Injection.enable_speedDensity},

        {"injector",
         {
             {"flowCCMin", p.Injection.injector.flowCCMin},
             {"injectorCount", p.Injection.injector.injectorCount},
             {"fuelPressure", p.Injection.injector.fuelPressure},
             {"fuelDensity", p.Injection.injector.fuelDensity},
             {"onTime", p.Injection.injector.onTime},
             {"offTime", p.Injection.injector.offTime},
         }},

        {"alphaN_ve_table",
         {{"x_max", p.Injection.alphaN_ve_table.x_max},
          {"y_max", p.Injection.alphaN_ve_table.y_max},
          {"memory_address", p.Injection.alphaN_ve_table.memory_address}

         }}}},
  };
} */