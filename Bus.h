#pragma once
#include <stdint.h>
#include <string>

#include "Debug.h"

class Memory;
class Nes6502;
struct State6502;
class PPU;
class Game;
class Debug;

class Bus
{
public:
    Bus();
    ~Bus();

    void runNES();                  // Run the emulator, will emulate CPU instruction by instruction

    Memory& memory();
    Nes6502& cpu();
    State6502& cpuState();
    PPU& ppu();
    Game& game();

    void loadGame(std::string filename);

    void cpuCycle();                // Called on every memory read/write
    void finalizeCPUInstruction();  // Called once entire CPU instruction is done

    uint32_t cpuCycleCount();
    uint32_t ppuCycleCount();
    uint32_t busCycleCount();

    // Debug
    Debug& debug();
    uint32_t instructionCyclesCount();

private:
    Memory* nesmemory;
    Nes6502* nescpu;
    PPU* nesppu;
    Game* nesgame;

    uint32_t busCycles;            // Total number of bus clock cycles
    uint32_t instructionCycles;    // Number of cycles taken for the current instruction, used for debugging

    // Debug
    Debug* nesdebug;
};