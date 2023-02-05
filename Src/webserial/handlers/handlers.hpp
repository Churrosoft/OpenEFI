#ifndef OPENEFI_HANDLERS_HPP
#define OPENEFI_HANDLERS_HPP

namespace handlers {
  void handle_core(web_serial::serial_command command);
  void handle_tables(web_serial::serial_command command);
  void handle_efi_config(web_serial::serial_command command);
  void handle_fw(web_serial::serial_command command);
}

#endif    // OPENEFI_HANDLERS_HPP
