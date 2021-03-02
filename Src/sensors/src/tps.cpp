#include "../include/tps.hpp"
#include "dtc_codes.h"

uint8_t TPS::get_value(uint16_t filt_input)
{
    uint16_t _t = convert_to_volt(filt_input) * 1334;
    if (_t > TPS_MAX_A || _t < TPS_MIN_A)
        return 0;
    TPS::last_value = (uint16_t)TPS_CALC_A(_t);
    return (uint8_t)last_value;
}

uint32_t TPS::get_calibrate_value(uint16_t filt_input)
{
    uint16_t _t = convert_to_volt(filt_input) * 1334;
    if (_t > TPS_MAX_A || _t < TPS_MIN_A)
        return 0;
    return (uint32_t)TPS_CALC_A(_t) * 1000;
}

uint8_t *TPS::dtc()
{

    if (TPS::last_value > TPS_MAX_A)
        return NEW_DTC DTC_TPS_OUT_OF_RANGE;
    if (TPS::last_value < TPS_MIN_A)
        return NEW_DTC DTC_TPS_OUT_OF_RANGE;
    return 0;
}

uint8_t TPS::dtc(uint16_t in1, uint16_t in2)
{
    //TODO: laburar x1
    return 0;
}
uint8_t TPS::get_value(uint16_t filt_input, uint16_t filt_input2)
{
    //TODO: laburar x2
    return 0;
}

uint32_t TPS::get_calibrate_value(uint16_t filt_input, uint16_t filt_input2)
{
    //TODO: laburar x3
    return 0;
}