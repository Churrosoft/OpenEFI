#include <stdint.h>
#include "defines.h"
#include "../utils/basic_electronics.h"
#include "../include/map.hpp"

uint16_t MAP::get_value(uint16_t filt_input)
{
    return (uint16_t)MAP_CAL(filt_input);
}

uint32_t MAP::get_calibrate_value(uint16_t filt_input)
{
    return (uint32_t)MAP_CAL(filt_input) * 100;
}

uint8_t MAP::dtc(uint16_t in)
{
    if (in > MAP_MAX)
        return 1;
    if (in < MAP_MIN)
        return -1;
    return 0;
}
