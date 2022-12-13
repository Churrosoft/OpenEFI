#include "../injection.hpp"

#include "config.hpp"
#include "engine_status.hpp"

void injection::setup() {
  // pre-carga de tablas/valores sueltos dependiendo el alg
}

void injection::on_loop() {
  // TODO: garrar la pala

  using namespace injection;
  float time = 0;

  // depende del algoritmo a usar:
  time += Injectors::fuel_mass_to_time(speedN::calculate_injection_fuel());
  time += speedN::calculate_correction_time().get();    // Lambda y temp de aire
  // generico:
  time += Injectors::get_base_time();    // aka lo que tarda el inyector en despegarse
  time *= Injectors::get_battery_correction();
  time *= Injectors::get_pressure_correction();
  time += Injectors::get_wall_wetting_correction();
  time -= Injectors::get_off_time();
  _INY_T1 = time * 1000;    // ms => us
}

int32_t injection::Injectors::get_base_time() {
  return 50;    // 0.5ms
}

int32_t injection::Injectors::fuel_mass_to_time(fuel_mass_t fuel) {
  //TODO: revisar modo de inyeccion y cilindros
  return fuel.get() / efi_status.injection.fuelFlowRate * 1000;
}

int32_t injection::Injectors::get_off_time() {
  return 25;    // 0.25ms
}

int32_t injection::Injectors::get_pressure_correction() {
  // por ahora solo disponible en OpenEFI no en uEFI por falta de canales
  // (o uEFI aprovechando las entradas de sensores de dashdash)
  return 1;
}

int32_t injection::Injectors::get_battery_correction() { return 1; }

int32_t injection::Injectors::get_wall_wetting_correction() { return 1; }

void injection::Injectors::set_injectorFlow() {
#pragma GCC warn "WIP; falta agregar control de flow por presion"
  // NOTE: como no se revisa la presion del combustible solo se llama una sola vez esto
  auto flow = efi_config.Injection.injector.fuelPressure *
              (efi_config.Injection.injector.flowCCMin * (efi_config.Injection.injector.fuelDensity / 60.f));

  efi_status.injection.fuelFlowRate = flow;
}
