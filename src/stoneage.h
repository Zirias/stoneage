#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"
#include "app.h"

struct Board;

CLASS(Stoneage)
{
    INHERIT(App);

    SDL_Surface *screen;

    int res_x;
    int res_y;
    int bpp;
    Uint32 modeflags;

    SDL_TimerID ticker;
    SDL_TimerID keyCheck;

    struct Board *board;
};

#endif
