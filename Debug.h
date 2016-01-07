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

#if defined(DEBUG) && LOG_GAME == 1
#define logGame(format, ...) printf(format, __VA_ARGS__)
#else
#define logOp(format, ...)
#endif

#if defined(DEBUG) && LOG_PPU == 1
#define logPPU(str) bus.debug().logPPUDebug(str)
#else
#define logPPU(str)
#endif

#if defined(DEBUG) && LOG_MMIO == 1
#define logMemoryReg(op,reg,value) bus.debug().logMemoryRegDebug(op,reg,value)
#else
#define logMemoryReg(op,reg,value) 
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
    void logPPUDebug(const char* str);
    void logMemoryDebug(const char* op, uint8_t value, uint16_t index);
    void logMemoryRegDebug(const char* op, const char* reg, uint8_t value);

    void flushAll();

    std::string memoryDebug();
    std::string opDebug();

private:
    Bus& bus;

    std::stringstream memoryDebugStream;
    std::stringstream opDebugStream;

    std::string toHex(int val, int width);
};

