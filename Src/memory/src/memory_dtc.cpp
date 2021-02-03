#include "../include/memory_dtc.hpp"

uint16_t get_all_dtc()
{
    /*  // lee la memoria buscando la cantidad de dtc's existentes
    uint16_t dtc_cant = 0;
    for (uint16_t i = DTC_INIT_ADDR; i < DTC_END_ADDR; i += 5)
    {
        uint8_t *dtc = 0;
        memory::read_multiple(i >> 24, i >> 16, i >> 8, dtc, 4);
        if ((uint32_t)dtc != DTC_EMPTY)
            dtc_cant += 1;
    }
    return dtc_cant; */
    return memory::read_single(DTC_FLAG_ADDR >> 24, DTC_FLAG_ADDR >> 16, DTC_FLAG_ADDR >> 8);
}

/* bool is_dtc(){
    // devuelve true en caso de que exista un dtc;
    uint16_t data = memory::read_single(DTC_FLAG_ADDR >> 24, DTC_FLAG_ADDR >> 16, DTC_FLAG_ADDR >> 8);
    return data == 0xFF;
} */

void write_dtc(uint8_t *dtc_code)
{
    uint16_t dtc = get_all_dtc();
    if (dtc != 0xFF)
    {
        uint32_t ptr = (DTC_FLAG_ADDR + dtc);
        memory::write_multiple(ptr >> 24, ptr >> 16, ptr >> 8, dtc_code, 5);
        memory::write_single(DTC_FLAG_ADDR >> 24, DTC_FLAG_ADDR >> 16, DTC_FLAG_ADDR >> 8, dtc + 1);
    }
    else
    {
        memory::write_multiple(DTC_INIT_ADDR >> 24, DTC_INIT_ADDR >> 16, DTC_INIT_ADDR >> 8, dtc_code, 5);
        memory::write_single(DTC_FLAG_ADDR >> 24, DTC_FLAG_ADDR >> 16, DTC_FLAG_ADDR >> 8, 0);
    }
}

uint8_t *read_dtc(uint8_t dtc_number)
{
    uint8_t *data = 0;
    uint32_t ptr = (DTC_FLAG_ADDR + dtc_number);
    memory::read_multiple(ptr >> 24, ptr >> 16, ptr >> 8, data, 5);
    return data;
}
