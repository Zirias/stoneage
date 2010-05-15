#ifndef STONEAGE_RESFILE_H
#define STONEAGE_RESFILE_H

#include <stdio.h>

#include "common.h"

struct Resource;

struct Resfile_impl;

CLASS(Resfile)
{
    INHERIT(Object);

    struct Resfile_impl *pimpl;

    int FUNC(canRead) ARG();
    int FUNC(canWrite) ARG();
    int FUNC(setFile) ARG(const char *filename);
    int FUNC(open) ARG(int writeable);
    void FUNC(close) ARG();

    int FUNC(store) ARG(struct Resource *res);
    int FUNC(load) ARG(const char *key, struct Resource **res);
};

#endif
