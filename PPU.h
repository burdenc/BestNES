#pragma once

#include <stdint.h>
#include <SDL.h>
#include "CHR.h"

class Bus;

struct SpriteFlags
{
    uint8_t palette        : 2; // palette #4-7
    uint8_t                : 3; // padding
    uint8_t priority       : 1; // 0 in front of background, 1 behind background
    uint8_t horizontalFlip : 1;
    uint8_t verticalFlip   : 1;
};

struct Sprite
{
    uint8_t y;
    uint8_t tile;
    SpriteFlags flags;
    uint8_t x;
};

union PPUSTATUS_reg
{
    struct ppustatus_bits {
        uint8_t l : 5; // padding/latch
        uint8_t O : 1; // sprite overflow
        uint8_t S : 1; // sprite 0 hit
        uint8_t V : 1; // v-blank
    } bits;
    uint8_t byte;
};

class PPU
{
public:
    uint8_t registerLatch; // Used by memory IO, how PPU register communicates with memory controller

    PPU(Bus& bus);
    ~PPU();

    void init();
    void displayFrame();

    // Registers
    // Write
    void rPPUCTRL(uint8_t value);
    void rPPUMASK(uint8_t value);
    void rPPUSTATUS(uint8_t value);
    void rOAMADDR(uint8_t value);
    void rOAMDATA_write(uint8_t value);
    void rPPUSCROLL(uint8_t value);
    void rPPUADDR(uint8_t value);
    void rPPUDATA_write(uint8_t value);
    void rOAMDMA(uint8_t value);

    // Read
    uint8_t rPPUSTATUS();
    uint8_t rOAMDATA_read();
    uint8_t rPPUDATA_read();

private:
    static const size_t VRAM_SIZE = 0x4000;
    static const size_t OAM_SIZE = 0x0100;

    static const size_t NUM_PATTERNS = 0x200;

    static const size_t SCREEN_WIDTH = 256;
    static const size_t SCREEN_HEIGHT = 244;

    Bus& bus;
    uint8_t* VRAM;
    uint8_t* OAM;

    PPUSTATUS_reg ppustatus;

    SDL_Window* window = NULL;
    SDL_Surface* windowSurface = NULL;
    SDL_Surface* frameSurface = NULL;

    SDL_Surface* patterns[CHR::NUM_PATTERNS];

    void loadPatterns();
    inline uint32_t getColor(size_t index);
};

