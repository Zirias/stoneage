#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"
#include "app.h"
#include "event.h"

struct Board;

typedef struct
{
    int x, y;
} MoveWillyEventData;

struct Stoneage_impl;

CLASS(Stoneage)
{
    INHERIT(App);

    struct Stoneage_impl *pimpl;

    Event MoveWilly;
    Event Tick;

    SDL_Surface *screen;

    int res_x;
    int res_y;
    int bpp;
    Uint32 modeflags;
};

#endif
