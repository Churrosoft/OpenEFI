//    FILE: SpiRAM.cpp
//  AUTHOR: K. Tyler
// VERSION: 0.1.4
// PURPOSE: Arduino library for the 23A1024/23LC1024 SRAM memory chip
//
// HISTORY:
// 0.1.00 - 2013-12-15  Initial version.
// 0.1.01 - 2013-12-15  Refactored by Rob Tillaart.
// 0.1.02 - 2013-12-17  Changed readBytes & writeBytes name to readBuffer & writeBuffer respectfully.
//                      Added SPI_CLOCK_DIV8 to improve readBuffer reliability.
// 0.1.03 - 2013-12-20  Removed SPI_CLOCK_DIV8 added in version 0.1.02 as user needs to decide if clock needs decreasing.
//                      Added code in SpiRAM to setup SS pin as output for use of non standard SS pins.
// 0.1.4  - 2013-12-28  Changed readBuffer, writeBuffer & fillBytes length from uint16_t to uint32_t.
//                      Added changeSS function to allow multiple SRAM chips.
//
// Released to the public domain
//

#include <SpiRAM.h>

SpiRAM::SpiRAM(byte ss_Pin){
  _ss_Pin = ss_Pin;                     // Store Slave Select pin
  digitalWrite(_ss_Pin,HIGH);           // Set SS pin high
  pinMode(_ss_Pin,OUTPUT);              // Set it to output
  SPI.begin();                          // Start SPI
}

byte SpiRAM::changeSS(byte ss_Pin){
  byte temp = _ss_Pin;                  // Get current SS pin
  _ss_Pin = ss_Pin;                     // Store Slave Select pin
  digitalWrite(_ss_Pin,HIGH);           // Set SS pin high
  pinMode(_ss_Pin,OUTPUT);              // Set it to output
  return temp;                          // Return previous SS pin
}

byte SpiRAM::readByte(uint32_t address) {
  byte data_byte;
  readBuffer(address, (char*) &data_byte, 1);
  return data_byte;
}

void SpiRAM::writeByte(uint32_t address, byte data_byte) {
  writeBuffer(address, (char*) &data_byte, 1);
}

void SpiRAM::readBuffer(uint32_t address, char *buffer, uint32_t length) {
  digitalWrite(_ss_Pin, LOW);
  setAddressMode(address, READ);
  for (uint32_t i = 0; i < length; i++) buffer[i] = SPI.transfer(0x00);
  digitalWrite(_ss_Pin, HIGH);
}

void SpiRAM::writeBuffer(uint32_t address, char *buffer, uint32_t length) {
  digitalWrite(_ss_Pin, LOW);
  setAddressMode(address, WRITE);
  for (uint32_t i = 0; i < length; i++) SPI.transfer(buffer[i]);
  digitalWrite(_ss_Pin, HIGH);
}

void SpiRAM::fillBytes(uint32_t address, byte value, uint32_t length) {
  digitalWrite(_ss_Pin, LOW);
  setAddressMode(address, WRITE);
  for (uint32_t i = 0; i < length; i++) SPI.transfer(value);
  digitalWrite(_ss_Pin, HIGH);
}

void SpiRAM::setAddressMode(uint32_t address, byte mode) {
  SPI.transfer(mode);
  SPI.transfer((byte)(address >> 16));
  SPI.transfer((byte)(address >> 8));
  SPI.transfer((byte)address);
}

// END OF FILE