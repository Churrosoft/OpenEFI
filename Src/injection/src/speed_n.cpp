#include "../injection.hpp"
#include "config.hpp"
#include "engine_status.hpp"

using namespace injection;

#ifndef speed_n_on_file
#pragma GCC warning "forbiden usage of namespace speedN"
#endif

#ifndef alphaN_compensate
#define STD_AIR_PRES ((kPascal_t)101.325f)
#define STD_AIR_TEMP ((kelvin_t)293.15)
#else
#pragma GCC warning "SpeedDensity is recommended over this setup"
#define STD_AIR_PRES ((kPascal_t)(sensors::values._MAP / 100))
#define STD_AIR_TEMP ((kelvin_t)((sensors::values.IAT / 100) + 273.15))    // TODO: cast C° to K° with define/func
#endif

mix_mass_t speedN::calculate_correction_time() {
  // TODO: pala time
  return (mix_mass_t)0;
}

fuel_mass_t speedN::calculate_injection_fuel() {
  if (_RPM == 0) {
    return (fuel_mass_t)0;
  }
  auto currentAirMass = speedN::get_airmass();
  int8_t engine_cilinders = CIL;

  auto lambda = efi_config.Injection.targetLambda;    // algunas veces me cago solo anidando tanto
  auto stoich = efi_config.Injection.targetStoich;

  auto afr = stoich * lambda;
  fuel_mass_t baseFuel = (fuel_mass_t)(currentAirMass.get() / afr);

  // status antes de salir:
  efi_status.injection.airFlow = (air_mass_t)((currentAirMass.get() * engine_cilinders / getEngineCycleDuration(_RPM)) * 3600000 / 1000);
  efi_status.injection.baseAir = currentAirMass;
  efi_status.injection.baseFuel = baseFuel;

  return baseFuel;
}

// from: https://www.engineeringtoolbox.com/molecular-mass-air-d_679.html
#define AIR_R 8.31446261815324f / 28.9647f

air_mass_t speedN::get_airmass() {
  // tendria que ser ley de gases, volumen => cilindrada
  // VE fijo como el piñon fijo
  int32_t VE = 8000;
  // esto tendria que venir de la config
  int32_t engine_displacement = 1596;
  int8_t engine_cilinders = CIL;

  air_mass_t full_cycle = (air_mass_t)(VE * (engine_displacement * STD_AIR_PRES.get() / (0.28705f * STD_AIR_TEMP.get())));
  return full_cycle / (air_mass_t)engine_cilinders;
}
