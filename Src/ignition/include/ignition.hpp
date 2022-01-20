/**

 */

#ifndef IGNITION_HPP
#define IGNITION_HPP

#include <limits.h>
#include <stdint.h>
#include <vector>

#include "aliases/memory.hpp"
#include "aliases/sensors.hpp"
#include "variables.h"

namespace ignition {
void interrupt();
void setup();

// tabla de avance, por TPS y RPM
extern TABLEDATA avc_tps_rpm;
extern int16_t avc_rpm[13];
extern int16_t avc_tps[13];
extern bool loaded;

} // namespace ignition

#endif