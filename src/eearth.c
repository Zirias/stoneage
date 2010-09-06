#include "eearth.h"
#include "board.h"
#include "screen.h"

static void
m_dispose(THIS)
{
    METHOD(EEarth);
    DELETE(EEarth, this);
}

CTOR(EEarth)
{
    Entity e;

    BASECTOR(EEarth, Entity);

    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->bg = BG_Earth;
    return this;
}

DTOR(EEarth)
{
    BASEDTOR(Entity);
}

