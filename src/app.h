#ifndef STONEAGE_APP_H
#define STONEAGE_APP_H

#include "common.h"

CLASS(App)
{
    INHERIT(Object);

    int FUNC(run) ARG(int argc, char **argv);
    void FUNC(abort) ARG();
};

extern App mainApp;

#endif
