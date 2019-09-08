#include "interfazCAN.h"

ICAN::ICAN( MCP2515& MyMCP2 ){
    MyMCP = MyMCP2;
    MyMCP.reset();
    MyMCP.setBitrate(CAN_125KBPS);
    MyMCP.setLoopbackMode();

}

void ICAN::loop(){
    MyMCP.reset();
}