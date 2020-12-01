#ifndef MEMORY_COMMANDS
#define MEMORY_COMMANDS

// IMPORTANTEEEE, memoria usa big endian, mcu little endian
#define get_bit(data, bit) ((data >> bit) & 1)

/**
 * GET_STATUS => on datasheet "Read Status Register, page 17"
 *
 * STATUS REGISTER DATA
 *
 * S7 => Status Register Protect
 * S6 => Reserverd
 * S5 | S2 => Memory Protection
 * S1 => WEL => Write Enable Latch
 * S0 => Memory BUSY
 */
#define GET_STATUS 0x5

#define STATUS_BUSY 0x7

/**
 * READ_DATA => on datasheet "Read Data, page 19"
 */
#define READ_DATA 0x3
/**
 * GET_ID => on datasheet "Read Manufactuer / Device ID, page 33"
 */
#define GET_ID 0x90

#define WRITE_ENABLE 0x6
#define WRITE_DISABLE 0x4

/**
 * PAGE_PROGRAM => on datasheet "Page Program, page 25"
 * programs one byte to one page (256b)
 */

#define PAGE_PROGRAM 0x2

#endif