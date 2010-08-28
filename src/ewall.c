#include "ewall.h"
#include "board.h"
#include "screen.h"

static void
m_dispose(THIS)
{
    METHOD(EWall);
    DELETE(EWall, this);
}

static const SDL_Surface *
m_getSurface(THIS)
{
    METHOD(EWall);

    Screen s = getScreen();
    struct TileId id = { SATN_Wall, 0 };
    return s->getTile(s, id);
}

CTOR(EWall)
{
    Entity e;

    BASECTOR(EWall, Entity);

    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getSurface = &m_getSurface;
    return this;
}

DTOR(EWall)
{
    BASEDTOR(Entity);
}

