#ifndef STONEAGE_MOVE_H
#define STONEAGE_MOVE_H

#include "common.h"

#include <SDL.h>

struct Entity;

typedef enum
{
    TR_Linear = 0,
    TR_CircleX,
    Trajectory_count
} Trajectory;

struct Move_impl;

CLASS(Move)
{
    INHERIT(Object);

    struct Move_impl *pimpl;
    Move next;
    Move prev;
    int dx;
    int dy;

    void FUNC(init) ARG(struct Entity *e, int dx, int dy, Trajectory t);
    struct Entity *FUNC(entity) ARG();
    int FUNC(step) ARG(Uint16 *x, Uint16 *y);
};

extern void
computeTrajectories(int tile_width, int tile_height);

#endif
