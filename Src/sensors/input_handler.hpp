/* Esto se va a encargar de manejar el CD74HC4067 para obtener los diferentes
 * sensores */
#ifndef INPUT_HANDLER
#define INPUT_HANDLER
#include <stdint.h>

extern "C" {
#include "./utils/ema.h"
#include "main.h"
}
#include "defines.h"
#include "user_defines.h"

void SystemClock_Config(void);
// TODO: esto es un wrapper del ADC basicamente, refactorizar y dejar en
// namespace "ADC_Wrapper"

struct input_handler {
  struct EMALowPass values[16]; // valores pasados por EMA Low Pass
};
extern struct input_handler inputs;

/**
 * @brief devuelve input selecionada pasada por filtro EMA Low Pass
 * @param pin entrada analogica a leer
 */
int32_t get_input(uint8_t);

/** @return ADC raw data
 */
int32_t get_adc_data(uint8_t);

#endif