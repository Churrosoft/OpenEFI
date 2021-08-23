/**

 */
#include <stdint.h>

#ifndef IGNITION_HPP
#define IGNITION_HPP

#include "aliases/memory.hpp"
#include <limits.h>
#include <vector>

namespace ignition
{
   void interrupt();
   void setup();

   // tabla de avance, por TPS y RPM
   TABLEDATA avc_tps_rpm;
} // namespace ignition

#endif