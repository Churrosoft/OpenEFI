/** @file */
#ifndef EMALOWPASS
#define EMALOWPASS

#include <stdint.h>

#define EMA_ALPHA 0.45 //< Valor de alpha, 0.32 a 0.50 recomendado para el filtro LP, variar dependiendo del PCB

struct EMALowPass{
    uint16_t lastValue;   //< ultimo valor luego del filtrado, se utiliza este para acceder a la medicion
    uint16_t actualValue; //< valor nuevo a filtrar
};

/** @brief  Exponential Moving Average, Low Pass Filter
 *  @param in Struct EmaLowPass con los valores necesarios para el calculo
 */
struct EMALowPass EMALowPassFilter(struct EMALowPass);

struct EMALowPass EMALowPassFilter(struct EMALowPass in){
   in.lastValue = EMA_ALPHA * in.actualValue + (1 - EMA_ALPHA) * in.lastValue;
   return in;
}

#endif