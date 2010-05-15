#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"
#include "board.h"
#include "resource.h"

CLASS(Entity)
{
    INHERIT(Object);
    
    Board b;
    int x;
    int y;

    const SDL_Surface *FUNC(getSurface) ARG();

    void FUNC(init) ARG(Board b, int x, int y);

    /* move around on the board */
    int FUNC(left) ARG();
    int FUNC(right) ARG();
    int FUNC(up) ARG();
    int FUNC(down) ARG();

    /* virtual destructor */
    void FUNC(dispose) ARG();
};

#endif
