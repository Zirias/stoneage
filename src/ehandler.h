#ifndef STONEAGE_EHANDLER_H
#define STONEAGE_EHANDLER_H

#include "common.h"

struct Event;

CLASS(EHandler)
{
    INHERIT(Object);

    void FUNC(handleEvent) ARG(struct Event *e);
};

#endif
