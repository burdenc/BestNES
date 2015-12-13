#include "PRG.h"

#include "SDL_log.h"


PRG::PRG(uint8_t size, uint8_t* rawPrgROM = nullptr) : size(size), ROM(rawPrgROM)
{
    if (!rawPrgROM)
    {
        ROM = new uint8_t[size * PRG::BANK_SIZE];
    }
    
    bank1 = bank2 = ROM;
    if (size >= 2) // more than one bank in ROM, map second to [0xC000, 0xFFFF]
    {
        bank2 = ROM + PRG::BANK_SIZE;
    }
}

PRG::~PRG()
{
    delete[] ROM;
}

//Get pointer to PRG ROM address
uint8_t* PRG::getPtr(uint16_t address)
{
    if (address >= PRG::BANK_SIZE)
    {
        return bank2 + (address - PRG::BANK_SIZE);
    }

    return bank1 + address;
}

void PRG::mapBank1(uint8_t bankNum)
{
    if (bankNum >= size)
    {
        SDL_LogWarn(0, "Attempted to map PRG bank1 to invalid number %d (PRG size: %d)", bankNum, size);
        return;
    }

    bank1 = ROM + (bankNum * PRG::BANK_SIZE);
}

void PRG::mapBank2(uint8_t bankNum)
{
    if (bankNum >= size)
    {
        SDL_LogWarn(0, "Attempted to map PRG bank2 to invalid number %d (PRG size: %d)", bankNum, size);
        return;
    }

    bank2 = ROM + (bankNum * PRG::BANK_SIZE);
}

uint8_t PRG::getSize()
{
    return size;
}