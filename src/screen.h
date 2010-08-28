#ifndef STONEAGE_SCREEN_H
#define STONEAGE_SCREEN_H

#include "common.h"

struct Resfile;

CLASS(Screen)
{
    INHERIT(Object);


    struct Resfile *graphics;

};

extern Screen getScreen(void);

#endif
