#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>

#ifndef SPI_COMMON
#define SPI_COMMON

extern bool spi_enabled;

void spi_init(void);
void send_command(uint8_t);
uint8_t read_byte(void);
void close_connection(void);

#endif