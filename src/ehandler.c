#include "ehandler.h"

CTOR(EHandler)
{
    BASECTOR(EHandler, Object);
    this->handleEvent = 0;
    return this;
}

DTOR(EHandler)
{
    BASEDTOR(Object);
}

