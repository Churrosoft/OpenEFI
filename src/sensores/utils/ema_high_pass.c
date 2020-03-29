#ifndef EMAHIGHPASS
#define EMAHIGHPASS

#include <stdint.h>

#define EMA_ALPHA 0.025

struct EMAHighPass{
    uint16_t lastValue;
    uint16_t actualValue;
    uint16_t lastLP;
};

struct EMAHighPass ema_high_pass_filter(struct EMAHighPass in){
   in.lastLP = EMA_ALPHA * in.actualValue + (1 - EMA_ALPHA) * in.lastValue;
   in.lastValue = in.actualValue - in.lastLP;
   return in;
}

#endif