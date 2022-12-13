// struct con la info del motor en estado real, pa' poder pasar mas facil del codigo de iny hacia el webserial
#ifndef ENGINE_STATUS_HPP
#define ENGINE_STATUS_HPP

#include "custom_vars.hpp"

enum injectionStatus {
  FUEL_CUTOFF = 0,
  FULL_LOAD,
  FUEL_ACC,
};

struct injection_info {
  float targetAFR;
  float injectionBank1Time;
  float injectionBank2Time;
  air_mass_t airFlow;    // in kg/h, same as rusefi estimations
  air_mass_t baseAir;
  fuel_mass_t baseFuel;
  float fuelFlowRate;
  float fuelLoad;
  injectionStatus status;
};

struct engine_status {
  injection_info injection;
};

extern engine_status efi_status;

#endif