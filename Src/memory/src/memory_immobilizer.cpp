#include "../include/memory_immobilizer.hpp"

bool can_turn_on()
{
    uint16_t data = memory::read_single(EMERGENCY_FLAG_ADDR);
    if (data == 0xFE)
        return true;
    return false;
}

void write_emergency_stop(uint8_t *dtc_code)
{
    memory::write_single(EMERGENCY_FLAG_ADDR, 250);

    memory::write_multiple(EMERGENCY_DTC_ADDR, dtc_code, 5);
}