#include "commands.hpp"
#include "commands_definition.hpp"
#include <stdarg.h>

using namespace web_serial;
uint8_t serial_cache[128];

serial_command web_serial::create_command(uint16_t input_command,
                                          uint8_t *payload) {
  serial_command comm;
  comm.protocol = payload[0];
  comm.command = input_command;
  std::copy(payload, payload + 123, comm.payload);

  uint8_t serialized_command[126];
  std::fill_n(serialized_command, 126, 0xFF);
  serialize_command(comm, serialized_command);
  uint16_t comm_crc = crc16(serialized_command, 126);

  comm.crc[0] = (comm_crc >> 8) & 0xFF;
  comm.crc[1] = comm_crc & 0xFF;

  return comm;
}

void web_serial::export_command(serial_command command,
                                uint8_t (&buffer)[128]) {
  buffer[0] = 1; // protocol
  buffer[1] = (command.command >> 8) & 0xFF;
  buffer[2] = command.command & 0xFF;
  std::copy(command.payload, command.payload + 123, buffer + 3);
  buffer[126] = command.crc[0];
  buffer[127] = command.crc[1];
}

void web_serial::loop() {
  if (CDC_ReadRxBuffer_FS(serial_cache, 128) == USB_CDC_RX_BUFFER_OK) {
    web_serial::serial_command usb_command;

    usb_command.protocol = serial_cache[0];

    usb_command.command = ((int16_t)serial_cache[1] << 8) + serial_cache[2];

    usb_command.crc[0] = serial_cache[126];
    usb_command.crc[1] = serial_cache[127];

    std::copy(serial_cache + 3, serial_cache + 123, usb_command.payload);

    // web_serial::import_command(serial_cache, usb_command);
    web_serial::queue_command(usb_command);
    CDC_FlushRxBuffer_FS();

    /*     trace_printf("------------------------------\n");
        trace_printf("Event: <USB> New command ready\n");
        trace_printf("Payload; %d;%d;%d;%d;", usb_command.payload[0],
                     usb_command.payload[1], usb_command.payload[2],
                     usb_command.payload[3]); */
  }
}

uint8_t web_serial::send_debug_message(debugMessage message_type,
                                       const char *format, ...) {

  if (!web_serial::paired)
    return -1;

  int ret;
  va_list ap;

  va_start(ap, format);
  static char buf[123];

  ret = vsnprintf(buf, sizeof(buf), format, ap);
  if (ret > 0) {
    serial_command comm;
    uint16_t command;

    switch (message_type) {
    case debugMessage::LOG:
      command = EFI_DEBUG_LOG;
      break;
    case debugMessage::INFO:
      command = EFI_DEBUG_INFO;
      break;
    case debugMessage::EVENT:
      command = EFI_DEBUG_EVENT;
      break;
    case debugMessage::ERROR:
      command = EFI_DEBUG_ERROR;
      break;
    default:
      command = EFI_DEBUG_LOG;
      break;
    }
    comm = create_command(command, (uint8_t *)buf);
    web_serial::send_command(comm);
  }

  va_end(ap);
  return 1;
}
