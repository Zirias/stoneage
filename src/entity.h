#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"
#include "board.h"
#include "resource.h"

CLASS(Entity)
{
    INHERIT(Object);
    
    const SDL_Surface *tile;
    Board b;
    int x;
    int y;

    void FUNC(init) ARG(Board b, int x, int y);

    /* move around on the board */
    int FUNC(left) ARG();
    int FUNC(right) ARG();
    int FUNC(up) ARG();
    int FUNC(down) ARG();

    void FUNC(draw) ARG();

    /* virtual destructor */
    void FUNC(dispose) ARG();
};

#endif
