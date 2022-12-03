#include "../injection.hpp"

void injection::setup() {
  // pre-carga de tablas/valores sueltos dependiendo el alg
}

void injection::on_loop() {
  // TODO: garrar la pala
  _INY_T1 = injection::speedN::calculate_injection_time() + injection::speedN::calculate_correction_time();
}