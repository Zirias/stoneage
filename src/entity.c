#include "entity.h"

CTOR(Entity)
{
    BASECTOR(Entity, Object);
    this->tile = NULL;
    return this;
}

DTOR(Entity)
{
    BASEDTOR(Object);
}

