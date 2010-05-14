#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"

CLASS(Stoneage)
{
    INHERIT(Object);

    SDL_Surface *screen;

    void FUNC(run) ARG();
    void FUNC(abort) ARG();
};

extern Stoneage mainApp;

extern CTOR(Stoneage);
extern DTOR(Stoneage);

#endif
