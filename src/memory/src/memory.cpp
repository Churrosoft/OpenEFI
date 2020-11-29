
#include "../include/memory.hpp"
#include "../include/commands.hpp"
#include <stdio.h>

bool memory::is_busy() {
  send_command(GET_STATUS);
  // el primer bit indica si la memoria sigue grabando/leyendo data
  bool busy = (bool)get_bit(read_byte(), STATUS_BUSY);
  close_connection();
  return busy;
}

uint8_t memory::get_id() {
  uint8_t data = 0xFF;
  if (!memory::is_busy()) {
    send_command(GET_ID);
    send_command(0x00);
    send_command(0x00);
    send_command(0x00);
    // primer byte descartado, trae el ID del fabricante, en este caso es
    // siempre 0xEF
    data = read_byte();
    close_connection();
  }
  return data;
}

uint16_t memory::read_single(uint8_t a, uint8_t b, uint8_t c) {
  if (!memory::is_busy()) {
    send_command(READ_DATA);
    send_command(a);
    send_command(b);
    send_command(c);
    uint8_t data = read_byte();
    close_connection();
    return data;
  }
  return 0x0;
}

void memory::read_multiple(uint8_t a, uint8_t b, uint8_t c, uint8_t *buffer,
                           uint8_t size) {
  if (!memory::is_busy()) {
    send_command(READ_DATA);
    send_command(a);
    send_command(b);
    send_command(c);
    for (uint8_t i = 0; i < size; i++)
      buffer[i] = read_byte();
    close_connection();
  }
}

void memory::write__single(uint8_t a, uint8_t b, uint8_t c, uint8_t data) {
  if (!memory::is_busy()) {
    send_command(PAGE_PROGRAM);
    send_command(a);
    send_command(b);
    send_command(c);
    send_command(data);
    close_connection();
  }
}

void memory::write_multiple(uint8_t a, uint8_t b, uint8_t c, uint8_t *buffer,
                            uint8_t size) {
  if (!memory::is_busy()) {
    send_command(PAGE_PROGRAM);
    send_command(a);
    send_command(b);
    send_command(c);
    for (uint8_t i = 0; i < size; i++)
      send_command(buffer[i]);
    close_connection();
  }
}