#include "../include/tps.hpp"

uint8_t TPS::get_value(uint16_t filt_input)
{
    uint16_t _t = convert_to_volt(filt_input) * 1334;
    if (_t > TPS_MAX_A || _t < TPS_MIN_A)
        return 0;
    return (uint8_t)TPS_CALC_A(_t);
}

uint32_t TPS::get_calibrate_value(uint16_t filt_input)
{
    uint16_t _t = convert_to_volt(filt_input) * 1334;
    if (_t > TPS_MAX_A || _t < TPS_MIN_A)
        return 0;
    return (uint32_t)TPS_CALC_A(_t) * 1000;
}

uint8_t TPS::dtc(uint16_t in1)
{
    if (in1 > TPS_MAX_A)
        return 1;
    if (in1 < TPS_MIN_A)
        return -1;
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