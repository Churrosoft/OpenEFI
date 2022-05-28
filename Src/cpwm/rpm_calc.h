#ifndef RPM_H
#define RPM_H

#include "stdint.h"
#include <cmath>

#include "defines.h"
#include "variables.h"

#include "../sensors/utils/sinc.h"

extern "C" {
#include "trace.h"
}

namespace RPM {

void interrupt(void);

extern uint16_t WheelTooth;
extern uint32_t lastWheelTime;
extern uint32_t actualWheelTime;
extern bool tooth_status;

extern float _DEG;
extern float _RPM;

enum RPM_STATUS {
  CRANK, // < 400 rpm
  RUNNING,
  STOPED, // < 100 rpm
};

} // namespace RPM

#endif