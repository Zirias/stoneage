#include "ewilly.h"
#include "board.h"

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(EWilly);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getWillyTile;
    e->getBaseSurface = b->getEmptyTile;
}

static void
m_dispose(THIS)
{
    METHOD(EWilly);
    DELETE(EWilly, this);
}

CTOR(EWilly)
{
    Entity e;

    BASECTOR(EWilly, Entity);

    e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(EWilly)
{
    BASEDTOR(Entity);
}

