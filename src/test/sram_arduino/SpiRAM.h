#define SPIRAM_LIB_VERSION "0.1.4"
//    FILE: SpiRAM.h
//  AUTHOR: K. Tyler
// VERSION: 0.1.4
// PURPOSE: Arduino library for the 23A1024/23LC1024 SRAM memory chip 
//     URL: 
//
// HISTORY:
// see SpiRAM.cpp file

#ifndef SpiRAM_h
#define SpiRAM_h

// support for old IDE's
#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

#include <SPI.h>

// SRAM Instructions
#define RDMR  5
#define WRMR  1
#define WRITE 2
#define READ  3

// SRAM modes
#define BYTE_MODE       0x00
#define PAGE_MODE       0x80
#define SEQUENTIAL_MODE 0x40

class SpiRAM {
  public:
    // Initialize and specify the SS pin
    SpiRAM          (byte ss_Pin);
    
    // Change SS pin (used in multi SRAM chip)
    byte changeSS   (byte ss_Pin);
    
    // Read a single byte from address
    byte readByte   (uint32_t address);
    
    // Write a single byte to address
    void writeByte  (uint32_t address, byte data_byte);
    
    // Read several bytes starting at address and of length into a char buffer
    void readBuffer (uint32_t address, char *buffer, uint32_t length);
    
    // Write several bytes starting at address and of length from a char buffer
    void writeBuffer(uint32_t address, char *buffer, uint32_t length);
    
    // Write several bytes of value, starting at address and of length
    void fillBytes  (uint32_t address, byte value, uint32_t length);
    
  private:
    byte _ss_Pin;
    void setAddressMode(uint32_t address, byte mode);
};

#endif
// END OF FILE