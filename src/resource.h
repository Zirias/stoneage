#ifndef STONEAGE_RESOURCE_H
#define STONEAGE_RESOURCE_H

#include <stdio.h>

#include "common.h"

struct Resource_impl;

CLASS(Resource)
{
    INHERIT(Object);

    struct Resource_impl *pimpl;

    void FUNC(setName) ARG(const char *name);
    const char *FUNC(getName) ARG();

    int FUNC(giveData) ARG(void *data, size_t dataSize);
    int FUNC(copyDataFrom) ARG(void *data, size_t dataSize);
    int FUNC(readDataFrom) ARG(FILE *file, size_t dataSize);

    size_t FUNC(getDataSize) ARG();
    const void *FUNC(getData) ARG();
    void *FUNC(takeData) ARG();

    void FUNC(deleteData) ARG();
};

#endif
