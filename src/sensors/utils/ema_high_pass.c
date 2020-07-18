/** @file */
#ifndef EMAHIGHPASS
#define EMAHIGHPASS

#include <stdint.h>

#define EMA_ALPHA 0.025  //< Valor de alpha, 0.025 a 0.035 recomendado para el filtro HP, variar dependiendo del PCB

struct EMAHighPass{
    uint16_t lastValue;   //< ultimo valor luego del filtrado, se utiliza este para acceder a la medicion
    uint16_t actualValue; //< valor nuevo a filtrar
    uint16_t lastLP;      //< este es el ultimo valor del filtro LowPass, luego se resta al valor actual para dejar la frecuencia alta
};

struct EMAHighPass ema_high_pass_filter(struct EMAHighPass in){
   in.lastLP = EMA_ALPHA * in.actualValue + (1 - EMA_ALPHA) * in.lastValue;
   in.lastValue = in.actualValue - in.lastLP;
   return in;
}

#endif