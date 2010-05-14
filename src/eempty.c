#include "eempty.h"

static void
FUNC(parent_init) ARG(Board b, int x, int y);

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(EEmpty);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->tile = b->getEmptyTile(b);
}

static void
m_dispose ARG()
{
    METHOD(EEmpty);
    DELETE(EEmpty, this);
}

CTOR(EEmpty)
{
    BASECTOR(EEmpty, Entity);
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(EEmpty)
{
    BASEDTOR(Entity);
}

