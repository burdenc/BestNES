#pragma once

#include <stdint.h>

class CHR
{
public:
    CHR(uint8_t size, uint8_t* rawChrROM);
    ~CHR();

    uint8_t* getPtr(uint16_t address);

    static const size_t BANK_SIZE      = 0x2000;
    static const size_t NUM_PATTERNS   = 0x200;
    static const size_t PATTERN_HEIGHT = 8;
    static const size_t PATTERN_WIDTH  = 8;

private:
    uint8_t size;
    uint8_t* ROM;
};

