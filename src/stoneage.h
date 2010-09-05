#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"
#include "app.h"
#include "event.h"

struct Stoneage_impl;

typedef struct
{
    int x;
    int y;
} Stoneage_MoveWillyData;

CLASS(Stoneage)
{
    INHERIT(App);

    struct Stoneage_impl *pimpl;

    SDL_Surface *screen;

    Event MoveWilly;
    Event Tick;

    int res_x;
    int res_y;
    int bpp;
    Uint32 modeflags;
};

#endif
