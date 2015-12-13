#pragma once
#include <stdint.h>

#include <string>
#include <sstream>

const size_t MEMORY_SIZE = 0xFFFF;
const size_t STACK_OFFSET = 0x0100;

class Memory
{
public:
    Memory();
    ~Memory();

    void write(unsigned int index, uint8_t value);
    uint8_t read(unsigned int index);

    void push(uint8_t& stackPtr, uint8_t value);
    uint8_t pop(uint8_t& stackPtr);

    // Debug
    std::string getDebugStream();

private:
    uint8_t* memory;

    uint8_t* evalAddress(unsigned int index);

    std::stringstream debugStream;
};

