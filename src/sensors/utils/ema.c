#include <stdint.h>
#include "ema.h"

struct EMAHighPass ema_high_pass_filter(struct EMAHighPass in){
   in.lastLP = EMA_ALPHA * in.actualValue + (1 - EMA_ALPHA) * in.lastValue;
   in.lastValue = in.actualValue - in.lastLP;
   return in;
}

struct EMALowPass EMALowPassFilter(struct EMALowPass in){
   in.lastValue = EMA_LOW_ALPHA * in.actualValue + (1 - EMA_LOW_ALPHA) * in.lastValue;
   return in;
}