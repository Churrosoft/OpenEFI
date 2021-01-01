#include "../include/commands.hpp"
#include "spi.h"
#include <stdio.h>
#include <stm32f4xx_hal_gpio.h>

#ifndef MEMORY
#define MEMORY

namespace memory {

bool is_busy(void);

uint8_t get_id(void);

uint16_t read_single(uint8_t, uint8_t, uint8_t);

void read_multiple(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t);

void write__single(uint8_t, uint8_t, uint8_t, uint8_t);
void write_multiple(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t);

void CS(bool state);
} // namespace memory

#endif