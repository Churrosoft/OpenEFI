#ifndef STATUS_H
#define STATUS_H

typedef struct{
    char buffer[110];
    int RPM;
    int TEMP;
    int V00;
} Status;

#endif