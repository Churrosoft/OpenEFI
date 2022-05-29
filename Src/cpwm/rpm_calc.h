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

namespace rpm_status {
enum __rpm_status {
  STOPPED = 0, // < 50
  SPIN_UP,     // > 50
  CRANK,       // > 400 rpm
  RUNNING,     // > 750
};
}
typedef rpm_status::__rpm_status RPM_STATUS;

namespace RPM {

void interrupt(void);

extern uint16_t WheelTooth;
extern uint32_t lastWheelTime;
extern uint32_t actualWheelTime;
extern bool tooth_status;

extern float _DEG;
extern float _RPM;

extern RPM_STATUS status;

} // namespace RPM

#endif