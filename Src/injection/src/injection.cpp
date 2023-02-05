#include "../injection.hpp"

#include "aliases/memory.hpp"
#include "efi_config.hpp"
#include "engine_status.hpp"

void injection::setup() {
  // pre-carga de tablas/valores sueltos dependiendo el alg
  injection::Injectors::set_injectorFlow();

  // Tabla TPS / RPM => VE
  // TODO: la tabla podriiia ser de tamaño configurable
  AlphaN::tps_rpm_ve = tables::read_all({17, 17, 0x3});
}

void injection::on_loop() {
  // TODO: garrar la pala

  using namespace injection;
  float time = 0;

  // TODO: revisar RPM::status para que en SPIN_UP/CRANK tengan tiempos fijos desde la memoria flash

  // injection::Injectors::set_injectorFlow();
  // depende del algoritmo a usar:
  time += Injectors::fuel_mass_to_time(AlphaN::calculate_injection_fuel());
  time += AlphaN::calculate_correction_time() /* .get() */;    // Lambda y temp de aire
  // generico:
  time += Injectors::get_base_time();    // aka lo que tarda el inyector en despegarse
  time *= Injectors::get_pressure_correction();
  time += Injectors::get_wall_wetting_correction();
  time -= Injectors::get_off_time();
  _INY_T1 = time /*  * 1000 */;    // ms => us BUG: soy tarado, fuel_mass_to_time ya devuelve en uS

#ifdef EFI_DEBUG
  // mas que assert tendrian que ser DTC's o algo asi:
  assert_param(baseFuel * 2 < getEngineCycleDuration(_RPM) && _RPM > 0 || _RPM <= 0);
#endif
}
