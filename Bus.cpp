#include "Bus.h"

Bus::Bus() : busCycles(0), instructionCycles(0), nesmemory(*this), nescpu(*this), nesdebug(*this)
{
}


Bus::~Bus()
{
}

void Bus::runNES()
{
    nescpu.emulate6502();
}

Memory& Bus::memory()
{
    return nesmemory;
}

Nes6502& Bus::cpu()
{
    return nescpu;
}

State6502& Bus::cpuState()
{
    return nescpu.state;
}

Debug& Bus::debug()
{
    return nesdebug;
}

void Bus::cpuCycle()
{
    busCycles += 12;
    // TODO: cycle PPU 3 times
}

void Bus::finalizeCPUInstruction()
{
    busCycles += instructionCycles;
    instructionCycles = 0;
}

uint32_t Bus::cpuCycleCount()
{
    return busCycles / 12;
}

uint32_t Bus::ppuCycleCount()
{
    return busCycles / 4;
}

uint32_t Bus::busCycleCount()
{
    return busCycles;
}