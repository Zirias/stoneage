#include "eearth.h"
#include "board.h"
#include "screen.h"

static void
m_dispose(THIS)
{
    METHOD(EEarth);
    DELETE(EEarth, this);
}

static const SDL_Surface *
m_getSurface(THIS)
{
    METHOD(EEarth);

    Screen s = getScreen();
    struct TileId id = { SATN_Earth, 0 };
    return s->getTile(s, id);
}

CTOR(EEarth)
{
    Entity e;

    BASECTOR(EEarth, Entity);

    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getSurface = &m_getSurface;
    return this;
}

DTOR(EEarth)
{
    BASEDTOR(Entity);
}

