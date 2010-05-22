#ifndef STONEAGE_LEVEL_H
#define STONEAGE_LEVEL_H

#include "common.h"

#define LVL_ROWS 24
#define LVL_COLS 32

struct Board;
struct Entity;
struct Resource;

struct Level_impl;

CLASS(Level)
{
    INHERIT(Object);

    struct Level_impl *pimpl;

    void FUNC(createEntities) ARG(struct Board *b, struct Entity **buffer);
    void FUNC(random) ARG();
    void FUNC(debug) ARG(int num);
    void FUNC(load) ARG(struct Resource *r);
};


#endif
