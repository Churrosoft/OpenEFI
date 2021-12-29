#include "commands.hpp"

#include <vector>

using namespace web_serial;

std::vector<serial_command> pending_commands;

void web_serial::command_handler()
{
    //
    if (pending_commands.size())
    {
        serial_command command = pending_commands.back();
        trace_printf("CRC 0: %d \n", command.crc[0]);
        trace_printf("CRC 1: %d \n", command.crc[1]);
        pending_commands.pop_back();
    }
}

void web_serial::queue_command(serial_command command)
{
    pending_commands.push_back(command);
}
