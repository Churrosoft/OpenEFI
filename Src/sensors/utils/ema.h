#include <stdint.h>

/** @file */
#ifndef EMA
#define EMA

#define EMA_ALPHA 0.025  //< Valor de alpha, 0.025 a 0.035 recomendado para el filtro HP, variar dependiendo del PCB
#define EMA_LOW_ALPHA 0.45 //< Valor de alpha, 0.32 a 0.50 recomendado para el filtro LP, variar dependiendo del PCB

struct EMAHighPass{
    uint16_t lastValue;   //< ultimo valor luego del filtrado, se utiliza este para acceder a la medicion
    uint16_t actualValue; //< valor nuevo a filtrar
    uint16_t lastLP;      //< este es el ultimo valor del filtro LowPass, luego se resta al valor actual para dejar la frecuencia alta
};

struct EMALowPass{
    uint16_t lastValue;   //< ultimo valor luego del filtrado, se utiliza este para acceder a la medicion
    uint16_t actualValue; //< valor nuevo a filtrar
};

/** @brief  Exponential Moving Average, Low Pass Filter
 *  @param in Struct EmaLowPass con los valores necesarios para el calculo
 */
struct EMALowPass EMALowPassFilter(struct EMALowPass);

struct EMAHighPass ema_high_pass_filter(struct EMAHighPass);

#endif