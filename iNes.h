#pragma once

#include <stdint.h>

#define iNesHeaderConst "\x4E\x45\x53\x1A"    // "NES^Z"

struct iNesHeader
{
    uint8_t prgPages;    // Num of PRG ROM Pages in 16 KB Units
    uint8_t chrPages;    // Num of CHR ROM Pages in 8 KB Units

    //uint8_t prgRAMSize;  // Size of PRG RAM in 8 KB Units
    bool trainerPresent;
};