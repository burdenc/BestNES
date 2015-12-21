#pragma once
#include <stdint.h>

#include <string>
#include <sstream>

class Bus;

const size_t MEMORY_SIZE = 0xFFFF;
const size_t STACK_OFFSET = 0x0100;

class Memory
{
public:
    Memory(Bus& bus);
    ~Memory();

    void write(uint16_t index, uint8_t value);
    uint8_t read(uint16_t index);

    void push(uint8_t value);
    uint8_t pop();

    // Debug
    std::string getDebugStream();

private:
    Bus& bus;
    uint8_t* memory;

    uint8_t* evalAddress(uint16_t index);

    std::stringstream debugStream;
};

