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
    /* METHOD(EEarth); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Earth, 0);
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

