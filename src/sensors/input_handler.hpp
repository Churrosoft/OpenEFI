/* Esto se va a encargar de manejar el CD74HC4067 para obtener los diferentes sensores */
#ifndef INPUT_HANDLER
#define INPUT_HANDLER
#include <stdint.h>
// libopencm3:
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
// custom:
extern "C"
{
#include "./utils/ema.h"
}
#include "defines.h"

//TODO: esto es un wrapper del ADC basicamente, refactorizar y dejar en namespace "ADC_Wrapper"

struct input_handler
{
  struct EMALowPass values[16]; // valores pasados por EMA Low Pass
};
extern struct input_handler inputs;
/** devuelve input selecionada pasada por filtro EMA Low Pass
  * @param pin entrada analogica a leer
*/
uint16_t get_input(uint8_t);
/** @return ADC register data
*/
uint16_t get_adc_data(uint8_t);

/** @brief inicia el ADC y los trigers por tiempo
*/
void input_setup(void);
void adc_setup(); //static?
#endif