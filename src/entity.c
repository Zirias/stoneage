#include "entity.h"

Entity
Entity_ctor(Entity e, TypeList types)
{
    OBJECT_INIT(e, Entity);
    e->tile = NULL;
    return e;
}

void
Entity_dtor(Entity e)
{
    Object_dtor((Object)e);
}

