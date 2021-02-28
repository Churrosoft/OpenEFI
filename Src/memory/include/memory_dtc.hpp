#ifndef MEMORY_DTC
#define MEMORY_DTC
/**
 * DTC Example:
 * primer byte en primer pagina podria indicar si hay o no dtcs grabados
 * P0XXX => 6 char => 6 bytes (byte adicional para indicar si es temporal | permanente?)
 * max dtc on memory => 50 => 200 bytes
 * memory init addr: 0x1000
 * memory end addr:  0x1FFF
 */
#include <stdint.h>
#include "memory.hpp"
#include "dtc_codes.h"

uint16_t get_all_dtc();
void write_dtc(char *);
uint8_t * read_dtc(uint8_t);

#define DTC_FLAG_ADDR 0x1000

#define DTC_INIT_ADDR 0x1001
#define DTC_END_ADDR  0x1FFF
#define DTC_EMPTY 0xFFFF

#endif