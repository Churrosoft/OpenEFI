//  Log messages to the debug console.  Note: Logging will be buffered in memory.  Messages will not 
//  be displayed until debug_flush() is called.
#ifndef LOGGER_H_
#define LOGGER_H_
#include <stdint.h>   //  For uint8_t
#include <stdlib.h>   //  For size_t
#ifdef DISABLE_DEBUG  //  If logging is disabled...

//  All logging functions get removed. 
#define enable_log() {}
#define disable_log() {}
#define debug_begin(a) {}
#define debug_write(a) {}
#define debug_print(a) {}
#define debug_println(a) {}
#define debug_printhex(a) {}
#define debug_print_int(a) {}
#define debug_print_unsigned(a) {}
#define debug_print_char(a) {}
#define debug_print_float(a) {}
#define debug_flush() {}
#define debug_force_flush() {}
#define logger_add_output(a) {}

#else  //  If logging is enabled...

#ifdef __cplusplus
extern "C" {  //  Allows functions below to be called by C and C++ code.
#endif
void enable_log(void);   //  Enable the debug log.
void disable_log(void);  //  Disable the debug log.
void debug_begin(uint16_t bps);     //  Open the debug console at the specified bits per second.
void debug_write(uint8_t ch);       //  Write a character to the buffered debug log.
void debug_print(const char *s);    //  Write a string to the buffered debug log.
void debug_println(const char *s);  //  Write a string plus newline to the buffered debug log.
void debug_print_int(int i);
void debug_print_unsigned(size_t l);
void debug_print_char(char ch);
void debug_print_float(float f);    //  Note: Always prints with 2 decimal places.
void debug_printhex(uint8_t ch);    //  Write a char in hexadecimal to the buffered debug log.
void debug_printhex_unsigned(size_t l);  //  Write an unsigned int in hexadecimal to the buffered debug log.
void debug_flush(void);             //  Flush the buffer of the debug log so that buffered data will appear.
void debug_force_flush(void);             //  Flush the buffer of the debug log so that buffered data will appear.

typedef int logger_output_func(const uint8_t *buf, uint16_t len);
int logger_add_output(logger_output_func *func);  //  Add a logger output function e.g. USB Serial, HF2.

#ifdef __cplusplus
}  //  End of extern C scope.
#endif

#ifdef __cplusplus  //  Overloaded debug functions for C++ only
//  Write an int / size_t / char / float to the buffered debug log.  Will not be displayed until debug_flush() is called.
void debug_print(int i);
void debug_print(size_t l);
void debug_print(char ch);
void debug_print(float f);  //  Note: Always prints with 2 decimal places.

//  Write an int / size_t / char / float plus newline to the buffered debug log.  Will not be displayed until debug_flush() is called.
void debug_println(int i);
void debug_println(size_t l);
void debug_println(char ch);
void debug_println(float f);  //  Note: Always prints with 2 decimal places.  Will not be displayed until debug_flush() is called.
#endif  //  __cplusplus

#endif  //  DISABLE_DEBUG
#endif  //  LOGGER_H_
