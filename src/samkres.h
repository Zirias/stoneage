#ifndef STONEAGE_SAMKRES_H
#define STONEAGE_SAMKRES_H

#include "common.h"
#include "app.h"

struct Resfile;

CLASS(SaMkres)
{
    INHERIT(App);

    struct Resfile *resfile;
};

#endif
