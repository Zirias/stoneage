#ifndef STONEAGE_BOARD_H
#define STONEAGE_BOARD_H

#include <SDL.h>

#include "common.h"

struct Board_impl;

CLASS(Board)
{
    INHERIT(Object);

    struct Board_impl *pimpl;

    void FUNC(init) ARG(SDL_Surface *screen, int width, int height);
    SDL_Surface *FUNC(getScreen) ARG();
    const SDL_Surface *FUNC(getEmptyTile) ARG();
    const SDL_Surface *FUNC(getEarthTile) ARG();
    const SDL_Surface *FUNC(getWallTile) ARG();
    const SDL_Surface *FUNC(getRockTile) ARG();
    const SDL_Surface *FUNC(getCabbageTile) ARG();
    const SDL_Surface *FUNC(getWillyTile) ARG();
};

#endif
