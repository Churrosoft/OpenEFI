#include "../../helpers/spi_common.h"
#include "../include/commands.hpp"
#include <stdio.h>

#ifndef MEMORY
#define MEMORY

namespace memory {

bool is_busy(void);

uint8_t get_id(void);

uint16_t read_single(uint8_t, uint8_t, uint8_t);

void read_multiple(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t);

void write__single(uint8_t, uint8_t, uint8_t, uint8_t);
void write_multiple(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t);
} // namespace memory

#endif