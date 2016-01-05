#include "Bus.h"

#include "Memory.h"
#include "Nes6502.h"
#include "PPU.h"
#include "Game.h"
#include "Debug.h"

Bus::Bus() : busCycles(0), instructionCycles(0)
{
    nesmemory = new Memory(*this);
    nescpu = new Nes6502(*this);
    nesppu = new PPU(*this);
    nesgame = new Game(*this);
    nesdebug = new Debug(*this);
}


Bus::~Bus()
{
    delete nesmemory, nescpu, nesppu, nesgame, nesdebug;
}

void Bus::runNES()
{
    nesppu->init();
    nescpu->emulate6502();
}

Memory& Bus::memory()
{
    return *nesmemory;
}

Nes6502& Bus::cpu()
{
    return *nescpu;
}

State6502& Bus::cpuState()
{
    return nescpu->state;
}

PPU& Bus::ppu()
{
    return *nesppu;
}

Game& Bus::game()
{
    return *nesgame;
}

Debug& Bus::debug()
{
    return *nesdebug;
}

void Bus::loadGame(std::string filename)
{
    nesgame->loadGame(filename);
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