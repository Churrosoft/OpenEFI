// TODO: morirme
#ifndef _ICAN_h
#define _ICAN_h
#include "defines.h"
#include "mcp2515.h"

class ICAN{
public:
    
    ICAN( MCP2515& MyMCP2 );
    void loop();
private:
    MCP2515& MyMCP;
};

#endif