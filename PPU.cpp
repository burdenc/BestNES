#include "PPU.h"
#include "Config.h"
#include "Bus.h"
#include "Game.h"

#include <bitset>
#include <iostream>

static const SDL_Color transparent = { 0, 0, 0, 0 }; // Transparent color used for color 0 in sprites
static const size_t NUM_COLORS = 128;

SDL_Color palette[NUM_COLORS];
uint16_t rawPalette[];

PPU::PPU(Bus& bus) : bus(bus), VRAM(new uint8_t[VRAM_SIZE]), OAM(new uint8_t[OAM_SIZE])
{
    // Convert palette to SDL format
    for (uint8_t i = 0; i < NUM_COLORS; i++)
    {
        // rawPalette is RGB in 0-7, need to convert to 0-255 thus the mult 36
        palette[i].r = ((rawPalette[i] >> 6) & 0x07) * 36;
        palette[i].g = ((rawPalette[i] >> 3) & 0x07) * 36;
        palette[i].b = ((rawPalette[i] >> 0) & 0x07) * 36;
        palette[i].a = 255;
    }

    window = SDL_CreateWindow(
        "NES Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * WINDOW_SCALE,
        SCREEN_HEIGHT * WINDOW_SCALE,
        SDL_WINDOW_OPENGL
        );

    if (window == NULL) {
        std::cout << "SDL Window could not be created: " << SDL_GetError() << std::endl;
    }

    windowSurface = SDL_GetWindowSurface(window);
    frameSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 255);
}

PPU::~PPU()
{
    delete[] VRAM;
    delete[] OAM;

    for (int i = 0; i < NUM_PATTERNS; i++)
    {
        SDL_FreeSurface(patterns[i]);
    }
}

void PPU::init()
{
    loadPatterns();

    ppustatus.byte = 0xA0; // Power up state

    SDL_Rect srcRect;
    srcRect.x = -8;
    srcRect.y = 0;
    srcRect.h = 8;
    srcRect.w = 8;
    for (int i = 0; i < CHR::NUM_PATTERNS; i++)
    {
        srcRect.x += 8;
        if (srcRect.x >= SCREEN_WIDTH) { srcRect.x = 0; srcRect.y += 8; }
        if (srcRect.y >= SCREEN_HEIGHT) { srcRect.y = 0; }
        if (SDL_BlitSurface(patterns[i], NULL, frameSurface, &srcRect) != 0)
        {
            std::cout << "Blit failed: " << SDL_GetError() << std::endl;
        }
    }

    displayFrame();
}

void PPU::displayFrame()
{
    /*SDL_Surface* optimizedSurface = SDL_ConvertSurface(frameSurface, windowSurface->format, 0);
    if (optimizedSurface == NULL)
    {
        std::cout << "Convert failed: " << SDL_GetError() << std::endl;
    }*/

    // Automatically scale frame buffer to window size
    if (SDL_BlitScaled(frameSurface, NULL, windowSurface, NULL) != 0)
    {
        std::cout << "Blit failed: " << SDL_GetError() << std::endl;
    }

    SDL_UpdateWindowSurface(window);
}

void PPU::loadPatterns()
{
    std::bitset<8> lower, upper;
    for (int i = 0; i < CHR::NUM_PATTERNS; i++)
    {
        patterns[i] = SDL_CreateRGBSurface(0, CHR::PATTERN_HEIGHT, CHR::PATTERN_WIDTH, 32, 0, 0, 0, 255);
        SDL_Surface* patt = patterns[i];
        SDL_LockSurface(patt);

        uint32_t* pixels = (uint32_t*) patt->pixels;
        for (int j = 0; j < CHR::PATTERN_HEIGHT; j++)
        {
            lower = *bus.game().getCHR().getPtr((i * 0x10) + j);
            upper = *bus.game().getCHR().getPtr((i * 0x10) + CHR::PATTERN_HEIGHT + j);

            for (int k = CHR::PATTERN_WIDTH - 1; k >= 0; k--)
            {
                uint8_t val = (upper[k] << 1) | lower[k];
                *pixels = getColor(val);
                pixels++;
            }
        }

        SDL_UnlockSurface(patt);
    }
}


{

// Get raw pixel value from color index
uint32_t PPU::getColor(size_t index)
{
    SDL_Color* color = &palette[index];
    return SDL_MapRGBA(frameSurface->format, color->r, color->g, color->b, color->a);
}

// Value = (R << 6) | (G << 3) | B, 3 bits for each color
uint16_t rawPalette[NUM_COLORS] = {
//    0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F
/*0*/ 0x0DB, 0x00C, 0x006, 0x0D6, 0x103, 0x143, 0x148, 0x110, 0x0D0, 0x050, 0x019, 0x020, 0x012, 0x000, 0x000, 0x000,
/*1*/ 0x16D, 0x01E, 0x017, 0x107, 0x147, 0x1C4, 0x1C0, 0x198, 0x118, 0x060, 0x020, 0x02B, 0x024, 0x000, 0x000, 0x000,
/*2*/ 0x1FF, 0x0EF, 0x127, 0x19F, 0x1C7, 0x1DF, 0x1E0, 0x1E8, 0x1B0, 0x0F0, 0x038, 0x0BE, 0x03F, 0x000, 0x000, 0x000,
/*3*/ 0x1FF, 0x177, 0x1AF, 0x1EF, 0x1E7, 0x1ED, 0x1F4, 0x1FA, 0x1FB, 0x17A, 0x13B, 0x0BE, 0x137, 0x000, 0x000, 0x000,
};