#include "Game.h"
#include "iNes.h"
#include "PRG.h"
#include "SDL_log.h"

#include <bitset>
#include <iostream>
#include <fstream>

Game::Game()
{
}

Game::~Game()
{
}

bool Game::loadGame(std::string path)
{
    std::ifstream gameIn;
    gameIn.open(path, std::ios::binary);

    char inputHeaderConst[4];
    gameIn.read(inputHeaderConst, 4);
    if (memcmp(inputHeaderConst, iNesHeaderConst, 4))
    {
        std::cout << "Invalid iNes header const" << std::endl;
        return false;
    }

    gameIn.get((char&) header.prgPages);
    gameIn.get((char&) header.chrPages);

    char flag6Raw;
    gameIn.get(flag6Raw);
    std::bitset<8> flag6(flag6Raw);
    header.trainerPresent = flag6[2];

    //TODO: Read in the other crap
    uint8_t *rawPrgROM = new uint8_t[header.prgPages * PRG::BANK_SIZE];
    gameIn.seekg(16);
    if (header.trainerPresent) { gameIn.seekg(512); }

    gameIn.read((char*) rawPrgROM, header.prgPages * PRG::BANK_SIZE);
    prgROM = new PRG(header.prgPages, rawPrgROM);

    SDL_Log("PRG ROM Size: %d\n", (int) header.prgPages);
    SDL_Log("CHR ROM Size: %d\n", (int) header.chrPages);
    SDL_Log("Trainer Present: %d\n", header.trainerPresent);
}

PRG& Game::getPRG()
{
    return *prgROM;
}

Game& Game::getGame()
{
    static Game instance;
    return instance;
}