#ifndef STONEAGE_STONEAGE_H
#define STONEAGE_STONEAGE_H

#include <SDL.h>

#include "common.h"

struct Stoneage;
typedef struct Stoneage *Stoneage;

extern Stoneage mainApp;

struct Stoneage
{
    CLASS();

    SDL_Surface *screen;

    void (*run)(Stoneage);
    void (*abort)(Stoneage);
};

extern Stoneage Stoneage_ctor(Stoneage s, TypeList types);
extern void Stoneage_dtor(Stoneage s);
extern void Stoneage_run(Stoneage s);
extern void Stoneage_abort(Stoneage s);

#endif
