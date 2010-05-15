#include "ecabbage.h"

static void
FUNC(parent_init) ARG(Board b, int x, int y);

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(ECabbage);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getCabbageTile;
    e->getBaseSurface = b->getEarthTile;
}

static void
m_dispose ARG()
{
    METHOD(ECabbage);
    DELETE(ECabbage, this);
}

CTOR(ECabbage)
{
    BASECTOR(ECabbage, Entity);
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(ECabbage)
{
    BASEDTOR(Entity);
}

