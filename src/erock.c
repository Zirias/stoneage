#include "erock.h"

static void
FUNC(parent_init) ARG(Board b, int x, int y);

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(ERock);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getRockTile;
    e->getBaseSurface = b->getEmptyTile;
}

static void
m_dispose ARG()
{
    METHOD(ERock);
    DELETE(ERock, this);
}

CTOR(ERock)
{
    BASECTOR(ERock, Entity);
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(ERock)
{
    BASEDTOR(Entity);
}

