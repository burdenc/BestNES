#include <iostream>
#include "Bus.h"
#include "Game.h"
#include "SDL.h"

void printState(ConditionCodes6502);

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL Init failed" << std::endl;
        return -1;
    }

    /*SDL_Window *window = SDL_CreateWindow(
                         "NES Emulator",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         256,
                         244,
                         SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        std::cout << "SDL Window could not be created: " << SDL_GetError() << std::endl;
        return -1;
    }

    Nes6502* nes = new Nes6502();
    uint8_t instr[] = {0x69, 0x80, 0};
    nes->state.A = 0xFF;
    nes->executeOpCode(instr);
    printState(nes->state.P);

    std::cout << (uint16_t) nes->state.A << std::endl;*/

    //std::string path = "C:\\Users\\Cassidy\\Documents\\NES\\instr_test-v4(1)\\instr_test-v4\\rom_singles\\01-basics.nes";
    std::string path = "C:\\Users\\Cassidy\\Downloads\\nestest.nes";
    SDL_Log("Loading %s", path.c_str());
    Game::getGame().loadGame(path);

    Bus bus;
    bus.runNES();
    std::system("pause");
    return 0;
}

void printState(ConditionCodes6502 state)
{
    std::printf("C %d\n", state.C);
    std::printf("Z %d\n", state.Z);
    std::printf("I %d\n", state.I);
    std::printf("D %d\n", state.D);
    std::printf("B %d\n", state.B);
    std::printf("V %d\n", state.V);
    std::printf("N %d\n", state.N);
}