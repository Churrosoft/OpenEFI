// Tested uning a 23LC1024 sRam chip in a 8 pin DIP package
// Test connections are...
// Chip UNO MEGA  NAME
//  1   9   9     SS    (Hardware SS Pin (10 or 53) needs to remain output no matter what other pin you may for SS)
//  2   12  50    MISO
//  3   NC  NC
//  4   GND GND   Vss
//  5   11  51    MOSI
//  6   13  52    SCK
//  7   +5V +5V   ~HOLD
//  8   +5V +5V   Vcc


#include <SPI.h>
#include <SpiRAM.h>

const uint32_t ramSize = 0x1FFFF;           // 128K x 8 bit
const byte LED = 13;

char buffer[] = {"ABCDEFGHIJ"};
char buffer2[sizeof(buffer)];

SpiRAM sRam(9);                             // Initialize the RAM with non standard SS

void setup(){
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);                    // Turn off LED (Mega LED stays on else)
  Serial.begin(115200);
  Serial.println("Ram Tests Begin.");
  
  //SPI.setClockDivider(SPI_CLOCK_DIV8);
  
  Serial.println("\r\nFill Memory with 0xFF.");
  sRam.fillBytes(0,0xFF,ramSize);
  dumpsRam(0,100);
  dumpsRam(ramSize - 100, 100);

  Serial.println("\r\nFill Memory with 0xAA.");
  sRam.fillBytes(0,0xAA,ramSize);
  dumpsRam(0,100);
  dumpsRam(ramSize - 100, 100);

  Serial.println("\r\nFill Memory with Buffer.");
  for (uint32_t x = 0; x < ramSize - (sizeof(buffer) - 1); x += sizeof(buffer) - 1){
    sRam.writeBuffer(x, buffer, sizeof(buffer) - 1);
  }
  dumpsRam(0,100);
  dumpsRam(ramSize - 100, 100);

  Serial.println("\r\nRead Buffer.");
  sRam.readBuffer(0,buffer2,sizeof(buffer2) - 1);
  Serial.println(buffer2);
  sRam.readBuffer(5,buffer2,sizeof(buffer2) - 1);
  Serial.println(buffer2);

  Serial.println("\r\nWrite byte.");
  for (uint32_t x = 0; x <= ramSize; x++){
    if((x % 1024) == 0) digitalWrite(LED,!digitalRead(LED));
    sRam.writeByte(x, (byte) x / 10);
  }
  dumpsRam(0,100);
  dumpsRam(ramSize - 100, 100);

  Serial.println("\r\nRam Tests Finished.");
}

void loop(){
}

void dumpsRam(uint32_t addr, uint32_t length)
{
  // block to 10
  addr = addr / 10 * 10;
  length = (length + 9)/10 * 10;

  byte b = sRam.readByte(addr);
  for (int i = 0; i < length; i++)
  {
    if (addr % 10 == 0)
    {
      Serial.println();
      Serial.print(addr, HEX);
      Serial.print(":\t");
    }
    Serial.print(b, HEX);
    b = sRam.readByte(++addr);
    Serial.print("\t");
  }
  Serial.println();
}

void dumpsRam2(uint32_t addr, uint32_t length)
{
  // block to 10
  addr = addr / 10 * 10;
  length = (length + 9)/10 * 10;

  char b = sRam.readByte(addr);
  for (int i = 0; i < length; i++)
  {
    if (addr % 10 == 0)
    {
      Serial.println();
      Serial.print(addr);
      Serial.print(":\t");
    }
    Serial.print(b);
    b = sRam.readByte(++addr);
    Serial.print("\t");
  }
  Serial.println();
}
