#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"
#include "app.h"
#include "board.h"

CLASS(Stoneage)
{
    INHERIT(App);

    SDL_Surface *screen;
    Board board;
};

#endif
