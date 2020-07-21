#ifndef STATUS_H
#define STATUS_H

typedef struct{
    char buffer[116];
    uint16_t RPM;
    uint16_t TEMP;
    uint16_t V00;
} Status;

#endif