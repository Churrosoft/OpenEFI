#include "config.hpp"

engine_config efi_config;

void set_default_engine_config() {
  engine_config cfg;
  injectionConfig inj;


  // hasta que agregue esta tabla al tuner esto va escrito en piedra
  #pragma GCC warning "WIP: agregar tablas de lambda, todavia no es funcional"
  inj.targetLambda = 0.85;
  inj.targetStoich = 14.7;


  cfg.ready = false;
  cfg.Injection = inj;

  efi_config = cfg;
}