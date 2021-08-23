#include "../include/memory_dtc.hpp"

uint16_t get_all_dtc()
{
    // lee la memoria buscando la cantidad de dtc's existentes
    uint16_t dtc_cant = 0;

    for (uint16_t addr = 0; addr < 50; addr++)
    {
        uint8_t dtc_readed[5];
        read_dtc(addr, dtc_readed);

        volatile uint8_t dtc = dtc_readed[0];

        if (dtc != DTC_EMPTY && dtc != DTC_EMPTY_ALT)
            dtc_cant += 1;
        else
            break;
    }
    return dtc_cant;
}

bool is_dtc()
{
    // devuelve true en caso de que exista un dtc;
    uint16_t data = memory::read_single(DTC_FLAG_ADDR);
    return data != 0xFE;
}

void write_dtc(uint8_t *dtc_code)
{
    uint16_t dtc_number = get_all_dtc();
    if (dtc_number && dtc_number < 5)
    {
        memory::write_multiple(DTC_INIT_ADDR + ((dtc_number + 1) * 5), dtc_code, 5);
        //  memory::write_single(DTC_FLAG_ADDR, 0);
    }
    else
    {
        memory::write_multiple(DTC_INIT_ADDR, dtc_code, 5);
        // memory::write_single(DTC_FLAG_ADDR, 0);
    }
}

void write_dtc(uint8_t *dtc_code, uint8_t dtc_number)
{
    memory::write_multiple(DTC_INIT_ADDR + (dtc_number * 5), dtc_code, 5);
}

// busca un codigo en la memoria para ver si ya existe, (no vamo a grabar las cosas dos veces vite')
bool search_dtc(uint8_t *dtc_code)
{
    return false;
}

void read_dtc(uint8_t dtc_number, uint8_t *data)
{
    uint32_t ptr = (DTC_INIT_ADDR + (dtc_number * 5));
    memory::read_multiple(ptr, data, 5);
    data[5] = '\0';
}
