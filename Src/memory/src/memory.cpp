
#include "../include/memory.hpp"
#include "../include/commands.hpp"
#include <stdio.h>
#include "main.h"
#include "tim.h"

/**
 * check if memory is still on a Write operation
 * @return boolean with the memory WIP status
 * @see <a href="https://www.microchip.com/en-us/product/25LC1024#document-table" target="_top">Memory Datasheet (later on OpenEFI Docs)</a>
*/

void delay_us2(uint16_t microseconds)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0); // set the counter value a 0
  while (__HAL_TIM_GET_COUNTER(&htim1) < microseconds)
    ; // wait for the counter to reach the us input in the parameter
}

bool memory::is_busy()
{
  return false;
}

void memory::CS(bool set)
{
  if (set)
    HAL_GPIO_WritePin(
        SPI_CS_0_GPIO_Port, SPI_CS_0_Pin,
        GPIO_PinState::GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(
        SPI_CS_0_GPIO_Port, SPI_CS_0_Pin,
        GPIO_PinState::GPIO_PIN_RESET);
}

uint8_t memory::get_id()
{
  uint8_t data = 0xFF;
  return data;

  //not valid for 25LC1024 memory
  if (!memory::is_busy())
  {
    memory::CS(true);

    // en teoria se pueden mandar de estar forma los mensajes por spi
    // TODO: implementar No-Blocking con DMA
    uint8_t data[] = {GET_ID, GET_ID, GET_ID, GET_ID};

    // HAL_SPI_Transmit(&hspi1, data, sizeof(data), 50);
    // primer byte descartado, trae el ID del fabricante, en este caso es
    // siempre 0xEF
    // HAL_SPI_Receive(&hspi1, (uint8_t *)data, 1, 50);
    // HAL_SPI_Receive(&hspi1, (uint8_t *)data, 1, 50);

    memory::CS(false);
  }
  return data;
}

uint16_t memory::read_single(uint32_t address)
{
  uint8_t address_data[3];
  memory::format_address(address, address_data);
  uint8_t data = 0xFF;

  if (!memory::is_busy())
  {
    memory::CS(false);
    spi_send_byte(READ_DATA);
    spi_send_multiple(address_data, 3);
    spi_read_byte();
    data = spi_read_byte();
    spi_disable();
    delay_us2(5);
    memory::CS(true);
  }
  return data;
}

void memory::read_multiple(uint32_t address, uint8_t *buffer, uint32_t size)
{
  uint8_t address_data[3];
  memory::format_address(address, address_data);

  if (true)
  {
    memory::CS(false);

    spi_send_byte(READ_DATA);
    spi_send_multiple(address_data, 3);
    spi_read_multiple(buffer, size);
    spi_disable();
    memory::CS(true);
  }
}

void memory::write_enable()
{
  memory::CS(false);
  spi_send_byte(WRITE_ENABLE);
  spi_disable();
  memory::CS(true);
}

void memory::write_single(uint32_t address, uint8_t data)
{
  uint8_t address_data[3];
  memory::format_address(address, address_data);

  if (!memory::is_busy())
  {
    memory::write_enable();
    delay_us2(2);
    memory::CS(false);
    spi_send_byte(PAGE_PROGRAM);
    spi_send_multiple(address_data, 3);
    spi_send_byte(data);
    spi_disable();
    memory::CS(true);
  }
}

void memory::write_multiple(uint32_t address, uint8_t *buffer, uint32_t size)
{

  uint8_t address_data[3];
  memory::format_address(address, address_data);

  if (!memory::is_busy())
  {
    memory::write_enable();
    delay_us2(2);
    memory::CS(false);
    spi_send_byte(PAGE_PROGRAM);
    spi_send_multiple(address_data, 3);
    spi_send_multiple(buffer, size);
    spi_disable();
    memory::CS(true);
    delay_us2(5);
  }
}

/**
 * Cast int32 to 24b word in 3 segments of 8bytes
 * @param address address to be formated
 * @param data array of uint8_t for output
*/
void memory::format_address(uint32_t address, uint8_t (&data)[3])
{
  data[0] = (address >> 16) & 0xFF;
  data[1] = (address >> 8) & 0xFF;
  data[2] = address & 0xFF;
}