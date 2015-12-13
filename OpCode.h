#pragma once
#include "Nes6502.h"

#include <stdint.h>

enum AddressingMode
{
    NADR,   // Null, No Such Addressing Mode
    ACC,    // Accumulator
    REL,    // Relative
    IMP,    // Implied
    IMM,    // Immediate
    ZP,     // Zero-page
    ZPX,    // Zero-page, offset X
    ZPY,    // Zero-page, offset Y
    ABS,    // Absolute
    ABSX,   // Absolute, offset X
    ABSY,   // Absolute, offset Y
    IND,    // (Indirect)
    INDX,   // (Indirect, offset X)
    INDY    // (Indirect), offset Y
};

struct Argument
{
    uint8_t value;
    AddressingMode addrMode;
    uint16_t address;
    uint8_t rawInstr[3];

    uint16_t oopsAddress; // Used for oops page wrap
    bool pageWrap;

    inline uint8_t getValue(State6502& state)
    {
        if (addrMode == IMM || addrMode == ACC)
        {
            return value;
        }

        // "oops" cycle logic, waste cycle if page boundary crossed
        if (pageWrap) { state.memory.read(oopsAddress); }

        return state.memory.read(address);
    }
};

typedef void(*InstrFunc)(State6502&, Argument);

InstrFunc getInstrFunc(uint8_t opcode);

static uint8_t instructionSize[] = {   // Instruction Size lookup based on Addressing Mode
    1,  // NADR
    1,  // ACC
    2,  // REL
    1,  // IMP
    2,  // IMM
    2,  // ZP
    2,  // ZPX
    2,  // ZPY
    3,  // ABS
    3,  // ABSX
    3,  // ABSY
    3,  // IND
    2,  // INDX
    2,  // INDY
};

static AddressingMode addressingMode[] = { // Addressing Mode lookup based on opcode
//    0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F     
/*0*/ IMP,  INDX, NADR, NADR, NADR, ZP,   ZP,   NADR, IMP,  IMM,  ACC,  NADR, NADR, ABS,  ABS,  NADR,
/*1*/ REL,  INDY, NADR, NADR, NADR, ZPX,  ZPX,  NADR, IMP,  ABSY, NADR, NADR, NADR, ABSX, ABSX, NADR,
/*2*/ ABS,  INDX, NADR, NADR, ZP,   ZP,   ZP,   NADR, IMP,  IMM,  ACC,  NADR, ABS,  ABS,  ABS,  NADR,
/*3*/ REL,  INDY, NADR, NADR, NADR, ZPX,  ZPX,  NADR, IMP,  ABSY, NADR, NADR, NADR, ABSX, ABSX, NADR,
/*4*/ IMP,  INDX, NADR, NADR, NADR, ZP,   ZP,   NADR, IMP,  IMM,  ACC,  NADR, ABS,  ABS,  ABS,  NADR,
/*5*/ REL,  INDY, NADR, NADR, NADR, ZPX,  ZPX,  NADR, IMP,  ABSY, NADR, NADR, NADR, ABSX, ABSX, NADR,
/*6*/ IMP,  INDX, NADR, NADR, NADR, ZP,   ZP,   NADR, IMP,  IMM,  ACC,  NADR, IND,  ABS,  ABS,  NADR,
/*7*/ REL,  INDY, NADR, NADR, NADR, ZPX,  ZPX,  NADR, IMP,  ABSY, NADR, NADR, NADR, ABSX, ABSX, NADR,
/*8*/ NADR, INDX, NADR, NADR, ZP,   ZP,   ZP,   NADR, IMP,  NADR, IMP,  NADR, ABS,  ABS,  ABS,  NADR,
/*9*/ REL,  INDY, NADR, NADR, ZPX,  ZPX,  ZPY,  NADR, IMP,  ABSY, IMP,  NADR, NADR, ABSX, NADR, NADR,
/*A*/ IMM,  INDX, IMM,  NADR, ZP,   ZP,   ZP,   NADR, IMP,  IMM,  IMP,  NADR, ABS,  ABS,  ABS,  NADR,
/*B*/ REL,  INDY, NADR, NADR, ZPX,  ZPX,  ZPY,  NADR, IMP,  ABSY, IMP,  NADR, ABSX, ABSX, ABSY, NADR,
/*C*/ IMM,  INDX, NADR, NADR, ZP,   ZP,   ZP,   NADR, IMP,  IMM,  IMP,  NADR, ABS,  ABS,  ABS,  NADR,
/*D*/ REL,  INDY, NADR, NADR, NADR, ZPX,  ZPX,  NADR, IMP,  ABSY, NADR, NADR, NADR, ABSX, ABSX, NADR,
/*E*/ IMM,  INDX, NADR, NADR, ZP,   ZP,   ZP,   NADR, IMP,  IMM,  IMP,  NADR, ABS,  ABS,  ABS,  NADR,
/*F*/ REL,  INDY, NADR, NADR, NADR, ZPX,  ZPX,  NADR, IMP,  ABSY, NADR, NADR, NADR, ABSX, ABSX, NADR,
};

static uint8_t cycleCount[] = { // Cycle count based on opcode, 255 == illegal op, /**/ == conditional
//    0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F     
/*0*/ 7,    6,    255,  255,  255,  3,    5,    255,  3,    2,    2,    255,  255,  4,    6,    255, 
/*1*/ 2/3,  5,    255,  255,  255,  4,    6,    255,  2,    4,    255,  255,  255,  4,    7,    255, 
/*2*/ 6,    6,    255,  255,  3,    3,    5,    255,  4,    2,    2,    255,  4,    4,    6,    255, 
/*3*/ 2/3,  5,    255,  255,  255,  4,    6,    255,  2,    4,    255,  255,  255,  4,    7,    255, 
/*4*/ 6,    6,    255,  255,  255,  3,    5,    255,  3,    2,    2,    255,  3,    4,    6,    255, 
/*5*/ 2/3,  5,    255,  255,  255,  4,    6,    255,  2,    4,    255,  255,  255,  4,    7,    255, 
/*6*/ 6,    6,    255,  255,  255,  3,    5,    255,  4,    2,    2,    255,  5,    4,    7,    255, 
/*7*/ 2/3,  5,    255,  255,  255,  4,    6,    255,  2,    4,    255,  255,  255,  4,    6,    255, 
/*8*/ 255,  6,    255,  255,  3,    3,    3,    255,  2,    255,  2,    255,  4,    4,    4,    255, 
/*9*/ 2/3,  6,    255,  255,  4,    4,    4,    255,  2,    5,    2,    255,  255,  5,    255,  255, 
/*A*/ 2,    6,    2,    255,  3,    3,    3,    255,  2,    2,    2,    255,  4,    4,    4,    255, 
/*B*/ 2/3,  5,    255,  255,  4,    4,    4,    255,  2,    4,    2,    255,  4,    4,    4,    255, 
/*C*/ 2,    6,    255,  255,  3,    3,    5,    255,  2,    2,    2,    255,  4,    4,    6,    255, 
/*D*/ 2/3,  5,    255,  255,  255,  4,    6,    255,  2,    4,    255,  255,  255,  4,    7,    255, 
/*E*/ 2,    6,    255,  255,  3,    3,    5,    255,  2,    2,    2,    255,  4,    4,    6,    255, 
/*F*/ 2/3,  5,    255,  255,  255,  4,    6,    255,  2,    4,    255,  255,  255,  4,    7,    255, 

};