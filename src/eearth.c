#include "eearth.h"
#include "board.h"

static void
FUNC(parent_init) ARG(Board b, int x, int y);

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(EEarth);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getEarthTile;
}

static void
m_dispose ARG()
{
    METHOD(EEarth);
    DELETE(EEarth, this);
}

CTOR(EEarth)
{
    BASECTOR(EEarth, Entity);
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(EEarth)
{
    BASEDTOR(Entity);
}

