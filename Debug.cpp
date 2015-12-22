#include "Debug.h"
#include "Bus.h"

#include <iostream>
#include <iomanip>

Debug::Debug(Bus& bus) : bus(bus)
{
}


Debug::~Debug()
{
}

char* opLookup[];

void Debug::logOpDebug(Argument arg)
{

    opDebugStream << toHex(bus.cpuState().PC - instructionSize[arg.addrMode], 4) << "  ";

    std::stringstream rawOpCode;
    for (int i = 0; i < instructionSize[arg.addrMode]; i++)
    {
        rawOpCode << toHex((int)arg.rawInstr[i], 2) << " ";
    }
    opDebugStream << std::setw(10) << std::left << rawOpCode.str();

    opDebugStream << std::right;

    std::stringstream addrStr;
    addrStr << opLookup[arg.rawInstr[0]] << " ";
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
    opDebugStream << std::setw(31) << std::left << addrStr.str();

    uint32_t size = (instructionSize[arg.addrMode] == 1) ? 2 : instructionSize[arg.addrMode];
    opDebugStream << " A:" << toHex(bus.cpuState().A, 2)
                  << " X:" << toHex(bus.cpuState().X, 2)
                  << " Y:" << toHex(bus.cpuState().Y, 2)
                  << " P:" << toHex(bus.cpuState().P.toByte(), 2)
                  << " SP:" << toHex(bus.cpuState().SP, 2)
                  << " CPUC:" << bus.cpuCycleCount() - size;
}

void Debug::logMemoryDebug(const char* op, uint8_t value, uint16_t index)
{
    memoryDebugStream << std::setw(6)  << ""
                      << std::setw(10) << std::left << std::string(op)
                      << "$" << toHex(index, 4) << " = " << toHex(value, 2)
                      << std::endl;
}

void Debug::flushAll()
{
    std::string debug;
    
    debug = opDebug();
    if (!debug.empty())
    {
        std::cout << debug << std::endl;
    }

    debug = memoryDebug();
    if (!debug.empty())
    {
        std::cout << debug << std::endl;
    }
}

std::string Debug::opDebug()
{
    std::string out = opDebugStream.str();
    opDebugStream.str("");
    return out;
}

std::string Debug::memoryDebug()
{
    std::string out = memoryDebugStream.str();
    memoryDebugStream.str("");
    return out;
}

std::string Debug::toHex(int val, int width)
{
    std::stringstream hex;
    hex << std::setw(width) << std::setfill('0') << std::uppercase << std::hex << val;
    return hex.str();
}

char* opLookup[] = {   // Instruction name lookup
    //    0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F     
    /*0*/ "BRK", "ORA", "NOP", "NOP", "NOP", "ORA", "ASL", "NOP", "PHP", "ORA", "ASL", "NOP", "NOP", "ORA", "ASL", "NOP",
    /*1*/ "BPL", "ORA", "NOP", "NOP", "NOP", "ORA", "ASL", "NOP", "CLC", "ORA", "NOP", "NOP", "NOP", "ORA", "ASL", "NOP",
    /*2*/ "JSR", "AND", "NOP", "NOP", "BIT", "AND", "ROL", "NOP", "PLP", "AND", "ROL", "NOP", "BIT", "AND", "ROL", "NOP",
    /*3*/ "BMI", "AND", "NOP", "NOP", "NOP", "AND", "ROL", "NOP", "SEC", "AND", "NOP", "NOP", "NOP", "AND", "ROL", "NOP",
    /*4*/ "RTI", "EOR", "NOP", "NOP", "NOP", "EOR", "LSR", "NOP", "PHA", "EOR", "LSR", "NOP", "JMP", "EOR", "LSR", "NOP",
    /*5*/ "BVC", "EOR", "NOP", "NOP", "NOP", "EOR", "LSR", "NOP", "CLI", "EOR", "NOP", "NOP", "NOP", "EOR", "LSR", "NOP",
    /*6*/ "RTS", "ADC", "NOP", "NOP", "NOP", "ADC", "ROR", "NOP", "PLA", "ADC", "ROR", "NOP", "JMP", "ADC", "ROR", "NOP",
    /*7*/ "BVS", "ADC", "NOP", "NOP", "NOP", "ADC", "ROR", "NOP", "SEI", "ADC", "NOP", "NOP", "NOP", "ADC", "ROR", "NOP",
    /*8*/ "NOP", "STA", "NOP", "NOP", "STY", "STA", "STX", "NOP", "DEY", "NOP", "TXA", "NOP", "STY", "STA", "STX", "NOP",
    /*9*/ "BCC", "STA", "NOP", "NOP", "STY", "STA", "STX", "NOP", "TYA", "STA", "TXS", "NOP", "NOP", "STA", "NOP", "NOP",
    /*A*/ "LDY", "LDA", "LDX", "NOP", "LDY", "LDA", "LDX", "NOP", "TAY", "LDA", "TAX", "NOP", "LDY", "LDA", "LDX", "NOP",
    /*B*/ "BCS", "LDA", "NOP", "NOP", "LDY", "LDA", "LDX", "NOP", "CLV", "LDA", "TSX", "NOP", "LDY", "LDA", "LDX", "NOP",
    /*C*/ "CPY", "CMP", "NOP", "NOP", "CPY", "CMP", "DEC", "NOP", "INY", "CMP", "DEX", "NOP", "CPY", "CMP", "DEC", "NOP",
    /*D*/ "BNE", "CMP", "NOP", "NOP", "NOP", "CMP", "DEC", "NOP", "CLD", "CMP", "NOP", "NOP", "NOP", "CMP", "DEC", "NOP",
    /*E*/ "CPX", "SBC", "NOP", "NOP", "CPX", "SBC", "INC", "NOP", "INX", "SBC", "NOP", "NOP", "CPX", "SBC", "INC", "NOP",
    /*F*/ "BEQ", "SBC", "NOP", "NOP", "NOP", "SBC", "INC", "NOP", "SED", "SBC", "NOP", "NOP", "NOP", "SBC", "INC", "NOP",
};