#pragma once

#include <string>
#include "iNes.h"

class PRG;
class CHR;
class Bus;

class Game
{
public:
    Game(Bus& bus);
    ~Game();
    bool loadGame(std::string path);
    PRG& getPRG();
    CHR& getCHR();

    static const size_t TRAINER_SIZE = 512;

private:
    iNesHeader header;
    PRG* prgROM;
    CHR* chrROM;

    Bus& bus;
};

