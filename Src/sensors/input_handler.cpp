
#include "input_handler.hpp"
#include "spi.h"

struct input_handler inputs;

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
  uint8_t main_addr = 0b00000110;
  uint16_t sec_addr = 0b0; // ch0 || ch4;

  if (channel > 3)
    main_addr = 0b00000111;

  if (channel == 1 || channel == 5)
    sec_addr = 0b0100000000000000;

  if (channel == 2 || channel == 6)
    sec_addr = 0b1000000000000000;

  if (channel == 3 || channel == 7)
    sec_addr = 0b1100000000000000;

  HAL_SPI_Transmit(&hspi2, (uint8_t *)main_addr, 1, 50);

  HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)sec_addr, out_data, 2, 50);

  inputs.values[channel].actualValue =
      (int32_t)((uint16_t)out_data[0] << 8) + out_data[1];

  inputs.values[channel] = EMALowPassFilter(inputs.values[channel]);
  return inputs.values[channel].actualValue;
}

void input_setup() {
#pragma GCC warning "Function decreapated"
}

void adc_setup() {
#pragma GCC warning "Function decreapated"
}

void adc_loop(){
#pragma GCC warning "Function decreapated"
}

int32_t get_adc_data(uint8_t channel) {
#pragma GCC warning "Function decreapated"
}
