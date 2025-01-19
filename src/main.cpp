#include <SDL2/SDL.h>
#include <iostream>

#include "Game.h"
#undef main

int main(int argc, char *argv[])
{
    Game game;
    bool success = game.Init();
    if (success)
    {
        game.RunLoop();
    }
    return 0;
}
