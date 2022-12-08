#include "../injection.hpp"

void injection::setup() {
  // pre-carga de tablas/valores sueltos dependiendo el alg
}

void injection::on_loop() {
  // TODO: garrar la pala

  using namespace injection;
  int32_t time = 0;

  // depende del algoritmo a usar:
  time += speedN::calculate_injection_time();
  time += speedN::calculate_correction_time();    // Lambda y temp de aire
  // generico:
  time += Injectors::get_base_time(); // aka lo que tarda el inyector en despegarse
  time *= Injectors::get_battery_correction();
  time *= Injectors::get_pressure_correction();
  time += Injectors::get_wall_wetting_correction();
  time -= Injectors::get_off_time();
  _INY_T1 = time;
}

int32_t injection::Injectors::get_base_time() {
  return 50;    // 0.5ms
}

int32_t injection::Injectors::get_off_time() {
  return 25;    // 0.25ms
}

int32_t injection::Injectors::get_pressure_correction() {
  // por ahora solo disponible en OpenEFI no en uEFI por falta de canales
  // (o uEFI aprovechando las entradas de sensores de dashdash)
  return 1;
}

int32_t injection::Injectors::get_battery_correction() {
  return 1;
}

int32_t injection::Injectors::get_wall_wetting_correction() {
  return 1;
}
