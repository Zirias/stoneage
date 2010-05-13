#include "common.h"
#include "entity.h"

int
main(int argc, char **argv)
{
    Entity e = NEW(Entity);
    Object o = CAST(e, Object);
    e = CAST(o, Entity);
    DELETE(Entity, e);
    o = NEW(Object);
    e = CAST(o, Entity);
}

