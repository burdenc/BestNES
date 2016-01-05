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

class PPU
{
public:
    PPU(Bus& bus);
    ~PPU();

    void init();

private:
    static const size_t VRAM_SIZE = 0x4000;
    static const size_t OAM_SIZE = 0x0100;

    static const size_t NUM_PATTERNS = 0x200;

    static const size_t SCREEN_WIDTH = 256;
    static const size_t SCREEN_HEIGHT = 244;

    Bus& bus;
    uint8_t* VRAM;
    uint8_t* OAM;

    SDL_Window* window = NULL;
    SDL_Surface* windowSurface = NULL;

    SDL_Surface* patterns[CHR::NUM_PATTERNS];

    void loadPatterns();
};

