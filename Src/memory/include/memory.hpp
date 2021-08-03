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

void read_multiple(uint8_t, uint8_t, uint8_t, uint8_t *, uint16_t);

void write_single(uint8_t, uint8_t, uint8_t, uint8_t);
void write_multiple(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t);

void CS(bool state);
} // namespace memory

// Memory Block usage:
/**
 * BLOCK 0: 
 *  Sector 0 [0x0000;0x0FFF]
 *  Sector 1 [0x1000;0x1FFF] DTC codes
 */
#endif