#include "defines.h"
#include <algorithm>

#ifndef WEBSERIAL_COMMANDS_HPP
#define WEBSERIAL_COMMANDS_HPP

namespace web_serial
{

    typedef struct
    {
        uint16_t command;
        uint8_t payload[123];
        uint8_t crc[2];
    } serial_command;

    serial_command create_command(uint16_t input_command, uint8_t payload[]);

    bool check_crc(serial_command input_command);

    void export_command(serial_command command, uint8_t (&buffer)[128]);
    void import_command(uint8_t (&buffer)[128], serial_command command);

    void command_handler();
    void queue_command(serial_command command);

    namespace
    {
        static inline uint16_t crc16(const unsigned char *data_p, uint8_t length)
        {
            uint8_t x;
            uint16_t crc = 0xFFFF;

            while (length--)
            {
                x = crc >> 8 ^ *data_p++;
                x ^= x >> 4;
                crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
            }
            return crc;
        }

        static inline void serialize_command(serial_command command, uint8_t (&buffer)[126])
        {
            buffer[0] = 1; // protocol
            buffer[1] = (command.command >> 8) & 0xFF;
            buffer[2] = command.command & 0xFF;
            std::copy(command.payload, command.payload + 123, buffer + 3);
        }
    } // namespace

} // namespace WebSerial

#endif