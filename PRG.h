#pragma once

#include <stdint.h>

class PRG
{
public:
    PRG(uint8_t size, uint8_t* rawPrgROM);
    ~PRG();

    uint8_t* getPtr(uint16_t address);
    uint8_t getSize();

    void mapBank1(uint8_t bankNum);
    void mapBank2(uint8_t bankNum);

    static const size_t BANK_SIZE = 0x4000;

private:
    uint8_t size;
    uint8_t *bank1, *bank2;
    uint8_t* ROM;
};

