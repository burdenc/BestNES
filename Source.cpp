#include <iostream>
#include "Bus.h"
#include "Game.h"
#include "SDL.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL Init failed" << std::endl;
        return -1;
    }

    Bus bus;
    std::string path = "C:\\Users\\Cassidy\\Downloads\\nestest.nes";
    SDL_Log("Loading %s", path.c_str());
    bus.game().loadGame(path);

    bus.runNES();
    std::system("pause");
    return 0;
}