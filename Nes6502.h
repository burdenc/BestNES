#pragma once
#include <stdint.h>

class Bus;
struct Argument;

struct ConditionCodes6502
{
    uint8_t C : 1;  // Carry Flag
    uint8_t Z : 1;  // Zero Flag
    uint8_t I : 1;  // Interrupt Disable
    uint8_t D : 1;  // Decimal Mode, Ignored
    uint8_t B : 1;  // Break Command
    uint8_t p : 1;  // Padding Bit
    uint8_t V : 1;  // Overflow Flag
    uint8_t N : 1;  // Negative Flag

    ConditionCodes6502()
    {
        C = Z = D = B = V = N = 0;
        I = p = 1;
    }

    uint8_t toByte()
    {
        uint8_t byte = 0;
        byte |= C << 0;
        byte |= Z << 1;
        byte |= I << 2;
        byte |= D << 3;
        byte |= B << 4;
        byte |= p << 5;
        byte |= V << 6;
        byte |= N << 7;
        return byte;
    }

    void set(uint8_t byte)
    {
        C = (byte & 0x01) >> 0;
        Z = (byte & 0x02) >> 1;
        I = (byte & 0x04) >> 2;
        D = (byte & 0x08) >> 3;
        //B = (byte & 0x10) >> 4;       Both RTI and PLP ignore B and p bits
        //p = (byte & 0x20) >> 5;
        V = (byte & 0x40) >> 6;
        N = (byte & 0x80) >> 7;
    }
};

struct State6502
{
    // Registers
    uint16_t            PC; // Program Counter
    uint8_t             SP; // Stack Pointer
    ConditionCodes6502  P;  // Status
    uint8_t             A;  // Accumulator
    uint8_t             X;  // Index X
    uint8_t             Y;  // Index Y
};

class Nes6502
{
public:
    State6502 state;

    Nes6502(Bus& bus);
    ~Nes6502();

    void emulate6502();
    void executeOpCode(uint8_t instr[]);

private:
    Bus& bus;
    Argument obtainArgument(uint8_t instr[]);
    void pageWrap(Argument& arg, uint8_t offset);
};

