#pragma once
#include <stdint.h>

class Bus
{
public:
    static uint32_t cycles;
    static uint32_t nextCycles;
    Bus();
    ~Bus();
};