#include "commands.hpp"

extern "C"
{
#include "trace.h"
}

using namespace web_serial;

serial_command pending_commands[200];
uint8_t pending_commands_index = 0;

void web_serial::command_handler()
{
    //
    if (pending_commands_index)
    {
        serial_command command = pending_commands[0];
        trace_printf("CRC 0: %d \n", command.crc[0]);
        trace_printf("CRC 1: %d \n", command.crc[1]);
        pending_commands_index = 0;
    }
}

void web_serial::queue_command(serial_command command)
{
    pending_commands[pending_commands_index] = command;
    pending_commands_index++;
}
