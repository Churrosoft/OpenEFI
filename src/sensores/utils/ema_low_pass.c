#ifndef EMALOWPASS
#define EMALOWPASS

#include <stdint.h>

#define EMA_ALPHA 0.45

struct EMALowPass{
    uint16_t lastValue;
    uint16_t actualValue;
};

struct EMALowPass EMALowPassFilter(struct EMALowPass in){
   in.lastValue = EMA_ALPHA * in.actualValue + (1 - EMA_ALPHA) * in.lastValue;
   return in;
}

#endif