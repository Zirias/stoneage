#include "ewilly.h"

static void
FUNC(parent_init) ARG(Board b, int x, int y);

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(EWilly);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getWillyTile;
    e->getBaseSurface = b->getEmptyTile;
}

static void
m_dispose ARG()
{
    METHOD(EWilly);
    DELETE(EWilly, this);
}

CTOR(EWilly)
{
    BASECTOR(EWilly, Entity);
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(EWilly)
{
    BASEDTOR(Entity);
}

