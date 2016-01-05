#include "Nes6502.h"
#include "Memory.h"
#include "Game.h"
#include "Bus.h"
#include "PRG.h"

Memory::Memory(Bus& bus) : bus(bus), memory(new uint8_t[MEMORY_SIZE])
{
}


Memory::~Memory()
{
    delete [] memory;
}

// TODO: one cycle for each read or write
// TODO: read initial page on ABSX, ABSY, INDY ops
//       ex: 0x00FF,X (X = 1) ===> READ 0x0000 (thrown away), READ 0x0100
void Memory::write(uint16_t index, uint8_t value)
{
    logMemory("WRITE", value, index);
    uint8_t* address = evalAddress(index);

    *address = value;

    bus.cpuCycle();
}

uint8_t Memory::read(uint16_t index)
{
    uint8_t* address = evalAddress(index);

    logMemory("READ", *address, index);

    bus.cpuCycle();
    return *address;
}

void Memory::push(uint8_t value)
{
    uint8_t& stackPtr = bus.cpuState().SP;
    logMemory("WRITE", value, STACK_OFFSET + stackPtr);
    memory[STACK_OFFSET + stackPtr] = value;
    stackPtr--;

    bus.cpuCycle();
}

uint8_t Memory::pop()
{
    uint8_t& stackPtr = bus.cpuState().SP;
    uint8_t result = memory[STACK_OFFSET + stackPtr + 1];
    stackPtr++;

    logMemory("READ", result, STACK_OFFSET + stackPtr + 1);

    bus.cpuCycle();
    return result;
}

// Evaluate real address based on NES memory mapping
uint8_t* Memory::evalAddress(uint16_t address)
{
    // PRG ROM Redirect
    if (0x8000 <= address && address <= 0xFFFF)
    {
        return bus.game().getPRG().getPtr(address - 0x8000);
    }

    // RAM Mirroring 0x000 - 0x7FF to 0x800, 0x1000, 0x1800
    if (0x00 <= address && address < 0x1800)
    {
        return memory + (address % 0x800);
    }

    // RAM Mirroring 0x2000 - 0x2007 to 0x2008, ..., 0x3FF8
    if (0x2000 <= address && address < 0x4000)
    {
        return memory + (address % 0x08);
    }

    return memory + address;
}