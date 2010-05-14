#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"

CLASS(Entity)
{
    INHERIT(Object);
    
    SDL_Surface *tile;
    int x;
    int y;

    int FUNC(init) ARG();

    /* move around on the board */
    int FUNC(left) ARG();
    int FUNC(right) ARG();
    int FUNC(up) ARG();
    int FUNC(down) ARG();

    int FUNC(draw) ARG();
};

extern CTOR(Entity);
extern DTOR(Entity);

#endif
