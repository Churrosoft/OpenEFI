
#include "input_handler.hpp"
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

struct input_handler inputs;
uint32_t ADC_A_RAW_DATA[5];
uint32_t ADC_B_RAW_DATA[10];

uint16_t get_input(uint8_t pin)
{
    if (pin < 15)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            if (pin < 5)
                inputs.values[pin].actualValue = ADC_A_RAW_DATA[pin];
            else
                inputs.values[pin].actualValue = ADC_B_RAW_DATA[pin];
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
    if (HAL_ADC_Start_DMA(&hadc1, ADC_A_RAW_DATA, 5) != HAL_OK)
        Error_Handler();
    if (HAL_ADC_Start_DMA(&hadc2, ADC_B_RAW_DATA, 10) != HAL_OK)
        Error_Handler();
}

uint16_t get_adc_data(uint8_t channel)
{

#pragma GCC warning "Function decreapated"
}
