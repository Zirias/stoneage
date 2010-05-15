#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"
#include "ehandler.h"

struct Board;

CLASS(Entity)
{
    INHERIT(EHandler);
    
    struct Board *b;
    int x;
    int y;

    const SDL_Surface *FUNC(getSurface) ARG();
    const SDL_Surface *FUNC(getBaseSurface) ARG();

    void FUNC(init) ARG(
	    struct Board *b, int x, int y);

    /* move around on the board */
    int FUNC(left) ARG();
    int FUNC(right) ARG();
    int FUNC(up) ARG();
    int FUNC(down) ARG();

    /* virtual destructor */
    void FUNC(dispose) ARG();
};

#endif
