#include "Nes6502.h"
#include "OpCode.h"
#include "Bus.h"

#include <iostream>
#include <string>

void printTestOutput(Memory& mem)
{
    uint8_t c = 0;
    uint16_t ptr = 0x6004;
    do
    {
        c = mem.read(ptr);
        ptr++;
        std::cout << c;
    } while (c != 0);
}

Nes6502::Nes6502()
{
    state.SP = 0xFD;
    state.A = 0;
    state.X = 0;
    state.Y = 0;
}


Nes6502::~Nes6502()
{
}

void Nes6502::emulate6502()
{
    /*uint8_t startMSB = state.memory.read(0xFFFD);
    uint8_t startLSB = state.memory.read(0xFFFC);

    state.PC = (startMSB << 8 | startLSB);*/
    state.PC = 0xC000;

    while (1)
    {
        Bus::nextCycles = 0;

        uint8_t instr[3] = {};
        instr[0] = state.memory.read(state.PC);
        instr[1] = state.memory.read(state.PC + 1); // Second instruction byte always read

        for (int i = 2; i < instructionSize[addressingMode[instr[0]]]; i++)
        {
            instr[i] = state.memory.read(state.PC + i);
        }

        executeOpCode(instr);
        Bus::cycles += Bus::nextCycles;

        // DEBUG OUTPUT
        // TODO: Centralize to Logging file
        std::cout << state.memory.getDebugStream();
    }
}

// TODO: Does PC always increment? Will it increment even if doing a JMP?
void Nes6502::executeOpCode(uint8_t instr[])
{
    state.PC += instructionSize[addressingMode[instr[0]]];
    Argument arg = obtainArgument(instr);
    getInstrFunc(instr[0])(state, arg);
}

Argument Nes6502::obtainArgument(uint8_t instr[])
{
    uint16_t address = 0;
    Argument arg = {};
    arg.addrMode = addressingMode[instr[0]];
    arg.rawInstr[0] = instr[0]; arg.rawInstr[1] = instr[1]; arg.rawInstr[2] = instr[2];

    switch (arg.addrMode)
    {
    case IMP:   // Implied and NOP have no args
    case NADR:
        return arg;
    case ACC:
        arg.address = 0xCDCD;
        arg.value = state.A;
        return arg;
    case REL:
        arg.address = state.PC + (int8_t) instr[1];
        arg.value = 0xCD;
        return arg;
    case IMM:
        arg.address = 0xCDCD;
        arg.value = instr[1];
        return arg;
    case ZP:
        arg.address = instr[1];
        break;
    case ZPX:
        state.memory.read(instr[1]);
        arg.address = (instr[1] + state.X) & 0xFF;
        break;
    case ZPY:
        state.memory.read(instr[1]);
        arg.address = (instr[1] + state.Y) & 0xFF;
        break;
    case ABS:
        arg.address = (uint16_t) instr[2] << 8 | instr[1];
        break;
    case ABSX:
        arg.address = ((uint16_t) instr[2] << 8 | instr[1]);
        pageWrap(arg, state.X);
        arg.address += state.X;
        break;
    case ABSY:
        arg.address = ((uint16_t) instr[2] << 8 | instr[1]);
        pageWrap(arg, state.Y);
        arg.address += state.Y;
        break;
    case IND:
        arg.address = (uint16_t) instr[2] << 8 | instr[1];
        arg.address = state.memory.read(arg.address & 0xFF00 | (arg.address + 1) & 0xFF) << 8 |
                      state.memory.read(arg.address);
        break;
    case INDX:
        state.memory.read(instr[1]);
        arg.address = (uint16_t) state.memory.read((instr[1] + state.X + 1) % 256) << 8 |
                                 state.memory.read((instr[1] + state.X)     % 256);
        break;
    case INDY:
        arg.address = (uint16_t) state.memory.read((instr[1] + 1) % 256) << 8 |
                                 state.memory.read(instr[1]);
        pageWrap(arg, state.Y);
        arg.address += state.Y;
        break;
    }

    return arg;
}

// Used to calculate wrong address for "oops" cycle that happens with ABSX, ABSY, INDY
void Nes6502::pageWrap(Argument& arg, uint8_t offset)
{
    uint16_t lowerByte = (uint16_t) (arg.address & 0xFF) + offset;
    if (lowerByte > 0xFF)   // Carry occured, "oops" cycle
    {
        arg.pageWrap = true;
        arg.oopsAddress = (arg.address & 0xFF00) | (lowerByte & 0x00FF);
    }
}