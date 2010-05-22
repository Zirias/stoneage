#ifndef STONEAGE_MOVE_H
#define STONEAGE_MOVE_H

#include "common.h"

typedef enum
{
    TR_Linear = 0,
    Trajectory_count
} Trajectory;

struct Move_impl;

CLASS(Move)
{
    INHERIT(Object);

    struct Move_impl *pimpl;
    Move next;
    Move prev;

    void FUNC(init) ARG(int x, int y, int tx, int ty, Trajectory t);
    int FUNC(step) ARG(int *x, int *y);
};

#endif
