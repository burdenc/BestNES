#include "PPU.h"
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
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL
        );

    if (window == NULL) {
        std::cout << "SDL Window could not be created: " << SDL_GetError() << std::endl;
    }

    windowSurface = SDL_GetWindowSurface(window);

    // Test
    /*int ret = 0;
    SDL_Surface* img = SDL_CreateRGBSurface(0, 128, 128, 8, 0, 0, 0, 0);
    if (img == NULL)
    {
        std::cout << "Couldn't create surface" << std::endl;
        std::cout << SDL_GetError();
    }
    SDL_Palette* p = SDL_AllocPalette(256);
    if (SDL_SetPaletteColors(p, palette, 0, NUM_COLORS) < 0)
    {
        std::cout << "Couldn't set palette colors" << std::endl;
    }*/
    //std::cout << "Bits per pixel: " << img->format->BitsPerPixel << std::endl;
    //SDL_ConvertSurfaceFormat(img, SDL_PIXELTYPE_INDEX1, 0);
    //SDL_PixelFormat* f = SDL_AllocFormat(SDL_PIXELTYPE_INDEX1);
    //f->palette = p;
    //SDL_ConvertSurface(windowSurface, f, 0);
    /*if (SDL_SetSurfacePalette(img, p) != 0)
    {
        std::cout << "Couldn't set palette to surface" << std::endl;
        std::cout << SDL_GetError();
    }*/

    /*for (int j = 0; j < NUM_COLORS; j++)
    {
        SDL_LockSurface(img);
        Uint8* pixels = (Uint8*)img->pixels;
        for (int i = 0; i < 128 * 128; i++)
            pixels[i] = j;
        SDL_UnlockSurface(img);
        SDL_BlitSurface(img, NULL, windowSurface, NULL);

        SDL_UpdateWindowSurface(window);
    }*/
}

void PPU::init()
{
    loadPatterns();

    /*SDL_Surface* patt = patterns[0];
    SDL_LockSurface(patt);
    Uint8* pixels = (Uint8*)patt->pixels;
    for (int i = 0; i < 8 * 8; i++)
    *pixels++ = 0x16;
    SDL_UnlockSurface(patt);*/

    SDL_Rect srcRect;
    srcRect.x = -32;
    srcRect.y = 0;
    srcRect.h = 32;
    srcRect.w = 32;
    for (int i = 0; i < CHR::NUM_PATTERNS; i++)
    {
        srcRect.x += 32;
        if (srcRect.x >= SCREEN_WIDTH) { srcRect.x = 0; srcRect.y += 32; }
        if (srcRect.y >= SCREEN_HEIGHT) { srcRect.y = 0; }
        if (SDL_BlitScaled(SDL_ConvertSurface(patterns[i], windowSurface->format, 0), NULL, windowSurface, &srcRect) != 0)
        {
            std::cout << "Blit failed: " << std::endl;
            std::cout << SDL_GetError();
        }
        if (SDL_UpdateWindowSurface(window) != 0)
        {
            std::cout << "Update failed: " << std::endl;
            std::cout << SDL_GetError();
        }
    }
}

void PPU::loadPatterns()
{
    SDL_Palette* p = SDL_AllocPalette(256);
    if (SDL_SetPaletteColors(p, palette, 0, NUM_COLORS) < 0)
    {
        std::cout << "Couldn't set palette colors" << std::endl;
    }

    std::bitset<8> lower, upper;
    for (int i = 0; i < CHR::NUM_PATTERNS; i++)
    {
        patterns[i] = SDL_CreateRGBSurface(0, CHR::PATTERN_HEIGHT, CHR::PATTERN_WIDTH, 8, 0, 0, 0, 0);
        SDL_Surface* patt = patterns[i];
        SDL_LockSurface(patt);

        SDL_ConvertSurfaceFormat(patt, SDL_PIXELTYPE_INDEX1, 0);
        //SDL_ConvertSurfaceFormat(patt, SDL_PIXELFORMAT_INDEX1, 0);

        if (SDL_SetSurfacePalette(patt, p) != 0)
        {
            std::cout << "Couldn't set palette to surface" << std::endl;
            std::cout << SDL_GetError();
        }

        Uint8* pixels = (Uint8*) patt->pixels;
        for (int j = 0; j < CHR::PATTERN_HEIGHT; j++)
        {
            uint8_t lval = *bus.game().getCHR().getPtr((i * 0x10) + j), uval = *bus.game().getCHR().getPtr((i * 0x10) + CHR::PATTERN_HEIGHT + j);
            lower = *bus.game().getCHR().getPtr((i * 0x10) + j);
            upper = *bus.game().getCHR().getPtr((i * 0x10) + CHR::PATTERN_HEIGHT + j);

            for (int k = CHR::PATTERN_WIDTH - 1; k >= 0; k--)
            {
                uint8_t val = (upper[k] << 1) | lower[k];
                uint8_t lowerVal = lower[k], upperVal = upper[k];
                *pixels = val;
                pixels++;
            }
        }

        SDL_UnlockSurface(patt);
    }
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

// Value = (R << 6) | (G << 3) | B, 3 bits for each color
uint16_t rawPalette[NUM_COLORS] = {
//    0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F
/*0*/ 0x0DB, 0x00C, 0x006, 0x0D6, 0x103, 0x143, 0x148, 0x110, 0x0D0, 0x050, 0x019, 0x020, 0x012, 0x000, 0x000, 0x000,
/*1*/ 0x16D, 0x01E, 0x017, 0x107, 0x147, 0x1C4, 0x1C0, 0x198, 0x118, 0x060, 0x020, 0x02B, 0x024, 0x000, 0x000, 0x000,
/*2*/ 0x1FF, 0x0EF, 0x127, 0x19F, 0x1C7, 0x1DF, 0x1E0, 0x1E8, 0x1B0, 0x0F0, 0x038, 0x0BE, 0x03F, 0x000, 0x000, 0x000,
/*3*/ 0x1FF, 0x177, 0x1AF, 0x1EF, 0x1E7, 0x1ED, 0x1F4, 0x1FA, 0x1FB, 0x17A, 0x13B, 0x0BE, 0x137, 0x000, 0x000, 0x000,
};