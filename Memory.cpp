#include "Nes6502.h"
#include "Memory.h"
#include "Game.h"
#include "Bus.h"
#include "PRG.h"
#include "PPU.h"

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
    if (writeRegister(index, value)) { return; }

    // No register mapped, do normal write
    uint8_t* address = evalAddress(index);

    *address = value;

    logMemory("WRITE", value, index);
    bus.cpuCycle();
}

uint8_t Memory::read(uint16_t index)
{
    uint8_t value;
    if (readRegister(index, value)) { return value; }

    // No register mapped, do normal read
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

// Handle register mapping through memory
bool Memory::writeRegister(uint16_t address, uint8_t value)
{
    switch (address)
    {
    default: return false; // No write register for this address
    case PPUCTRL: bus.ppu().rPPUCTRL(value); break;
    case PPUMASK: bus.ppu().rPPUMASK(value); break;
    case OAMADDR: bus.ppu().rOAMADDR(value); break;
    case OAMDATA: bus.ppu().rOAMDATA_write(value); break;
    case PPUSCROLL: bus.ppu().rPPUSCROLL(value); break;
    case PPUADDR: bus.ppu().rPPUADDR(value); break;
    case PPUDATA: bus.ppu().rPPUDATA_write(value); break;
    case OAMDMA: bus.ppu().rOAMDMA(value); break;
    }

    return true;
}

bool Memory::readRegister(uint16_t address, uint8_t& value)
{
    switch (address)
    {
    default: return false;
    case PPUSTATUS: value = bus.ppu().rPPUSTATUS(); break;
    case OAMDATA: value = bus.ppu().rOAMDATA_read(); break;
    case PPUDATA: value = bus.ppu().rPPUDATA_read(); break;
    }

    return true;
}

// Evaluate real address based on NES memory mapping/mirroring
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