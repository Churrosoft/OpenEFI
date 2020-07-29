//  Log messages to the debug console.  We use ARM Semihosting to display messages.
//  Should not have any dependencies on other libraries.
#include <string.h>
#include "logger.h"

#define DEBUG_BUFFER_SIZE 512  //  Use a larger buffer size so that we don't interrupt USB processing.
static char debugBuffer[DEBUG_BUFFER_SIZE + 1] = { 0 };  //  Buffer to hold output before flushing.
static volatile uint16_t debugBufferLength = 0;          //  Number of bytes in debug buffer.
static bool logEnabled = false;  //  Logging is off by default.  Developer must switch it on with enable_debug().

void enable_log(void) { logEnabled = true; }
void disable_log(void) { logEnabled = false; }

//  ARM Semihosting code from 
//  http://www.keil.com/support/man/docs/ARMCC/armcc_pge1358787046598.htm
//  http://www.keil.com/support/man/docs/ARMCC/armcc_pge1358787048379.htm
//  http://www.keil.com/support/man/docs/ARMCC/armcc_chr1359125001592.htm
//  https://wiki.dlang.org/Minimal_semihosted_ARM_Cortex-M_%22Hello_World%22

static int __semihost(int command, void* message) {
	//  Send an ARM Semihosting command to the debugger, e.g. to print a message.
	//  To see the message you need to run opencd:
	//    openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -f scripts/connect.ocd

	//  Warning: This code will trigger a breakpoint and hang unless a debugger is connected.
	//  That's how ARM Semihosting sends a command to the debugger to print a message.
	//  This code MUST be disabled on production devices.
    if (!logEnabled) return -1;
    __asm( 
      "mov r0, %[cmd] \n"
      "mov r1, %[msg] \n" 
      "bkpt #0xAB \n"
	:  //  Output operand list: (nothing)
	:  //  Input operand list:
		[cmd] "r" (command), 
		[msg] "r" (message)
	:  //  Clobbered register list:
		"r0", "r1", "memory"
	);
	return 0;  //  TODO
}

//  ARM Semihosting code from https://github.com/ARMmbed/mbed-os/blob/master/platform/mbed_semihost_api.c

// ARM Semihosting Commands
// #define SYS_OPEN   (0x1)
// #define SYS_CLOSE  (0x2)
#define SYS_WRITE  (0x5)
// #define SYS_READ   (0x6)
// #define SYS_ISTTY  (0x9)
// #define SYS_SEEK   (0xa)
// #define SYS_ENSURE (0xb)
// #define SYS_FLEN   (0xc)
// #define SYS_REMOVE (0xe)
// #define SYS_RENAME (0xf)
// #define SYS_EXIT   (0x18)

// We normally set the file handle to 2 to write to the debugger's stderr output.
#define SEMIHOST_HANDLE 2

static int semihost_write(uint32_t fh, const unsigned char *buffer, unsigned int length) {
    //  Write "length" number of bytes from "buffer" to the debugger's file handle fh.
    //  We normally set fh=2 to write to the debugger's stderr output.
    if (length == 0) { return 0; }
    uint32_t args[3];
    args[0] = (uint32_t)fh;
    args[1] = (uint32_t)buffer;
    args[2] = (uint32_t)length;
    return __semihost(SYS_WRITE, args);
}

//  Don't flush to Arm Semihosting if we are called by an Interrupt Service Routine.  From libopencm3:
#define MMIO32(addr)    (*(volatile uint32_t *)(addr))  /* Generic memory-mapped I/O accessor functions */
#define PPBI_BASE       (0xE0000000U)                   /* Private peripheral bus - Internal */
#define SCS_BASE        (PPBI_BASE + 0xE000)            /* PPBI_BASE + 0x3000 (0xE000 3000 - 0xE000 DFFF): Reserved */
#define SCB_BASE        (SCS_BASE + 0x0D00)             /* SCB: System Control Block */
#define SCB_ICSR	    MMIO32(SCB_BASE + 0x04)         /* ICSR: Interrupt Control State Register */
#define SCB_ICSR_VECTACTIVE_LSB		0                   /* VECTACTIVE[8:0] Active vector */
#define SCB_ICSR_VECTACTIVE		    (0x1FF << SCB_ICSR_VECTACTIVE_LSB)

static uint32_t target_in_isr(void) {
    //  Return true if CPU is in ISR now.
    return SCB_ICSR & SCB_ICSR_VECTACTIVE;
}

//  List of all logger output functions e.g. USB Serial, HF2.
#define MAX_OUTPUT_FUNCS 4
static logger_output_func *output_funcs[MAX_OUTPUT_FUNCS];

int logger_add_output(logger_output_func *func) {
    //  Add a logger output function e.g. USB Serial, HF2.
    if (!func) { return -1; }
    for (int i = 0; i < MAX_OUTPUT_FUNCS; i++) {
        if (output_funcs[i] == func) {
            return 0;  //  Already added.
        }
    }
    for (int i = 0; i < MAX_OUTPUT_FUNCS; i++) {
        if (!output_funcs[i]) {
            output_funcs[i] = func;
            return 0;
        }
    }
    return -1;  //  Too many outputs.
}

#define MAX_OUTPUT_LENGTH 60  //  Output at most 60 bytes at a time.

static uint16_t write_all_output(
    const uint8_t *buf,
	uint16_t len,
    bool forced) {
    //  Write the buffer to all outputs: Arm Semihosting, USB Serial, HF2, ...
    //  We must flush as quickly as possible and USB Serial can only handle 64 bytes, so we just flush the next 60 bytes.
    //  Return the number of bytes flushed.
    if (!logEnabled) { return 0; }  //  Skip if log not enabled.
#ifdef NOTUSED
    ////if (!forced && target_in_isr()) { return 0; }      //  Can't write when called by interrupt routine, unless it's forced.
    //  if (target_in_isr()) { return 0; }      //  Can't write when called by interrupt routine
#endif  //  NOTUSD
    uint16_t outlen = (len > MAX_OUTPUT_LENGTH) ? MAX_OUTPUT_LENGTH : len;

#ifdef ENABLE_SEMIHOSTING
    #warning Semihosting ENABLED
    semihost_write(SEMIHOST_HANDLE, (const unsigned char *) buf, outlen);
#else
    #warning Semihosting DISABLED
#endif  //  ENABLE_SEMIHOSTING
    for (int i = 0; i < MAX_OUTPUT_FUNCS; i++) {
        if (output_funcs[i]) {
            logger_output_func *func = output_funcs[i];
            func(buf, outlen);
        }
    }
    return outlen;
}

void debug_flush_internal(bool forced) {
    //  Flush one chunk of the debug buffer to the debugger log.  This will be slow.
    if (!logEnabled) { debugBufferLength = 0; return; }  //  Skip if log not enabled.
    if (debugBufferLength == 0) { return; }  //  Debug buffer is empty, nothing to write.
	uint16_t outlen = write_all_output((const uint8_t *) debugBuffer, debugBufferLength, forced);
    if (outlen == 0) {
        return;
    } else if (outlen >= debugBufferLength) {
        debugBufferLength = 0;
        return;
    }
    //  Partial write. Copy the remaining bytes.
    memcpy(debugBuffer, &debugBuffer[outlen], debugBufferLength - outlen);
    debugBufferLength -= outlen;
}

void debug_flush(void) {
    //  Flush one chunk of the debug buffer to the debugger log.  This will be slow.
    return debug_flush_internal(false);
}

void debug_force_flush(void) {
    //  Flush the entire debug buffer to the debugger log, even when called by interrupt routine.  This will be slow.
    for (int i = 0; i < 100; i++) {  //  Assume 100 or fewer chunks.
        if (debugBufferLength == 0) { return; }  //  No more chunks.
        debug_flush_internal(true);
    }
}

static void debug_append(const char *buffer, unsigned int length) {
    //  Append "length" number of bytes from "buffer" to the debug buffer.
    if (length >= DEBUG_BUFFER_SIZE) { return; }  //  Don't allow logging of very long messages.
    if (debugBufferLength + length >= DEBUG_BUFFER_SIZE) {
        //  Erase the entire buffer.  Latest log is more important than old log.
        strcpy(debugBuffer, "[DROPPED]");
        debugBufferLength = 9;
        //  Still can't fit after clearing.  Quit.
        if (debugBufferLength + length >= DEBUG_BUFFER_SIZE) { return; }
    }
    //  Else append to the buffer.
    memcpy(&debugBuffer[debugBufferLength], buffer, length);
    debugBufferLength += length;
}

void debug_print(size_t l) {
    //  We only print up to 10 digits, since 32 bits will give max 4,294,967,296.    
    //  char buf[32]; sprintf(buf, "%u", l); debug_append(buf, strlen(buf)); return; ////  TODO
    
    #define MAX_INT_LENGTH 10
    char buffer[MAX_INT_LENGTH + 1];
    int size = MAX_INT_LENGTH + 1;
    bool prefixByZero = false;
    int length = 0;
    for(size_t divisor = 1000000000ul; divisor >= 1; divisor = divisor / 10) {
        char digit = '0' + (char)(l / divisor);
        if (digit > '9') {
            debug_print("(Overflow)");
            return;
        }
        if (digit > '0' || length > 0 || prefixByZero) {
            if (length < size) {
                buffer[length++] = digit;
            }
        }
        l = l % divisor;
    }
    if (length == 0) { buffer[length++] = '0'; };
    if (length < size) buffer[length] = 0;
    buffer[size - 1] = 0;  //  Terminate in case of overflow.

    debug_append(buffer, strlen(buffer));
}

void debug_print(int i) {
    if (i == 0) { debug_append("0", 1); } 
    else if (i >= 0) { debug_print((size_t) i); }
    else {  // i < 0.
        debug_append("-", 1);
        debug_print((size_t) -i);
    }
}

void debug_print(float f) {
    //  Assume max 10 digits to the left of decimal point and 2 digits to the right.
    if (f == 0) {
        debug_append("0.00", 4);
        return;
    } else if (f < 0) {
        debug_append("-", 1);
        f = -f;
    }
    //  Print the digits left of the decimal point.
    debug_print((size_t) f);
    debug_append(".", 1);
    //  Print the 2 digits right of the decimal point.
    f = f * 10.0;
    debug_print(((size_t) f) % 10);
    f = f * 10.0;
    debug_print(((size_t) f) % 10);
}

void debug_begin(uint16_t bps) {
    //  TODO
}

void debug_write(uint8_t ch) {
	debug_append((const char *) &ch, 1);
}

void debug_print(const char *s) {
    if (s[0] == 0) return;
	debug_append(s, strlen(s));
}

void debug_println(const char *s) {
    if (s[0] != 0) { debug_print(s); }
    debug_append("\r\n", 2);
}

void debug_print(char ch) {
	debug_append(&ch, 1);
}

void debug_println(int i) {
    debug_print(i);
    debug_append("\r\n", 2);
}

void debug_println(size_t l) {
    debug_print(l);
    debug_append("\r\n", 2);
}

void debug_println(char ch) {
    debug_print(ch);
    debug_append("\r\n", 2);
}

void debug_println(float f) {
    debug_print(f);
    debug_append("\r\n", 2);
}

void debug_print_int(int i) { debug_print(i); }
void debug_print_unsigned(size_t l) { debug_print(l); }
void debug_print_char(char ch) { debug_print(ch); }
void debug_print_float(float f) { debug_print(f); }

void debug_printhex(uint8_t v) {
    //  Write a char in hexadecimal to the buffered debug log.
    #define MAX_BYTE_LENGTH 2
    char buffer[MAX_BYTE_LENGTH + 1];
    int size = MAX_BYTE_LENGTH + 1;
    bool prefixByZero = true;
    int length = 0;
    for(uint8_t divisor = 16; divisor >= 1; divisor = divisor / 16) {
        char digit = '0' + (char)(v / divisor);
        if (digit > '9') { digit = digit - 10 - '0' + 'a'; }
        if (digit > '0' || length > 0 || prefixByZero) {
            if (length < size) {
                buffer[length++] = digit;
            }
        }
        v = v % divisor;
    }
    if (length == 0) { buffer[length++] = '0'; };
    if (length < size) buffer[length] = 0;
    buffer[size - 1] = 0;  //  Terminate in case of overflow.

    debug_append(buffer, strlen(buffer));
}

void debug_printhex_unsigned(size_t l) {
    //  Write an unsigned int in hexadecimal to the buffered debug log.
    for (int i = sizeof(l) - 1; i >= 0; i--) {
        uint8_t b = 0xff & (l >> (8 * i));
        debug_printhex(b);
    }
}
