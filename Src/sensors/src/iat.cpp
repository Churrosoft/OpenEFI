/*
    todo el codigo relacionado al control del sensor "IAT"
*/
#include <stdint.h>
#include "defines.h"
#include "user_defines.h"

#include "dtc_codes.h"
#include "../include/iat.hpp"
#include "../utils/basic_electronics.c"

uint16_t IAT::get_value(uint16_t filt_input)
{
    IAT::last_value = (uint16_t)IAT_CAL(filt_input);
    return last_value;
}

uint32_t IAT::get_calibrate_value(uint16_t filt_input)
{
    return (uint32_t)IAT_CAL(filt_input) * 100;
}

uint8_t *IAT::dtc()
{
    if (IAT::last_value > IAT_MAX)
        return NEW_DTC DTC_IAT_SENSOR_HIGH;
    if (IAT::last_value < IAT_MIN && IAT::last_value > IAT_OPEN)
        return NEW_DTC DTC_IAT_SENSOR_LOW;
    else
        return NEW_DTC DTC_IAT_SENSOR_OPEN;
    return 0;
}