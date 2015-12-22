#pragma once
#include "Config.h"
#include "OpCode.h"

#include <string>
#include <sstream>
#include <stdint.h>

#if defined(DEBUG)
#define flushAllDebug() bus.debug().flushAll()
#else
#define flushAllDebug()
#endif

#if defined(DEBUG) && LOG_OPCODE == 1
#define logOp(arg) bus.debug().logOpDebug(arg)
#else
#define logOp(arg) 
#endif

#if defined(DEBUG) && LOG_MEMORY == 1
#define logMemory(op,value,index) bus.debug().logMemoryDebug(op,value,index)
#else
#define logMemory(op,value,index) 
#endif

class Bus;

class Debug
{
public:
    Debug(Bus& bus);
    ~Debug();

    void logOpDebug(Argument arg);
    void logMemoryDebug(const char* op, uint8_t value, uint16_t index);

    void flushAll();

    std::string memoryDebug();
    std::string opDebug();

private:
    Bus& bus;

    std::stringstream memoryDebugStream;
    std::stringstream opDebugStream;

    std::string toHex(int val, int width);
};

