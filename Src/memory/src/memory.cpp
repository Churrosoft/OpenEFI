
#include "../include/memory.hpp"
#include "../include/commands.hpp"
#include <stdio.h>

bool memory::is_busy() {
  if (HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_READY) {
    memory::CS(true);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)GET_STATUS, 1, 50);
    // el primer bit indica si la memoria sigue grabando/leyendo data
    uint8_t data = 0xFF;
    HAL_SPI_Receive(&hspi2, (uint8_t *)data, 1, 50);

    bool busy = (bool)get_bit(data, STATUS_BUSY);
    memory::CS(false);
    return busy;
  }
  return true;
}

void memory::CS(bool set) {
  if (set)
    HAL_GPIO_WritePin(
      MEMORY_CS_GPIO_Port, MEMORY_CS_Pin,
      GPIO_PinState::GPIO_PIN_SET
    );
  else
    HAL_GPIO_WritePin(
      MEMORY_CS_GPIO_Port, MEMORY_CS_Pin,
      GPIO_PinState::GPIO_PIN_RESET
    );
}

uint8_t memory::get_id() {
  uint8_t data = 0xFF;
  if (!memory::is_busy()) {
    memory::CS(true);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)GET_ID, 1, 50);

    HAL_SPI_Transmit(&hspi2, (uint8_t *)GET_ID, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)GET_ID, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)GET_ID, 1, 50);
    // primer byte descartado, trae el ID del fabricante, en este caso es
    // siempre 0xEF
    HAL_SPI_Receive(&hspi2, (uint8_t *)data, 1, 50);
    HAL_SPI_Receive(&hspi2, (uint8_t *)data, 1, 50);

    memory::CS(false);
  }
  return data;
}

uint16_t memory::read_single(uint8_t a, uint8_t b, uint8_t c) {
  uint8_t data = 0xFF;
  if (!memory::is_busy()) {
    memory::CS(true);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)READ_DATA, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)a, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)b, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)c, 1, 50);

    HAL_SPI_Receive(&hspi2, &data, 1, 200);
    memory::CS(false);
  }
  return data;
}

void memory::read_multiple(uint8_t a, uint8_t b, uint8_t c, uint8_t *buffer,
                           uint8_t size) {
  if (!memory::is_busy()) {
    memory::CS(true);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)READ_DATA, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)a, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)b, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)c, 1, 50);

    HAL_SPI_Receive(&hspi2, buffer, size, 200);
    memory::CS(false);
  }
}

void memory::write_single(uint8_t a, uint8_t b, uint8_t c, uint8_t data) {
  if (!memory::is_busy()) {
    memory::CS(true);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)PAGE_PROGRAM, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)a, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)b, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)c, 1, 50);

    HAL_SPI_Transmit(&hspi2, (uint8_t *)data, 1, 200);
    memory::CS(false);
  }
}

void memory::write_multiple(uint8_t a, uint8_t b, uint8_t c, uint8_t *buffer,
                            uint8_t size) {
  if (!memory::is_busy()) {
    memory::CS(true);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)PAGE_PROGRAM, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)a, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)b, 1, 50);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)c, 1, 50);

    HAL_SPI_Transmit(&hspi2, buffer, size, 200);
    memory::CS(false);
  }
}