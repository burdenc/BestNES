#include "Game.h"
#include "Bus.h"
#include "iNes.h"
#include "CHR.h"
#include "PRG.h"
#include "Debug.h"

#include <bitset>
#include <iostream>
#include <fstream>

Game::Game(Bus& bus) : bus(bus)
{
}

Game::~Game()
{
    delete[] prgROM;
    delete[] chrROM;
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
    uint8_t *rawChrROM = new uint8_t[header.chrPages * CHR::BANK_SIZE];

    gameIn.seekg(INES_HEADER_SIZE);
    if (header.trainerPresent)
    {
        gameIn.seekg(TRAINER_SIZE, gameIn.cur);
    }

    gameIn.read((char*) rawPrgROM, header.prgPages * PRG::BANK_SIZE);
    prgROM = new PRG(header.prgPages, rawPrgROM);

    gameIn.read((char*) rawChrROM, header.chrPages * CHR::BANK_SIZE);
    chrROM = new CHR(header.chrPages, rawChrROM);

    logGame("PRG ROM Size: %d\n", (int) header.prgPages);
    logGame("CHR ROM Size: %d\n", (int) header.chrPages);
    logGame("Trainer Present: %d\n", header.trainerPresent);
}

PRG& Game::getPRG()
{
    return *prgROM;
}

CHR& Game::getCHR()
{
    return *chrROM;
}