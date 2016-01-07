#pragma once
#include <stdint.h>

#include <string>
#include <sstream>

class Bus;

const size_t MEMORY_SIZE = 0xFFFF;
const size_t STACK_OFFSET = 0x0100;

enum MemReg
{
    // PPU
    PPUCTRL     = 0x2000,
    PPUMASK     = 0x2001,
    PPUSTATUS   = 0x2002,
    OAMADDR     = 0x2003,
    OAMDATA     = 0x2004,
    PPUSCROLL   = 0x2005,
    PPUADDR     = 0x2006,
    PPUDATA     = 0x2007,
    OAMDMA      = 0x4014,
};

class Memory
{
public:
    Memory(Bus& bus);
    ~Memory();

    void write(uint16_t index, uint8_t value);
    uint8_t read(uint16_t index);

    void push(uint8_t value);
    uint8_t pop();

private:
    Bus& bus;
    uint8_t* memory;

    uint8_t* evalAddress(uint16_t index);
    bool writeRegister(uint16_t index, uint8_t value);
    bool readRegister(uint16_t index, uint8_t& value);
};

