
#include "input_handler.hpp"

#include "spi.h"
extern "C" {
#include "trace.h"
#include "w25qxx.h"
}
struct input_handler inputs;
uint8_t W25qxx_Spi(uint8_t Data)
{
	uint8_t ret;
	HAL_SPI_TransmitReceive(&hspi2, &Data, &ret, 1, 100);
	return ret;
}
/*
http://ww1.microchip.com/downloads/en/DeviceDoc/21298e.pdf
Channel selection:
  TX only:       |        TX/RX
0 0 0 0 0 1 1 0  | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH0
0 0 0 0 0 1 1 0  | 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH1
0 0 0 0 0 1 1 0  | 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH2
0 0 0 0 0 1 1 0  | 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH3

0 0 0 0 0 1 1 1  | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH4
0 0 0 0 0 1 1 1  | 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH5
0 0 0 0 0 1 1 1  | 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH6
0 0 0 0 0 1 1 1  | 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 single-ended CH7
*/

int32_t get_input(uint8_t channel) {
  uint8_t out_data[2] = {0xff, 0xff};
	uint8_t ret;

  uint8_t addr = 0b01100000 | ((channel & 0b111) << 2);

  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);

	HAL_SPI_TransmitReceive(&hspi2, &addr, &ret, 1, 100);
  HAL_SPI_TransmitReceive(&hspi2, 0, out_data, 2, 100);

  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_SET);
  trace_printf("%d--%d", out_data[0], out_data[1]);
  volatile uint16_t debug_value = ((uint16_t)(out_data[0] << 4) | (out_data[1] >> 4));

  inputs.values[channel].actualValue = (int32_t)((uint16_t)(out_data[0] << 4) | (out_data[1] >> 4));

  inputs.values[channel] = EMALowPassFilter(inputs.values[channel]);
  return inputs.values[channel].actualValue;
}

int32_t get_adc_data(uint8_t channel) {
  uint8_t out_data[2] = {0xff, 0xff};

  uint8_t addr = 0b01100000 | ((channel & 0b111) << 2);

  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_Transmit(&hspi2, &addr, 1, 50);
  HAL_SPI_Receive(&hspi2, out_data, 2, 50);

  HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_SET);

  return (int32_t)((uint16_t)(out_data[0] << 4) | (out_data[1] >> 4));
}
