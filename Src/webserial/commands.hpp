#include <algorithm>
#include <deque>

#include "commands_definition.hpp"
#include "memory/include/tables.hpp"
#include "usbd_cdc_if.h"

extern "C" {
#include "trace.h"
}

#ifndef WEBSERIAL_COMMANDS_HPP
#define WEBSERIAL_COMMANDS_HPP

namespace web_serial {

  typedef struct {
    uint8_t protocol;
    uint8_t command;
    uint8_t status;
    uint8_t payload[123];
    bool is_valid;
    uint8_t crc[2];
  } serial_command;

  namespace messageType {
    enum _MessageType { LOG = 0, INFO, EVENT, ERROR };
  }
  typedef messageType::_MessageType debugMessage;

  serial_command create_command(uint8_t input_command, uint8_t status, uint8_t payload[]);
  void export_command(serial_command command, uint8_t (&buffer)[128]);
  void send_command(serial_command command);
  void queue_command(serial_command command);
  bool check_crc(serial_command input_command);
  void reply_command(uint8_t command, uint8_t status, uint8_t *payload);
  void queue_reply_command(uint8_t command, uint8_t status, uint8_t *payload);

  void setup(void);
  void loop(void);           // called on main, for read on RX buff
  void send_deque(void);     // called on main, for write to TX buff
  void command_handler();    // called timered on main, output commands

  extern bool paired;
  extern table_data in_table;
  extern std::deque<serial_command> pending_commands;
  extern std::deque<serial_command> output_commands;

  uint8_t send_debug_message(debugMessage, const char *, ...);

  namespace {

    static inline uint16_t crc16(const unsigned char *data_p, uint8_t length) {
      uint8_t x;
      uint16_t crc = 0xFFFF;

      while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
      }
      return crc;
    }

    static inline void serialize_command(serial_command command, uint8_t (&buffer)[126]) {
      buffer[0] = 1;    // protocol
      buffer[1] = command.command;
      buffer[2] = command.status;
      std::copy(command.payload, command.payload + 123, buffer + 3);
    }
  }    // namespace

}    // namespace web_serial

#endif