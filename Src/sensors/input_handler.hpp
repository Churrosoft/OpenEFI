/* Esto se va a encargar de manejar el CD74HC4067 para obtener los diferentes sensores */
#ifndef INPUT_HANDLER
#define INPUT_HANDLER
#include <stdint.h>

extern "C"
{
#include "main.h"
#include "./utils/ema.h"
}
#include "defines.h"

void SystemClock_Config(void);
/* static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void); */
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
// var interna:
#endif