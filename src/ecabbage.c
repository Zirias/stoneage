#include "ecabbage.h"
#include "board.h"
#include "screen.h"

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(ECabbage);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getBackground = b->getEarthBackground;
}

static void
m_dispose(THIS)
{
    METHOD(ECabbage);
    DELETE(ECabbage, this);
}

static const SDL_Surface *
m_getSurface(THIS)
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
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    e->getSurface = &m_getSurface;
    return this;
}

DTOR(ECabbage)
{
    BASEDTOR(Entity);
}

