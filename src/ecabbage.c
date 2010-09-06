#include "ecabbage.h"
#include "screen.h"

static void
m_dispose(THIS)
{
    METHOD(ECabbage);
    DELETE(ECabbage, this);
}

static const SDL_Surface *
m_getTile(THIS)
{
    /* METHOD(ECabbage); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Cabbage, 0);
}

CTOR(ECabbage)
{
    Entity e;

    BASECTOR(ECabbage, Entity);

    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getTile = &m_getTile;
    e->bg = BG_Earth;
    return this;
}

DTOR(ECabbage)
{
    BASEDTOR(Entity);
}

