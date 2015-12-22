#include "OpCode.h"
#include "Nes6502.h"
#include "Bus.h"

void setZandN(State6502& state, uint8_t result)
{
    if (result == 0) { state.P.Z = 1; }
    else { state.P.Z = 0; }

    // Check if negative bit set
    state.P.N = result >> 7;
}

void fADC(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    uint16_t result = value + state.A + state.P.C;
    if (result > 0xFF) { state.P.C = 1; }
    else { state.P.C = 0; }     // TODO: unsure, Nintendulator says so

    int16_t s_result = (int8_t) value + (int8_t) state.A + state.P.C;

    state.A = 0xFF & result;
    if ((s_result < 0 && (int8_t) state.A > 0) || (s_result > 0 && (int8_t) state.A < 0)) { state.P.V = 1; }
    else { state.P.V = 0; }     // TODO: Unsure about this, Nintendulator log says to do this
    setZandN(state, state.A);
    
}

void fAND(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = state.A & input.getValue(memory);

    setZandN(state, result);
    
    state.A = result;
}

void fASL(State6502& state, Memory& memory, Argument input)
{
    if (input.addrMode == ABSX) { memory.read(input.address); }
    uint8_t value = input.getValue(memory);
    uint8_t result = value << 1;
    state.P.C = (value & 0x80) >> 7;

    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        memory.write(input.address, result);
        memory.write(input.address, result);
    }
}

void fBCC(State6502& state, Memory& memory, Argument input)
{
    if (state.P.C == 0)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBCS(State6502& state, Memory& memory, Argument input)
{
    if (state.P.C == 1)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBEQ(State6502& state, Memory& memory, Argument input)
{
    if (state.P.Z == 1)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBIT(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    
    if ((state.A & value) == 0) { state.P.Z = 1; }
    else { state.P.Z = 0; }     // TODO: Not sure about this, Nintendulator log says to
    state.P.V = (value & 0x40) >> 6;
    state.P.N = (value & 0x80) >> 7;
}

void fBMI(State6502& state, Memory& memory, Argument input)
{
    if (state.P.N == 1)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBNE(State6502& state, Memory& memory, Argument input)
{
    if (state.P.Z == 0)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBPL(State6502& state, Memory& memory, Argument input)
{
    if (state.P.N == 0)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBRK(State6502& state, Memory& memory, Argument input)
{
    if (state.P.I == 1) { return; }

    // TODO: unsure if PC is -1 for interrupt like JSR
    memory.push((state.PC & 0xFF00) >> 8);
    memory.push(state.PC & 0x00FF);
    memory.push(state.P.toByte() | 0x30); // Set B and padding flags

    uint8_t msb = memory.read(0xFFFF);
    uint8_t lsb = memory.read(0xFFFE);

    state.PC = (msb << 8 | lsb);

    state.P.B = 1;
}

void fBVC(State6502& state, Memory& memory, Argument input)
{
    if (state.P.V == 0)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fBVS(State6502& state, Memory& memory, Argument input)
{
    if (state.P.V == 1)
    {
        memory.read(state.PC);
        state.PC = input.address;
    }
}

void fCLC(State6502& state, Memory& memory, Argument input)
{
    state.P.C = 0;
}

void fCLD(State6502& state, Memory& memory, Argument input)
{
    state.P.D = 0;
}

void fCLI(State6502& state, Memory& memory, Argument input)
{
    state.P.I = 0;
}

void fCLV(State6502& state, Memory& memory, Argument input)
{
    state.P.V = 0;
}

// TODO: signed comparison?
void fCMP(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    uint8_t result = state.A - value;
    if (state.A >= value) { state.P.C = 1; }
    else { state.P.C = 0; } // TODO: Nintendulator says to do this
    setZandN(state, result);
}

void fCPX(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    uint8_t result = state.X - value;
    if (state.X >= value) { state.P.C = 1; }
    else { state.P.C = 0; } // TODO: Nintendulator says to do this
    setZandN(state, result);
}

void fCPY(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    uint8_t result = state.Y - value;
    if (state.Y >= value) { state.P.C = 1; }
    else { state.P.C = 0; } // TODO: Nintendulator says to do this
    setZandN(state, result);
}

void fDEC(State6502& state, Memory& memory, Argument input)
{
    if (input.addrMode == ABSX) { memory.read(input.address); }
    uint8_t result = input.getValue(memory) - 1;

    setZandN(state, result);

    memory.write(input.address, result);
    memory.write(input.address, result);
}

void fDEX(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = state.X - 1;

    setZandN(state, result);

    state.X = result;
}

void fDEY(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = state.Y - 1;

    setZandN(state, result);

    state.Y = result;
}

void fEOR(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = state.A ^ input.getValue(memory);

    setZandN(state, result);

    state.A = result;
}

void fINC(State6502& state, Memory& memory, Argument input)
{
    if (input.addrMode == ABSX) { memory.read(input.address); }
    uint8_t result = input.getValue(memory) + 1;

    setZandN(state, result);

    memory.write(input.address, result);
    memory.write(input.address, result);
}

void fINX(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = state.X + 1;

    setZandN(state, result);

    state.X = result;
}

void fINY(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = state.Y + 1;

    setZandN(state, result);

    state.Y = result;
}

void fJMP(State6502& state, Memory& memory, Argument input)
{
    state.PC = input.address;
}

void fJSR(State6502& state, Memory& memory, Argument input)
{
    memory.read(state.SP);
    memory.push(((state.PC - 1) & 0xFF00) >> 8);
    memory.push((state.PC - 1) & 0x00FF);
    state.PC = input.address;
}

void fLDA(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = input.getValue(memory);

    setZandN(state, result);

    state.A = result;
}

void fLDX(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = input.getValue(memory);

    setZandN(state, result);

    state.X = result;
}

void fLDY(State6502& state, Memory& memory, Argument input)
{
    uint8_t result = input.getValue(memory);

    setZandN(state, result);

    state.Y = result;
}

void fLSR(State6502& state, Memory& memory, Argument input)
{
    if (input.addrMode == ABSX) { memory.read(input.address); }
    uint8_t value = input.getValue(memory);
    uint8_t result = value >> 1;

    state.P.C = value & 0x01;
    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        memory.write(input.address, result);
        memory.write(input.address, result);
    }
}

void fNOP(State6502& state, Memory& memory, Argument input)
{
}

void fORA(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    uint8_t result = state.A | value;

    setZandN(state, result);

    state.A = result;
}

void fPHA(State6502& state, Memory& memory, Argument input)
{
    memory.push(state.A);
}

void fPHP(State6502& state, Memory& memory, Argument input)
{
    memory.push(state.P.toByte() | 0x30); // Set B and padding flag
}

void fPLA(State6502& state, Memory& memory, Argument input)
{
    memory.read(state.SP & 0x00FF);
    state.A = memory.pop();
    setZandN(state, state.A);
}

void fPLP(State6502& state, Memory& memory, Argument input)
{
    memory.read(state.SP & 0x00FF);
    state.P.set(memory.pop());
}

void fROL(State6502& state, Memory& memory, Argument input)
{
    if (input.addrMode == ABSX) { memory.read(input.address); }
    uint8_t value = input.getValue(memory);
    uint8_t result = (value << 1) | state.P.C;
    state.P.C = (value & 0x80) >> 7;

    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        memory.write(input.address, result);
        memory.write(input.address, result);
    }
}

void fROR(State6502& state, Memory& memory, Argument input)
{
    if (input.addrMode == ABSX) { memory.read(input.address); }
    uint8_t value = input.getValue(memory);
    uint8_t result = (value >> 1) | (state.P.C << 7);
    state.P.C = value & 0x01;

    setZandN(state, result);

    if (input.addrMode == ACC) { state.A = result; }
    else
    {
        memory.write(input.address, result);
        memory.write(input.address, result);
    }
}

void fRTI(State6502& state, Memory& memory, Argument input)
{
    memory.read(state.SP);
    state.P.set(memory.pop());

    uint8_t pcLSB = memory.pop();
    uint8_t pcMSB = memory.pop();
    state.PC = (pcMSB << 8 | pcLSB);
}

void fRTS(State6502& state, Memory& memory, Argument input)
{
    uint8_t pcLSB = memory.pop();
    uint8_t pcMSB = memory.pop();

    state.PC = (pcMSB << 8 | pcLSB);
    memory.read(state.PC);
    state.PC += 1;

    // TODO: determine actual read address
    memory.read(state.PC);
    //Bus::nextCycles++;
}

void fSBC(State6502& state, Memory& memory, Argument input)
{
    uint8_t value = input.getValue(memory);
    uint16_t result = state.A - value - (1 - state.P.C);
    if (result > 0xFF) { state.P.C = 0; }
    else { state.P.C = 1; } // TODO: check, Nintendulator

    int16_t s_result = (int8_t) state.A - (int8_t) value - (1 - state.P.C);

    state.A = 0xFF & result;
    if ((s_result < 0 && (int8_t)state.A > 0) || (s_result > 0 && (int8_t)state.A < 0)) { state.P.V = 1; }
    else { state.P.V = 0; }     // TODO: Unsure about this, Nintendulator log says to do this
    setZandN(state, state.A);
}

void fSEC(State6502& state, Memory& memory, Argument input)
{
    state.P.C = 1;
}

void fSED(State6502& state, Memory& memory, Argument input)
{
    state.P.D = 1;
}

void fSEI(State6502& state, Memory& memory, Argument input)
{
    state.P.I = 1;
}

void fSTA(State6502& state, Memory& memory, Argument input)
{
    input.pageWrap = false; // "oops" cycle doesn't happen with STA, STX, STY
    if (input.addrMode == ABSX || input.addrMode == ABSY || input.addrMode == INDY)
    {
        memory.read(input.address);
    }
    memory.write(input.address, state.A);
}

void fSTX(State6502& state, Memory& memory, Argument input)
{
    input.pageWrap = false; // "oops" cycle doesn't happen with STA, STX, STY
    if (input.addrMode == ABSX || input.addrMode == ABSY || input.addrMode == INDY)
    {
        memory.read(input.address);
    }
    memory.write(input.address, state.X);
}

void fSTY(State6502& state, Memory& memory, Argument input)
{
    input.pageWrap = false; // "oops" cycle doesn't happen with STA, STX, STY
    if (input.addrMode == ABSX || input.addrMode == ABSY || input.addrMode == INDY)
    {
        memory.read(input.address);
    }
    memory.write(input.address, state.Y);
}

void fTAX(State6502& state, Memory& memory, Argument input)
{
    state.X = state.A;
    setZandN(state, state.X);
}

void fTAY(State6502& state, Memory& memory, Argument input)
{
    state.Y = state.A;
    setZandN(state, state.Y);
}

void fTSX(State6502& state, Memory& memory, Argument input)
{
    state.X = state.SP;
    setZandN(state, state.X);
}

void fTXA(State6502& state, Memory& memory, Argument input)
{
    state.A = state.X;
    setZandN(state, state.A);
}

void fTXS(State6502& state, Memory& memory, Argument input)
{
    state.SP = state.X;
}

void fTYA(State6502& state, Memory& memory, Argument input)
{
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