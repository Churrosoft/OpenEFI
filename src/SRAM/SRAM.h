#ifndef SRAM_H
#define SRAM_H

//Libs de opencm3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>

//Libs C++ generales

#include <stdio.h>
#include <errno.h>
#include <cstdint>

//Defines memoria
#define READ_BYTE  0x03
#define WRITE_BYTE 0x02

//Defines SPI
#define SS   GPIO4
#define SCK  GPIO5
#define MISO GPIO6
#define MOSI GPIO7

namespace SRAM {
    //data de memoria:
    const uint32_t ramSize = 0x1FFFF;   // 128K x 8 bit

    bool Init();
    //lectura:
    int ReadInt();
    char ReadChar();
    char * ReadString();
    //escritura:
    bool Write(uint32_t addr, int value);
    bool Write(uint32_t addr, char value);
    bool Write(uint32_t addr, char * value);
    /* solo uso interno, o sino rompes todo maquinola */
    void SetAddr(uint32_t addr, byte mode);
};

#endif