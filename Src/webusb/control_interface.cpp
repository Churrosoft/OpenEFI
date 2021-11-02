/* USB Control Interface command processing. */
extern "C"
{

#include <string.h>
#include "webusb_commands.h"
#include "defines.h"
#include "variables.h"
//Structs para formatear data:
#include "status.h"
    // #include "../helpers/bootloader.c"
}
#include "usbd_cdc_if.h"
#include "control_interface.h"
#include "trace.h"

//Variables de todo el socotroco:
struct dataBuffer
{
    uint8_t buffer[128];
    int dataSize;
} buffer = {
    {}, 0};

/** Procesa comandos.
 * @param usbd_dev
 * @param message Mensaje recibido.
 */
void process_frame(/* usbd_device* usbd_dev, */ SerialMessage *message)
{
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

    SerialMessage response = {PROTOCOL_VERSION_1, 0, 0, {}, 0};
    response.protocolVersion = PROTOCOL_VERSION_1;
    response.command = message->command;
    uint16_t localCRC = crc16((uint8_t *)message, 126);
    /*     if (localCRC != message->checksum)
    {
        response.command = COMMAND_ERR;
        response.subcommand = ERROR_INVALID_CHECKSUM;
        send_message(&response);
        return;
    } */

    trace_printf("command: %d \n", message->command);

    Status _status = {{}, 0, 0, 0};
    switch (message->protocolVersion)
    {
    case PROTOCOL_VERSION_1:
        switch (message->command)
        {
        case COMMAND_PING:
            memcpy(response.payload, message->payload, sizeof(response.payload));
            break;
        case COMMAND_HELLO:
            // XXX: los datos deberian ir al final del payload y no al comienzo...
            response.payload[0] = OPENEFI_VER_MAJOR;
            response.payload[1] = OPENEFI_VER_MINOR;
            response.payload[2] = OPENEFI_VER_REV;
            break;
        case COMMAND_STATUS:
            //switch (message->subcommand){
            //case STATUS_TMP:
            _status.RPM = _RPM;
            _status.TEMP = 420;
            _status.V00 = 69;
            memcpy(response.payload, &_status, 123);
            //    break;
            //default:
            //    response.command = COMMAND_ERR;
            //    response.subcommand = ERROR_INVALID_COMMAND;
            //    break;
            //}
            break;
        case COMMAND_BOOTL_SW:
            // Escribimos los bytes mágicos en los registros backup rtc
            // para forzar el arranque del bootloader
            //rtc_backup_write(0, 0x544F4F42UL);
            //scb_reset_system();
            return;
        default:
            trace_printf("ERR: invalid command, command: %d \n", message->command);
            response.command = COMMAND_ERR;
            response.subcommand = ERROR_INVALID_COMMAND;
        }
        break;

    default:
        // Protocolo invalido.
        trace_printf("ERR: invalid protocol, command: %d \n", message->command);

        response.command = COMMAND_ERR;
        response.subcommand = ERROR_INVALID_PROTOCOL;
    }

    send_message(&response);
    send_message(&response);

    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
}
/** envia mensajes
 * @param usbd_dev puntero a constructor del USB
 * @param message Mensaje a enviar.
 */
void send_message(/* usbd_device *usbd_dev,  */ SerialMessage *message)
{
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    trace_printf("init crc16 \n");
    message->checksum = crc16((uint8_t *)message, 126);
    uint8_t *msg = (uint8_t *)message;
    trace_printf("end crc16 \n");

    CDC_Transmit_FS(msg, sizeof(msg) - 1);
    trace_printf("end CDC Transmit \n");

    /*     CDC_Transmit_FS(msg, 64);
    for (int i = 0; i < 0x8000; i++)
        __asm__("nop");
    CDC_Transmit_FS((msg + 64), 64); */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

    /*  usbd_ep_write_packet(usbd_dev, 0x82, msg, 64);
    for (int i = 0; i < 0x8000; i++) __asm__("nop");
    usbd_ep_write_packet(usbd_dev, 0x82, (msg + 64), 64); */
}

bool get_frame(uint8_t *tempbuf, int len)
{

    if ((buffer.dataSize + len) <= 128)
    {
        memcpy(buffer.buffer + buffer.dataSize, tempbuf, len);
        buffer.dataSize += len;
        if (buffer.dataSize >= 128)
        {
            return true;
        }
    }
    return false;
}

uint8_t *get_msg()
{
    return buffer.buffer;
}

void clear_msg()
{
    memset(buffer.buffer, 0, buffer.dataSize);
    buffer.dataSize = 0;
}

uint16_t crc16(uint8_t *data_p, uint8_t length)
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