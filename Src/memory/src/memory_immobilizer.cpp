#include "../include/memory_immobilizer.hpp"

bool can_turn_on()
{
    uint16_t data = memory::read_single(EMERGENCY_FLAG_ADDR >> 24, EMERGENCY_FLAG_ADDR >> 16, EMERGENCY_FLAG_ADDR >> 8);
    if (data == 0xFF)
        return true;
    return false;
}

void write_emergency_stop(uint8_t *dtc_code)
{
    memory::write_single(EMERGENCY_FLAG_ADDR >> 24, EMERGENCY_FLAG_ADDR >> 16, EMERGENCY_FLAG_ADDR >> 8, 250);

    memory::write_multiple(EMERGENCY_DTC_ADDR >> 24, EMERGENCY_DTC_ADDR >> 16, EMERGENCY_DTC_ADDR >> 8, dtc_code, 5);
}