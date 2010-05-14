#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"
#include "app.h"

CLASS(Stoneage)
{
    INHERIT(App);

    SDL_Surface *screen;

};

#endif
