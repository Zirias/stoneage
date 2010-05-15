#ifndef STONEAGE_EROCK_H
#define STONEAGE_EROCK_H

#include "common.h"
#include "entity.h"

CLASS(ERock)
{
    INHERIT(Entity);

    int falling;
    void FUNC(fall) ARG();
};

#endif
