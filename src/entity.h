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

    int (*init)(Entity);

    /* move around on the board */
    int (*left)(Entity);
    int (*right)(Entity);
    int (*up)(Entity);
    int (*down)(Entity);

    int (*draw)(Entity);
};

extern Entity Entity_ctor(Entity e, TypeList types);
extern void Entity_delete(Entity e);

#endif
