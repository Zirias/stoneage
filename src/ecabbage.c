#include "ecabbage.h"
#include "board.h"

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(ECabbage);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getCabbageTile;
    e->getBaseSurface = b->getEarthBaseTile;
}

static void
m_dispose(THIS)
{
    METHOD(ECabbage);
    DELETE(ECabbage, this);
}

CTOR(ECabbage)
{
    Entity e;

    BASECTOR(ECabbage, Entity);

    e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(ECabbage)
{
    BASEDTOR(Entity);
}

