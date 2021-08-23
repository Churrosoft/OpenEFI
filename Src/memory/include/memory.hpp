#include "./commands.hpp"
#include "spi.h"
#include <stdio.h>
#include "gpio.h"
#include "ll_spi.h"

#ifndef MEMORY
#define MEMORY

namespace memory
{

    bool is_busy(void);

    uint8_t get_id(void);
    void format_address(uint32_t, uint8_t (&)[3]);

    uint16_t read_single(uint32_t);

    void read_multiple(uint32_t, uint8_t *, uint32_t);
    void write_enable(void);
    void write_single(uint32_t, uint8_t);
    void write_multiple(uint32_t, uint8_t *, uint32_t);

    void CS(bool state);
} // namespace memory

// Memory Block usage:
// (la memoria usada en este caso es una W25X10CL, pero sirve cualquiera de 4mb, luego es cambiar comandos/sectores)
/**
 * BLOCK 0: 
 *  Sector 0 [0x0000;0x0FFF]
 *  Sector 1 [0x1000;0x1FFF] DTC codes
 * 
 * BLOCK 1:
 *  Sector 0 []
 */
#endif