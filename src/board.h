#ifndef STONEAGE_BOARD_H
#define STONEAGE_BOARD_H

#include <SDL.h>

#include "common.h"
#include "ehandler.h"

struct Board_impl;

struct Entity;

CLASS(Board)
{
    INHERIT(EHandler);

    struct Board_impl *pimpl;

    void FUNC(initVideo) ARG();
    void FUNC(redraw) ARG();
    void FUNC(draw) ARG(int x, int y, int refresh);
    int FUNC(isEmpty) ARG(int x, int y);
    void FUNC(startMove) ARG(
	    struct Entity *e, int dir_x, int dir_y);

    const SDL_Surface *FUNC(getEmptyTile) ARG();
    const SDL_Surface *FUNC(getEarthTile) ARG();
    const SDL_Surface *FUNC(getWallTile) ARG();
    const SDL_Surface *FUNC(getRockTile) ARG();
    const SDL_Surface *FUNC(getCabbageTile) ARG();
    const SDL_Surface *FUNC(getWillyTile) ARG();
};

#endif
