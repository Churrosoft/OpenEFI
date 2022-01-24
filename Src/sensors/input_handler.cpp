
#include "input_handler.hpp"
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

struct input_handler inputs;
uint32_t ADC_A_RAW_DATA[5] = {0, 0, 0, 0, 0};
uint32_t ADC_B_RAW_DATA[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint16_t get_input(uint8_t pin) {
  inputs.values[pin].actualValue = ADC_A_RAW_DATA[0];
  inputs.values[pin] = EMALowPassFilter(inputs.values[pin]);
  return inputs.values[pin].actualValue;

  if (pin < 15) {
    for (uint8_t i = 0; i < 5; i++) {
      if (pin < 5) {
        inputs.values[pin].actualValue = ADC_A_RAW_DATA[pin];
        volatile auto debug = ADC_A_RAW_DATA[pin];
        debug = debug;
      } else {
        inputs.values[pin].actualValue = ADC_B_RAW_DATA[pin];
        volatile auto debug2 = ADC_A_RAW_DATA[pin];
        debug2 = debug2;
      }
      inputs.values[pin] = EMALowPassFilter(inputs.values[pin]);
    }
    return inputs.values[pin].lastValue;
  }
  return 0;
}

void input_setup() {
#pragma GCC warning "Function decreapated"
}

void adc_setup() {
  if (HAL_ADC_Start_DMA(&hadc1, ADC_A_RAW_DATA, 5) != HAL_OK)
    Error_Handler();
  if (HAL_ADC_Start_DMA(&hadc2, ADC_B_RAW_DATA, 10) != HAL_OK)
    Error_Handler();
}

void adc_loop() {
  // FIXME: solo ADC_A_RAW_DATA[0-1] tiene data, el resto llega en 0
  // EDIT: ADC_B funciona ok, ADC_A no funca en modo circular
  /*  HAL_ADC_Start_DMA(&hadc1, ADC_A_RAW_DATA, 5); */
  HAL_Delay(15);
  /*   HAL_ADC_Stop(&hadc1); */

  /*  HAL_ADC_Start_DMA(&hadc2, ADC_B_RAW_DATA, 10);
   HAL_Delay(15); */
  /*   HAL_ADC_Stop(&hadc2); */
}

uint16_t get_adc_data(uint8_t channel) {
#pragma GCC warning "Function decreapated"
}
