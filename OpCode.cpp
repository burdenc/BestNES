#include "OpCode.h"
#include "Nes6502.h"
#include "Bus.h"

#include <stdio.h>
#include <iomanip>
#include <sstream>
#include "SDL_log.h"

#define LOG_OPCODE 1

// TODO: Centralize this logging logic to single file
#if defined(DEBUG) && LOG_OPCODE == 1
#define logOp(function, state, arg) logOpDebug(function, state, arg)
#else
#define logOp(function, state, arg) 
#endif

static std::string toHex(int val, int width)
{
    std::stringstream hex;
    hex << std::setw(width) << std::setfill('0') << std::uppercase << std::hex << val;
    return hex.str();
}

void logOpDebug(char* function, State6502& state, Argument arg)
{
    std::stringstream debug;

    debug << toHex(state.PC - instructionSize[arg.addrMode], 4) << "  ";

    std::stringstream rawOpCode;
    for (int i = 0; i < instructionSize[arg.addrMode]; i++)
    {
        rawOpCode << toHex((int) arg.rawInstr[i], 2) << " ";
    }
    debug << std::setw(10) << std::left << rawOpCode.str();

    debug << std::right;

    std::stringstream addrStr;
    addrStr << (function + 1) << " ";
    switch (arg.addrMode)
    {
    case IMP:   // Implied and NOP have no args
    case NADR:
        break;
    case ACC:
        addrStr << "A";
        break;
    case REL:
        //addrStr << "$" << toHex(arg.rawInstr[1], 2) << ", PC";
        addrStr << "$" << toHex(arg.address, 4);
        break;
    case IMM:
        addrStr << "#$" << toHex(arg.value, 2);
        break;
    case ZP:
        addrStr << "$" << toHex(arg.rawInstr[1], 2);
        break;
    case ZPX:
        addrStr << "$" << toHex(arg.rawInstr[1], 2) << ",X";
        break;
    case ZPY:
        addrStr << "$" << toHex(arg.rawInstr[1], 2) << ",Y";
        break;
    case ABS:
        addrStr << "$" << toHex(arg.rawInstr[2], 2) << toHex(arg.rawInstr[1], 2);
        break;
    case ABSX:
        addrStr << "$" << toHex(arg.rawInstr[2], 2) << toHex(arg.rawInstr[1], 2) << ",X";
        break;
    case ABSY:
        addrStr << "$" << toHex(arg.rawInstr[2], 2) << toHex(arg.rawInstr[1], 2) << ",Y";
        break;
    case IND:
        addrStr << "$(" << toHex(arg.rawInstr[2], 2) << toHex(arg.rawInstr[1], 2) << ")";
        break;
    case INDX:
        addrStr << "$(" << toHex(arg.rawInstr[1], 2) << ",X)";
        break;
    case INDY:
        addrStr << "$(" << toHex(arg.rawInstr[1], 2) << "),Y";
        break;
    }
    debug << std::setw(31) << std::left << addrStr.str();

    debug << " A:" << toHex(state.A, 2)
          << " X:" << toHex(state.X, 2)
          << " Y:" << toHex(state.Y, 2)
          << " P:" << toHex(state.P.toByte(), 2)
          << " SP:" << toHex(state.SP, 2)
          << " CPUC:" << Bus::cycles;

    printf("%s\n", debug.str().c_str());
}

void setZandN(State6502& state, uint8_t result)
{
    if (result == 0) { state.P.Z = 1; }
    else { state.P.Z = 0; }

    // Check if negative bit set
    state.P.N = result >> 7;
}

void fADC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    uint16_t result = value + state.A + state.P.C;
    if (result > 0xFF) { state.P.C = 1; }
    else { state.P.C = 0; }     // TODO: unsure, Nintendulator says so

    int16_t s_result = (int8_t) value + (int8_t) state.A + state.P.C;

    state.A = 0xFF & result;
    if ((s_result < 0 && (int8_t) state.A > 0) || (s_result > 0 && (int8_t) state.A < 0)) { state.P.V = 1; }
    else { state.P.V = 0; }     // TODO: Unsure about this, Nintendulator log says to do this
    setZandN(state, state.A);
    
}

void fAND(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = state.A & input.getValue(state);

    setZandN(state, result);
    
    state.A = result;
}

void fASL(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (input.addrMode == ABSX) { state.memory.read(input.address); }
    uint8_t value = input.getValue(state);
    uint8_t result = value << 1;
    state.P.C = (value & 0x80) >> 7;

    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        state.memory.write(input.address, result);
        state.memory.write(input.address, result);
    }
}

void fBCC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.C == 0)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBCS(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.C == 1)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBEQ(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.Z == 1)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBIT(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    
    if ((state.A & value) == 0) { state.P.Z = 1; }
    else { state.P.Z = 0; }     // TODO: Not sure about this, Nintendulator log says to
    state.P.V = (value & 0x40) >> 6;
    state.P.N = (value & 0x80) >> 7;
}

void fBMI(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.N == 1)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBNE(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.Z == 0)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBPL(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.N == 0)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBRK(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.I == 1) { return; }

    // TODO: unsure if PC is -1 for interrupt like JSR
    state.memory.push(state.SP, (state.PC & 0xFF00) >> 8);
    state.memory.push(state.SP, state.PC & 0x00FF);
    state.memory.push(state.SP, state.P.toByte() | 0x30); // Set B and padding flags

    uint8_t msb = state.memory.read(0xFFFF);
    uint8_t lsb = state.memory.read(0xFFFE);

    state.PC = (msb << 8 | lsb);

    state.P.B = 1;
}

void fBVC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.V == 0)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBVS(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (state.P.V == 1)
    {
        state.memory.read(state.PC);
        state.PC = input.address;
    }
}

void fCLC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.C = 0;
}

void fCLD(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.D = 0;
}

void fCLI(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.I = 0;
}

void fCLV(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.V = 0;
}

// TODO: signed comparison?
void fCMP(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    uint8_t result = state.A - value;
    if (state.A >= value) { state.P.C = 1; }
    else { state.P.C = 0; } // TODO: Nintendulator says to do this
    setZandN(state, result);
}

void fCPX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    uint8_t result = state.X - value;
    if (state.X >= value) { state.P.C = 1; }
    else { state.P.C = 0; } // TODO: Nintendulator says to do this
    setZandN(state, result);
}

void fCPY(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    uint8_t result = state.Y - value;
    if (state.Y >= value) { state.P.C = 1; }
    else { state.P.C = 0; } // TODO: Nintendulator says to do this
    setZandN(state, result);
}

void fDEC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (input.addrMode == ABSX) { state.memory.read(input.address); }
    uint8_t result = input.getValue(state) - 1;

    setZandN(state, result);

    state.memory.write(input.address, result);
    state.memory.write(input.address, result);
}

void fDEX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = state.X - 1;

    setZandN(state, result);

    state.X = result;
}

void fDEY(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = state.Y - 1;

    setZandN(state, result);

    state.Y = result;
}

void fEOR(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = state.A ^ input.getValue(state);

    setZandN(state, result);

    state.A = result;
}

void fINC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (input.addrMode == ABSX) { state.memory.read(input.address); }
    uint8_t result = input.getValue(state) + 1;

    setZandN(state, result);

    state.memory.write(input.address, result);
    state.memory.write(input.address, result);
}

void fINX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = state.X + 1;

    setZandN(state, result);

    state.X = result;
}

void fINY(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = state.Y + 1;

    setZandN(state, result);

    state.Y = result;
}

void fJMP(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);

    state.PC = input.address;
}

void fJSR(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);

    state.memory.read(state.SP);
    state.memory.push(state.SP, ((state.PC - 1) & 0xFF00) >> 8);
    state.memory.push(state.SP, (state.PC - 1) & 0x00FF);
    state.PC = input.address;
}

void fLDA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = input.getValue(state);

    setZandN(state, result);

    state.A = result;
}

void fLDX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = input.getValue(state);

    setZandN(state, result);

    state.X = result;
}

void fLDY(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t result = input.getValue(state);

    setZandN(state, result);

    state.Y = result;
}

void fLSR(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (input.addrMode == ABSX) { state.memory.read(input.address); }
    uint8_t value = input.getValue(state);
    uint8_t result = value >> 1;

    state.P.C = value & 0x01;
    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        state.memory.write(input.address, result);
        state.memory.write(input.address, result);
    }
}

void fNOP(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
}

void fORA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    uint8_t result = state.A | value;

    setZandN(state, result);

    state.A = result;
}

void fPHA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.memory.push(state.SP, state.A);
}

void fPHP(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.memory.push(state.SP, state.P.toByte() | 0x30); // Set B and padding flag
}

void fPLA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.memory.read(state.SP & 0x00FF);
    state.A = state.memory.pop(state.SP);
    setZandN(state, state.A);
}

void fPLP(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.memory.read(state.SP & 0x00FF);
    state.P.set(state.memory.pop(state.SP));
}

void fROL(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (input.addrMode == ABSX) { state.memory.read(input.address); }
    uint8_t value = input.getValue(state);
    uint8_t result = (value << 1) | state.P.C;
    state.P.C = (value & 0x80) >> 7;

    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        state.memory.write(input.address, result);
        state.memory.write(input.address, result);
    }
}

void fROR(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    if (input.addrMode == ABSX) { state.memory.read(input.address); }
    uint8_t value = input.getValue(state);
    uint8_t result = (value >> 1) | (state.P.C << 7);
    state.P.C = value & 0x01;

    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        state.memory.write(input.address, result);
        state.memory.write(input.address, result);
    }
}

void fRTI(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);

    state.memory.read(state.SP);
    state.P.set(state.memory.pop(state.SP));

    uint8_t pcLSB = state.memory.pop(state.SP);
    uint8_t pcMSB = state.memory.pop(state.SP);
    state.PC = (pcMSB << 8 | pcLSB);
}

void fRTS(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t pcLSB = state.memory.pop(state.SP);
    uint8_t pcMSB = state.memory.pop(state.SP);

    state.PC = (pcMSB << 8 | pcLSB);
    state.memory.read(state.PC);
    state.PC += 1;

    Bus::nextCycles++;
}

void fSBC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    uint8_t value = input.getValue(state);
    uint16_t result = state.A - value - (1 - state.P.C);
    if (result > 0xFF) { state.P.C = 0; }
    else { state.P.C = 1; } // TODO: check, Nintendulator

    int16_t s_result = (int8_t) state.A - (int8_t) value - (1 - state.P.C);

    state.A = 0xFF & result;
    if ((s_result < 0 && (int8_t)state.A > 0) || (s_result > 0 && (int8_t)state.A < 0)) { state.P.V = 1; }
    else { state.P.V = 0; }     // TODO: Unsure about this, Nintendulator log says to do this
    setZandN(state, state.A);
}

void fSEC(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.C = 1;
}

void fSED(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.D = 1;
}

void fSEI(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.P.I = 1;
}

void fSTA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);

    input.pageWrap = false; // "oops" cycle doesn't happen with STA, STX, STY
    if (input.addrMode == ABSX || input.addrMode == ABSY || input.addrMode == INDY)
    {
        state.memory.read(input.address);
    }
    state.memory.write(input.address, state.A);
}

void fSTX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);

    input.pageWrap = false; // "oops" cycle doesn't happen with STA, STX, STY
    if (input.addrMode == ABSX || input.addrMode == ABSY || input.addrMode == INDY)
    {
        state.memory.read(input.address);
    }
    state.memory.write(input.address, state.X);
}

void fSTY(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);

    input.pageWrap = false; // "oops" cycle doesn't happen with STA, STX, STY
    if (input.addrMode == ABSX || input.addrMode == ABSY || input.addrMode == INDY)
    {
        state.memory.read(input.address);
    }
    state.memory.write(input.address, state.Y);
}

void fTAX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.X = state.A;
    setZandN(state, state.X);
}

void fTAY(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.Y = state.A;
    setZandN(state, state.Y);
}

void fTSX(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.X = state.SP;
    setZandN(state, state.X);
}

void fTXA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.A = state.X;
    setZandN(state, state.A);
}

void fTXS(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.SP = state.X;
}

void fTYA(State6502& state, Argument input)
{
    logOp(__FUNCTION__, state, input);
    state.A = state.Y;
    setZandN(state, state.A);
}

InstrFunc instructionFunction[] = {   // Instruction function lookup
//    0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F     
/*0*/ fBRK, fORA, fNOP, fNOP, fNOP, fORA, fASL, fNOP, fPHP, fORA, fASL, fNOP, fNOP, fORA, fASL, fNOP,
/*1*/ fBPL, fORA, fNOP, fNOP, fNOP, fORA, fASL, fNOP, fCLC, fORA, fNOP, fNOP, fNOP, fORA, fASL, fNOP,
/*2*/ fJSR, fAND, fNOP, fNOP, fBIT, fAND, fROL, fNOP, fPLP, fAND, fROL, fNOP, fBIT, fAND, fROL, fNOP,
/*3*/ fBMI, fAND, fNOP, fNOP, fNOP, fAND, fROL, fNOP, fSEC, fAND, fNOP, fNOP, fNOP, fAND, fROL, fNOP,
/*4*/ fRTI, fEOR, fNOP, fNOP, fNOP, fEOR, fLSR, fNOP, fPHA, fEOR, fLSR, fNOP, fJMP, fEOR, fLSR, fNOP,
/*5*/ fBVC, fEOR, fNOP, fNOP, fNOP, fEOR, fLSR, fNOP, fCLI, fEOR, fNOP, fNOP, fNOP, fEOR, fLSR, fNOP,
/*6*/ fRTS, fADC, fNOP, fNOP, fNOP, fADC, fROR, fNOP, fPLA, fADC, fROR, fNOP, fJMP, fADC, fROR, fNOP,
/*7*/ fBVS, fADC, fNOP, fNOP, fNOP, fADC, fROR, fNOP, fSEI, fADC, fNOP, fNOP, fNOP, fADC, fROR, fNOP,
/*8*/ fNOP, fSTA, fNOP, fNOP, fSTY, fSTA, fSTX, fNOP, fDEY, fNOP, fTXA, fNOP, fSTY, fSTA, fSTX, fNOP,
/*9*/ fBCC, fSTA, fNOP, fNOP, fSTY, fSTA, fSTX, fNOP, fTYA, fSTA, fTXS, fNOP, fNOP, fSTA, fNOP, fNOP,
/*A*/ fLDY, fLDA, fLDX, fNOP, fLDY, fLDA, fLDX, fNOP, fTAY, fLDA, fTAX, fNOP, fLDY, fLDA, fLDX, fNOP,
/*B*/ fBCS, fLDA, fNOP, fNOP, fLDY, fLDA, fLDX, fNOP, fCLV, fLDA, fTSX, fNOP, fLDY, fLDA, fLDX, fNOP,
/*C*/ fCPY, fCMP, fNOP, fNOP, fCPY, fCMP, fDEC, fNOP, fINY, fCMP, fDEX, fNOP, fCPY, fCMP, fDEC, fNOP,
/*D*/ fBNE, fCMP, fNOP, fNOP, fNOP, fCMP, fDEC, fNOP, fCLD, fCMP, fNOP, fNOP, fNOP, fCMP, fDEC, fNOP,
/*E*/ fCPX, fSBC, fNOP, fNOP, fCPX, fSBC, fINC, fNOP, fINX, fSBC, fNOP, fNOP, fCPX, fSBC, fINC, fNOP,
/*F*/ fBEQ, fSBC, fNOP, fNOP, fNOP, fSBC, fINC, fNOP, fSED, fSBC, fNOP, fNOP, fNOP, fSBC, fINC, fNOP,
};

InstrFunc getInstrFunc(uint8_t opcode)
{
    return instructionFunction[opcode];
}