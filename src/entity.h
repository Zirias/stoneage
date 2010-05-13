#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"

struct Entity;
typedef struct Entity *Entity;

struct Entity
{
    CLASS();
    
    SDL_Surface *tile;
    int x;
    int y;

    int (*init)(void *);

    /* move around on the board */
    int (*left)(void *);
    int (*right)(void *);
    int (*up)(void *);
    int (*down)(void *);

    int (*draw)(void *);
};

extern Entity Entity_ctor(Entity e, TypeList types);
extern void Entity_dtor(Entity e);

#endif
