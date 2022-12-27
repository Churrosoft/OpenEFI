#include <stdint.h>
#include <math.h>

#ifndef DEADBAND_H
#define DEADBAND_H

// ctrl c + v de rusefi pero con maxvalue y usando el abs del FPU:
// https://github.com/rusefi/rusefi/blob/master/firmware/util/math/deadband.h

template <int32_t TDeadband, int32_t maxValue = 0>
class Deadband {
 public:
  bool gt(float lhs, float rhs) {
    float absError = math::abs(lhs - rhs);
    if (absError > TDeadband) {
      m_lastState = lhs > rhs;
    }

    return m_lastState;
  }

  bool lt(float lhs, float rhs) { return gt(rhs, lhs); }
  bool lt(float lhs) { return gt(maxValue, lhs); }

  bool gt(float lhs) { return gt(lhs, maxValue); }

 private:
  bool m_lastState = false;
};

#endif