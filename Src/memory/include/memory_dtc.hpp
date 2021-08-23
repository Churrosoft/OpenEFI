#ifndef MEMORY_DTC
#define MEMORY_DTC
/**
 * DTC Example:
 * primer byte en primer pagina podria indicar si hay o no dtcs grabados
 * segundo byte cantidad total de DTC guardados
 * P0XXX => 6 char => 6 bytes (byte adicional para indicar si es temporal | permanente?)
 * max dtc on memory => 50 => 200 bytes
 * memory init addr: 0x1000
 * memory end addr:  0x1FFF
 */
#include <stdint.h>
#include <initializer_list>
#include "memory.hpp"
#include "dtc_codes.h"

uint16_t get_all_dtc();
// void write_dtc(std::initializer_list<uint8_t *>, uint8_t);

void write_dtc(uint8_t *);
void read_dtc(uint8_t, uint8_t *);
void write_dtc(uint8_t *,uint8_t);
void read_dtc(uint8_t, uint8_t *);

#define DTC_FLAG_ADDR 0x1000

#define DTC_INIT_ADDR 0xAFF
#define DTC_END_ADDR 0x1FFF
#define DTC_EMPTY 0xFE
#define DTC_EMPTY_ALT 0xFF

#endif