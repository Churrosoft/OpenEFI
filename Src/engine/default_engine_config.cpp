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

  cfg.ready = false;
  cfg.Injection = inj;

  efi_config = cfg;
}