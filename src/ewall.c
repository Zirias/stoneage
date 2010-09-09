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
m_getTile(THIS)
{
    /* METHOD(EWall); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Wall, 0);
}

CTOR(EWall)
{
    Entity e;

    BASECTOR(EWall, Entity);

    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getTile = &m_getTile;
    e->bg = BG_None;
    return this;
}

DTOR(EWall)
{
    BASEDTOR(Entity);
}

