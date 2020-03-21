#ifndef CRC_H
#define CRC_H
int CRC(const char *data, size_t dataLength);

int CRC(const char *data, size_t dataLength){
//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license

int crc =0x00;
  for (size_t tempI2 = 0; tempI2 < dataLength ; tempI2++){
    int extract = *data++;
    for (int tempI = 8; tempI; tempI--){
      int sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) { crc ^= 0x9C; } //original 0x8C
      extract >>= 1;
    }
  }
  return crc;
}

#endif