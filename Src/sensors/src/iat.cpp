/*
    todo el codigo relacionado al control del sensor "IAT"
*/
#include <stdint.h>
#include "defines.h"
#include "../include/iat.hpp"
#include "../utils/basic_electronics.c"

uint16_t IAT::get_value(uint16_t filt_input)
{
    return (uint16_t)IAT_CAL(filt_input);
}

uint32_t IAT::get_calibrate_value(uint16_t filt_input)
{
    return (uint32_t)IAT_CAL(filt_input) * 100;
}

uint8_t IAT::dtc(uint16_t in)
{
    if (in > IAT_MAX)
        return 1;
    if (in < IAT_MIN)
        return -1;
    return 0;
}