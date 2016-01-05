#include "CHR.h"


CHR::CHR(uint8_t size, uint8_t* rawChrROM = nullptr) : size(size), ROM(rawChrROM)
{
    if (!rawChrROM)
    {
        ROM = new uint8_t[size * CHR::BANK_SIZE];
    }
}


CHR::~CHR()
{
    delete[] ROM;
}

uint8_t* CHR::getPtr(uint16_t address)
{
    // TODO: warn
    if (address >= BANK_SIZE)
    {
        return nullptr;
    }

    return ROM + address;
}