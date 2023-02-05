#include <math.h>

#include "../injection.hpp"
#include "efi_config.hpp"
#include "engine_status.hpp"

extern "C" {
#include "trace.h"
}

using namespace injection;
table_data AlphaN::tps_rpm_ve;

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

mix_mass_t AlphaN::calculate_correction_time() {
  // TODO: pala time
  return (mix_mass_t)0;
}

fuel_mass_t AlphaN::calculate_injection_fuel() {
  if (_RPM == 0) {
    return (fuel_mass_t)0;
  }
  // VE fijo como el piñon fijo
  int32_t VE = injection::AlphaN::get_ve();
  auto currentAirMass = AlphaN::get_airmass(VE);
  int8_t engine_cilinders = CIL;

  auto volatile lambda = efi_config.Injection.targetLambda;    // algunas veces me cago solo anidando tanto
  auto volatile stoich = efi_config.Injection.targetStoich;

  auto volatile afr = stoich * lambda;

  fuel_mass_t baseFuel = (fuel_mass_t)(currentAirMass /* .get() */ / afr);

#ifdef EFI_DEBUG
  assert_param(std::isfinite(VE));
  assert_param(std::isfinite(lambda));
  assert_param(std::isfinite(stoich));
  assert_param(std::isfinite(afr));
  assert_param(std::isfinite(baseFuel));
#endif

  // status antes de salir:
  efi_status.injection.airFlow =
      (air_mass_t)((currentAirMass /* .get() */ * engine_cilinders / getEngineCycleDuration(_RPM)) * 3600000 / 1000);
  efi_status.injection.baseAir = currentAirMass;
  efi_status.injection.baseFuel = baseFuel;
  efi_status.injection.injectionBank1Time = (float)injection::Injectors::fuel_mass_to_time(baseFuel) / 1000;
  efi_status.cycleDuration = getEngineCycleDuration(_RPM);
  efi_status.injection.targetAFR = afr;

  return baseFuel;
}

// from: https://www.engineeringtoolbox.com/molecular-mass-air-d_679.html
#define AIR_R (8.31446261815324f / 28.9647f)

air_mass_t AlphaN::get_airmass(int32_t VE) {
  // tendria que ser ley de gases, volumen => cilindrada
  // VE llega como numero del 0 al 100 (o ma' el turbo ameo), por eso al division por 100
  // esto tendria que venir de la config
  int32_t engine_displacement = 1596;
  int8_t engine_cilinders = CIL;

  air_mass_t full_cycle = (air_mass_t)((float)VE / 10000) * (engine_displacement * STD_AIR_PRES / (AIR_R * STD_AIR_TEMP));
  return (air_mass_t)(full_cycle / engine_cilinders);
}

int32_t injection::AlphaN::get_ve() {
#ifdef TESTING
  return 10;
#endif
  auto load_row = tables::col_to_row(injection::AlphaN::tps_rpm_ve, 0);

  // En este caso la carga la determina el TPS
  auto load_value = tables::find_nearest_neighbor(load_row, sensors::values._TPS);
  auto rpm_value = tables::find_nearest_neighbor(injection::AlphaN::tps_rpm_ve.at(0), _RPM * 100);

  if (tables::on_bounds(/* efi_config.Injection.alphaN_ve_table */ {17, 17, 0x3}, load_value, rpm_value)) {
    return injection::AlphaN::tps_rpm_ve.at(load_value).at(rpm_value);
  }
  return 0;
}