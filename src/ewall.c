#include "ewall.h"
#include "board.h"

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(EWall);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getWallTile;
}

static void
m_dispose(THIS)
{
    METHOD(EWall);
    DELETE(EWall, this);
}

CTOR(EWall)
{
    Entity e;

    BASECTOR(EWall, Entity);

    e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(EWall)
{
    BASEDTOR(Entity);
}

