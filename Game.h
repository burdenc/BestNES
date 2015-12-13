#pragma once

#include <string>
#include "iNes.h"
#include "PRG.h"

class Game
{
public:
    static Game& getGame();
    bool loadGame(std::string path);
    PRG& getPRG();

private:
    Game();
    ~Game();

    static Game* gameInstance;
    iNesHeader header;
    PRG* prgROM;
    //CHR chrROM;
};

