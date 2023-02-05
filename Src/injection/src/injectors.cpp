#include "../injection.hpp"

#include "aliases/memory.hpp"
#include "efi_config.hpp"
#include "engine_status.hpp"

int32_t injection::Injectors::fuel_mass_to_time(fuel_mass_t fuel) {
  // TODO: revisar modo de inyeccion y cilindros
  // monopunto al hacer 4 inyecciones por ciclo seria lo mismo que full secuencial
  // perooo en semi-secuencial al haber dos inyectores, y la mitad de injecciones por ciclo
  // tendria que ser 0.5
  return (fuel /* .get() */ / efi_status.injection.fuelFlowRate * 1000) / 2;
}

// TODO: estos dos tendrian que revisar una tabla nueva de delay vs voltaje
float injection::Injectors::get_base_time() { return efi_config.Injection.injector.onTime; }

float injection::Injectors::get_off_time() { return efi_config.Injection.injector.offTime; }

int32_t injection::Injectors::get_pressure_correction() {
  // por ahora solo disponible en OpenEFI no en uEFI por falta de canales
  // (o uEFI aprovechando las entradas de sensores de dashdash)
  // FIXME: en teoria con llamar a set_injectorFlow ya estaria
  return 1;
}

int32_t injection::Injectors::get_wall_wetting_correction() { return 1; }

void injection::Injectors::set_injectorFlow() {
#pragma GCC warn "WIP; falta agregar control de flow por presion"
  // NOTE: como no se revisa la presion del combustible solo se llama una sola vez esto
  auto flow = efi_config.Injection.injector.fuelPressure *
              (efi_config.Injection.injector.flowCCMin * (efi_config.Injection.injector.fuelDensity / 60.0f));

  efi_status.injection.fuelFlowRate = flow;
}