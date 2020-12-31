
#include "input_handler.hpp"
ADC_HandleTypeDef hadc1;

struct input_handler inputs;

uint16_t get_input(uint8_t pin)
{
    if (pin < 16)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
#ifdef Alpha
            inputs.values[pin].actualValue = get_adc_data(7);
#else
            inputs.values[pin].actualValue = get_adc_data(i);
#endif
            inputs.values[pin] = EMALowPassFilter(inputs.values[pin]);
        }
        return inputs.values[pin].lastValue;
    }
    return 0;
}

void input_setup()
{
#pragma GCC warning "Function decreapated"
}

void adc_setup()
{
    // Calibrate The ADC On Power-Up For Better Accuracy
    //HAL_ADCEx_Calibration_Start(&hadc1);
}

uint16_t get_adc_data(uint8_t channel)
{
#warning Channel selector not finished
    HAL_ADC_Start(&hadc1);
    // Poll ADC1 Perihperal & TimeOut = 1mSec
    HAL_ADC_PollForConversion(&hadc1, 1);
    return HAL_ADC_GetValue(&hadc1);
}
